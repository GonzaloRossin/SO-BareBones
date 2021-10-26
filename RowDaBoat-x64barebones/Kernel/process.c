#include "include/process.h"
#include "interrupts/int80.h"
#include "drivers/video_driver.h"

process_t processes[MAX_PROCESSES];
size_t process_count = 0, processes_so_far = 0;
process_t* curr_process = NULL;

static QUEUE_HD queues[2][MAX_PRIORITY - MIN_PRIORITY + 1];
static QUEUE_HD * act_queue; //queue with active processes
static QUEUE_HD * exp_queue; //queue with those processes that have expired their quantum or are new or priority changed
unsigned quantum[MAX_PRIORITY - MIN_PRIORITY + 1];

static int actual_queue = 0;
static unsigned int prior = 0;
int quantum_started = 0;
unsigned int processes_size = 0;

static void updateProcess(process_t *process);
static void * getNextProcess(void * rsp);
static void enqueueProcess(process_t * process);
static void prepareStack(int (*main)(int argc, char ** argv), int argc, char ** argv, void * rbp, void * rsp);
static void initQuantums();

void printProcess(process_t process);

static stackProcess stackModel = {15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0x8, 0x202, 0, 0};


void * scheduler(void * rsp) {
    if(!quantum_started) {
        initQuantums();
        quantum_started = 1;
    }

    if(process_count > 0) {
        //draw_string("Hay proceso\n", 13);
        act_queue = queues[actual_queue];
        exp_queue = queues[1 - actual_queue];
        if (curr_process != NULL) { //process running
            (curr_process->given_time--);

            if (curr_process->given_time == 0) {

                updateProcess(curr_process);
                curr_process->rsp = rsp;

                #ifdef _RR_AGING_ 
                    // set new priority
                    changePriority(curr_process->pid, (prior < MAX_PRIOR)? prior+1 : prior);
                #endif
                enqueueProcess(curr_process);
            } else {
                return rsp;
            }
        }
        //draw_string("Proximo proceso\n", 17);
        return getNextProcess(rsp);
    } else {
        //draw_string("No hay proceso\n", 16);
        return rsp;
    }
}

static void initQuantums() {
    for (unsigned int p = 0; p < MAX_PRIORITY - MIN_PRIORITY + 1; p++) {
        quantum[p] = PRIOR_SLOPE * p + PRIOR_INDVAR;
    }
}

static void updateProcess(process_t *process) {
    //update info
    (process->aging)++;

    //update act_queue
    if (curr_process == process) {
        act_queue[prior].first = curr_process->next_in_queue;
        if (act_queue[prior].last == curr_process)
            act_queue[prior].last == NULL;
    }

    process->next_in_queue = NULL;
}

static void * getNextProcess(void * rsp) {
    while (prior <= MAX_PRIORITY && act_queue[prior].first == NULL) //go to a queue with lower priority
    prior++;

    if(prior <= MAX_PRIORITY) {
        if (act_queue[prior].first->status == KILLED) {
            act_queue[prior].first = (act_queue[prior].first)->next_in_queue;
            return getNextProcess(rsp);
        }

        curr_process = act_queue[prior].first;
        curr_process->given_time = act_queue[prior].quantum;
        return curr_process->rsp;
    
    } else {
        actual_queue = 1 - actual_queue; // change to expired queue
        prior = 0;
        curr_process = NULL;
        return scheduler(rsp); // do it again in expirer queue
    }
}

static void enqueueProcess(process_t * process) {
    if (act_queue == NULL || exp_queue == NULL) {
        act_queue = queues[actual_queue];
        exp_queue = queues[1 - actual_queue];
    }

    if (exp_queue[process->priority].first == NULL) {
        exp_queue[process->priority].first = process;
    }

    if (exp_queue[process->priority].last != NULL) {
        exp_queue[process->priority].last->next_in_queue = process;
    }
    exp_queue[process->priority].last = process;
}


pid_t pCreate(main_func_t * main_f, char *name, int foreground) { // int (*code)(int, char **) el parametro de _start_process
    //falta: back or foreground, *funcion con argc y argv, 
    //draw_string("Creando proceso", 16);
    int i = 0;
    while(i < processes_size && processes[i].status != KILLED)
        i++;

    if (i < MAX_PROCESSES) {
        int size = Strlen(name);
        Strncpy(name, processes[i].process_name, 0, size);
        processes[i].pid = (processes_so_far + 1);
        processes[i].ppid = (curr_process != NULL)?curr_process->pid:0; 
        processes[i].foreground = foreground;
        processes[i].priority = BASE_PRIORITY;
        processes[i].status = READY;
        processes[i].next_in_queue = NULL;
        processes[i].aging = 0; 
        processes[i].given_time = quantum[BASE_PRIORITY];

        processes[i].stack = MyMalloc(MAX_STACK);
        
        //draw_hex(0420);

        processes[i].rbp = (void *)(((uint64_t) processes[i].stack + MAX_STACK) & -8);
        processes[i].rsp = (void *) (&processes[i].rbp - (STATE_SIZE + REGS_SIZE) * sizeof(uint64_t));
        //Preparar el stack
        prepareStack(main_f->f, main_f->argc, main_f->argv, processes[i].rbp, processes[i].rsp);

        
        enqueueProcess(&(processes[i]));

        process_count++;
        processes_so_far++; 
        //draw_string(processes[i].process_name, 16);

        return processes[i].pid;
    }
    return -1;
}

static void prepareStack(int (*main)(int argc, char ** argv), int argc, char ** argv, void * rbp, void * rsp) {
    stackModel.rbp = (uint64_t) rbp;
    stackModel.rsp = (uint64_t) rbp;
    stackModel.rip = (uint64_t) _start_process; //de lib.h
    stackModel.rdi = (uint64_t) main; 
    stackModel.rsi = (uint64_t) argc;
    stackModel.rdx = (uint64_t) argv; 
    
    memcpy(rsp, (void *) &stackModel, sizeof(stackModel)); //rsp == rbp al comienzo
}

int exit() {
    return kill(curr_process->pid);
}

int kill(size_t pid) {
    return changeStatus(pid, KILLED);
}

int getPid(void) {
    return curr_process->pid;
}

unsigned int getProcessesCount(void) {
    return process_count; 
}

int changePriority(pid_t pid, unsigned int new_priority) {

    if (new_priority >= MIN_PRIORITY && new_priority <= MAX_PRIORITY) {
        for (int i = 0; i < processes_size; i++) {
            if (processes[i].pid == pid) {
                if (processes[i].status == KILLED)
                    return -1;
                processes[i].priority = new_priority;
                return 0;
            }
        }
    }
    return -1;
}

int changeStatus(pid_t pid, pStatus new_status) {
    for (int i = 0; i < processes_size; i++) {
        if (processes[i].pid == pid) {
            if (processes[i].status == KILLED)
                return -1;
            processes[i].status = new_status;
            if (new_status == KILLED) {
                updateProcess(&(processes[i]));
                if (curr_process != NULL && curr_process->pid == pid) {
                    curr_process = NULL;
                    _halt_and_wait();
                }
                MyFree(processes[i].stack);
                process_count--;
            }
            return 0;
        }
    }
    return -1;
}

int changeForegorundStatus(pid_t pid, unsigned int status) {
    for (int i = 0; i < processes_size; i++) {
        if (processes[i].pid == pid) {
            if (processes[i].status == KILLED)
                return -1;
            processes[i].foreground = status;
            return 0;
        }
    }
    return -1;
}

void printProcess(process_t process){
    sys_newline();
    char* line = " -----\n ";
    char* name = "name: ";
    draw_string(name, 6);
    draw_string(process.process_name, 16); //hacer que no sea numero fijo
    draw_string(line, 9);
    
    char* pid = "pid: ";
    draw_string(pid, 5);
    draw_decimal(process.pid);
    //sys_print_num(process->pid,0);
    draw_string(line, 7);

    char* priority = "priority: ";
    draw_string(priority, 10);
    draw_decimal((uint64_t) process.priority);
    //sys_print_num(process->priority,0);
    draw_string(line, 7);
    
    char* rbp = "rbp: ";
    draw_string(rbp, 5);
    draw_hex((uint64_t) process.rbp);
    //sys_print_num(process->rbp,0);
    draw_string(line, 7);
    
    char* rsp = "rsp: ";
    draw_string(rsp, 5);
    draw_hex((uint64_t) process.rsp);
    //sys_print_num(process->rsp,0);
    //draw_string(line, 7);

    sys_newline();
}

void PS(){
    int i;
    for(i = 0; i < process_count; i++) {
        printProcess(processes[i]);
    }
}



// pid_t set_pStatus(pid_t pid, pStatus status) {
//     if(pid >= MAX_PROCESS_COUNT || pid < 0) //processes[pid] == NULL
//         return ERROR;
//     processes[pid].status = status;
//     return pid;
// }

// void free_process(pid_t pid) {
//     if (processes[pid].pid == NULL)
//         return;

//     if (processes[pid].stack.base != NULL)
//         MyFree(processes[pid].stack.base);
// }

// process_t get_process_by_id(pid_t pid) {
//     return processes[pid];
// }

// pStatus get_pStatus(pid_t pid) {
//     if(pid >= MAX_PROCESS_COUNT || pid < 0 || processes[pid].pid == NULL) 
//         return ERROR;
    
//     return processes[pid].status;
// }

// pid_t setPid() {
//     pid_t current_pid = ERROR;
//     for (int current_process = 0; current_process < MAX_PROCESS_COUNT; current_process++) {
//         if (processes[current_process].pid == NULL) {
//             current_pid = current_process;
//             break;
//         }
//     }
//     return current_pid;
// }