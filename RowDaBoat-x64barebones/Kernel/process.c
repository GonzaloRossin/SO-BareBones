#include "include/process.h"
#include "interrupts/int80.h"
#include "drivers/video_driver.h"

static void prepareStack(int (*main)(int argc, char ** argv), int argc, char ** argv, void * rbp, void * rsp);
static void * getNextProcess(void * rsp);
static void updateProcess(process_t * process);
static void enqueueProcess(process_t * process);
static void initQuantums(void);
static void createHalter(void);

process_t halter;
uint64_t halterStack[(STATE_SIZE + REGS_SIZE) + HALTER_EXTRA_STACK_SPACE];

static stackProcess stackModel = {15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0x8, 0x202, 0, 0};
process_t processes[MAX_PROCESSES];
QUEUE_HD queues[2][MAX_PRIORITY - MIN_PRIORITY + 1];
int actual_queue = 0;
QUEUE_HD * act_queue = NULL; //queue with active processes
QUEUE_HD * exp_queue = NULL; //queue with those processes that have expired their quantum or are new or priority changed
unsigned quantum[MAX_PRIORITY - MIN_PRIORITY + 1];
int initialized = 0;
process_t* curr_process = NULL;
unsigned int prior = 0;
int processes_size = 0; //array size
int processes_alive = 0; //without the killed ones
int processes_ready = 0; //without the killed ones
int processes_so_far = 0; // processes created, for the pid to be unique
int started = 0;

void printProcess(process_t process);

void * scheduler(void * rsp) {

    if(!initialized) {
        initQuantums();
        createHalter();
        initialized = 1;
    }

    if(curr_process != NULL || processes_ready > 0) {

        act_queue = queues[actual_queue];
        exp_queue = queues[1 - actual_queue];

        if (curr_process != NULL) { //process running
            (curr_process->given_time--);

            if (curr_process->given_time <= 0) {

                curr_process->aging++;

                updateProcess(curr_process);
                curr_process->rsp = rsp;

                #ifdef _RR_AGING_
                    // set new priority
                    changePriority(curr_process->pid, (prior < MAX_PRIOR)? prior+1 : prior);
                #endif
                if(curr_process->status != BLOCKED)
                    enqueueProcess(curr_process);
            } else {
                return rsp;
            }
        }
        return getNextProcess(rsp);
    } else {
         if (started)
            return halter.rsp;
        else
            return rsp;
    }
}

static void createHalter(void) {
    Strncpy("Halter", halter.name, 0, 6);
    halter.pid = 0;
    halter.ppid = 0;
    halter.foreground = 0;
    halter.priority = MAX_PRIORITY;
    halter.status = READY;
    halter.next_in_queue = NULL;
    halter.aging = 0;
    halter.given_time = quantum[BASE_PRIORITY];
    halter.stack = halterStack;
    halter.rbp = (void *)(((uint64_t) halter.stack + sizeof(halterStack)) & -8);
    halter.rsp = (void *) (halter.rbp - (STATE_SIZE + REGS_SIZE) * sizeof(uint64_t));

    stackModel.rbp = (uint64_t) halter.rbp;
    stackModel.rsp = (uint64_t) halter.rbp;
    stackModel.rip = (uint64_t) _halter;

    memcpy(halter.rsp, (void *) &stackModel, sizeof(stackModel));
}

static void initQuantums() {
    for (unsigned int p = 0; p < MAX_PRIORITY - MIN_PRIORITY + 1; p++) {
        quantum[p] = PRIOR_SLOPE * p + PRIOR_INDVAR;
    }
}

static void updateProcess(process_t *process) {

    //update act_queue, delete it from queue

    if (curr_process == process) {
        act_queue[prior].first = curr_process->next_in_queue;
        if (act_queue[prior].last == curr_process)
            act_queue[prior].last = NULL;
    }
    process->next_in_queue = NULL;
}

static void * getNextProcess(void * rsp) {
    while (prior <= MAX_PRIORITY && act_queue[prior].first == NULL) //go to a queue with lower priority
    prior++;

    if(prior <= MAX_PRIORITY) {
        if ((act_queue[prior].first)->status == KILLED || (act_queue[prior].first)->status == BLOCKED) {
            if (act_queue[prior].last == act_queue[prior].first)
                act_queue[prior].last = NULL;
            act_queue[prior].first = (act_queue[prior].first)->next_in_queue;
            return getNextProcess(rsp);
        }

        curr_process = act_queue[prior].first;
        curr_process->given_time = quantum[prior];
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


int pCreate(main_func_t * main_f, char *name, int foreground, int * pid) { // int (*code)(int, char **) el parametro de _start_process
    //falta: back or foreground, *funcion con argc y argv,
    //draw_string("Creando proceso", 16);
    int i = 0;

    if (!started)
        started = 1;

    while(i < processes_size && processes[i].status != KILLED)
        i++;

    if (i < MAX_PROCESSES) {
        int size = Strlen(name);
        Strncpy(name, processes[i].name, 0, size);
        processes[i].pid = (processes_so_far + 1);
        processes[i].ppid = (curr_process != NULL)?curr_process->pid:0;
        processes[i].foreground = (curr_process == NULL)?1:(curr_process->foreground)?foreground:0;
        processes[i].priority = BASE_PRIORITY;
        processes[i].status = READY;
        processes[i].next_in_queue = NULL;
        processes[i].aging = 0;
        processes[i].given_time = quantum[BASE_PRIORITY];

        processes[i].stack = MyMalloc(MAX_STACK);

        //draw_hex(0420);

        processes[i].rbp = (void *)(((uint64_t) processes[i].stack + MAX_STACK) & -8);
        processes[i].rsp = (void *) (processes[i].rbp - (STATE_SIZE + REGS_SIZE) * sizeof(uint64_t));
        //Preparar el stack

        if (i >= processes_size)
            processes_size++;

        prepareStack(main_f->f, main_f->argc, main_f->argv, processes[i].rbp, processes[i].rsp);


        enqueueProcess(&(processes[i]));

        processes_alive++;
        processes_ready++;
        processes_so_far++;
        *pid = processes[i].pid;

        if (curr_process != NULL && processes[i].foreground) { // PROBLEM
            changeStatus(curr_process->pid, BLOCKED);
        }

        return 0;
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
    kill(curr_process->pid);
    return 0;
}

int kill(int pid) {
    return changeStatus(pid, KILLED);
}

int getPid(int * pid) {
    *pid = curr_process->pid;
    return 0;
}

int getProcessesAlive(unsigned int * amount) {
    *amount =  processes_alive;
    return 0;
}

int changePriority(int pid, unsigned int new_priority) {
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

int changeStatus(int pid, unsigned int new_status) {
    for (int i = 0; i < processes_size; i++) {
        if (processes[i].pid == pid) {
            int last_status = processes[i].status;

            if (last_status == new_status || last_status == KILLED)
                return -1;

            processes[i].status = new_status;

            if (last_status == BLOCKED && new_status == READY) {
                enqueueProcess(&(processes[i]));
                processes_ready++;
            }
            else if (last_status == READY && new_status == BLOCKED) {
                processes_ready--;

                if (curr_process != NULL && curr_process->pid == pid) {
                    if (act_queue == NULL || exp_queue == NULL) {
                        act_queue = queues[actual_queue];
                        exp_queue = queues[1 - actual_queue];
                    }
                    curr_process->status = BLOCKED;
                    curr_process->given_time = 1;

                    _halt_and_wait();//wait next process
                } else {
                    updateProcess(&(processes[i]));
                }

            }
            else if (new_status == KILLED) {
                if (processes[i].foreground && processes[i].ppid > 0) {
                    changeStatus(processes[i].ppid, READY);
                }
                processes_alive--;
                if (last_status == READY)
                    processes_ready--;
                if (curr_process != NULL && curr_process->pid == pid) {
                    updateProcess(&(processes[i]));
                    curr_process = NULL;
                    MyFree(processes[i].stack);
                    _sti();
                    _int81();
                } else {
                    MyFree(processes[i].stack);
                }
            }
            return 0;
        }
    }
    return -1;
}

int changeForegorundStatus(int pid, unsigned int status) {
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

int getProcessStatus(int pid, unsigned int * status) {
    for(int i = 0; i < processes_size; i++) {
        if(processes[i].pid == pid) {
            *status = processes[i].status;
            return 0;
        }
    }
    return -1;
}

int getProcessesInfo(process_info * arr, unsigned int max_size, unsigned int * size) {
    unsigned int j = 0;
    for (unsigned int i = 0; i < processes_size && j < max_size; i++) {
        if (processes[i].status != KILLED) {
            Strncpy(processes[i].name, arr[j].name, 0, Strlen(processes[i].name));
            arr[j].foreground = processes[i].foreground;
            arr[j].priority = processes[i].priority;
            arr[j].status = processes[i].status;
            arr[j].pid = processes[i].pid;
            arr[j].ppid = processes[i].ppid;
            arr[j].rbp = processes[i].rbp;
            arr[j].rsp = processes[i].rsp;
            arr[j].aging = processes[i].aging;
            arr[j].given_time = processes[i].given_time;
            j++;
        }
    }
    *size = j;
    return 0;
}

int isCurrentForeground(void) {
    if (curr_process == NULL)
        return -1;
    return curr_process->foreground;
}
