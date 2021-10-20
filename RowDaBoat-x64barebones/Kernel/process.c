#include "include/process.h"
#include "interrupts/int80.h"
#include "drivers/video_driver.h"

process_t* processes[MAX_PROCESSES] = {NULL};
size_t process_count = 0, started = 0;
process_t* curr_process = NULL;

static QUEUE_HD queues[2][MAX_PRIORITY - MIN_PRIORITY + 1];
static QUEUE_HD * act_queue; //queue with active processes
static QUEUE_HD * exp_queue; //queue with those processes that have expired their quantum or are new or priority changed
unsigned quantum[MAX_PRIORITY - MIN_PRIORITY + 1];

static int actual_queue = 0;
static unsigned int prior = 0;
int quantum_started = 0;
unsigned int processes_size = 0;

static void updateCurrent(void);
static void expireCurrent(void);
static void * getNextProcess(void * rsp);
static void enqueueProcess(process_t * process);

static stackProcess stackModel = {15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0x8, 0x202, 0, 0};


void * scheduler(void * rsp) {
    draw_string("Scheduler\n", 11);

    if(!quantum_started) {
        initQuantums();
        quantum_started = 1;
    }

    if(process_count > 0) {
        draw_string("Hay proceso\n", 13);
        if (curr_process != NULL) { //process running
            (curr_process->given_time--);

            if (curr_process->given_time == 0) {
                act_queue = queues[actual_queue];
                exp_queue = queues[1 - actual_queue];

                updateCurrent();
                curr_process->rsp = rsp;
                expireCurrent();
            }
        }
        draw_string("Proximo proceso\n", 17);
        getNextProcess(rsp);
    } else {
        draw_string("No hay proceso\n", 16);
        return rsp;
    }
}

static void initQuantums() {
    for (unsigned int p = 0; p < MAX_PRIORITY - MIN_PRIORITY + 1; p++) {
        quantum[p] = PRIOR_SLOPE * p + PRIOR_INDVAR;
    }
}

static void updateCurrent(void) {
    //update info
    (curr_process->aging)++;

    //update act_queue
    act_queue[prior].first = curr_process->next_in_queue;
    curr_process->next_in_queue = NULL;
}

static void expireCurrent(void) {
    //if aging is used then go to a lower priority queue
    #ifdef _RR_AGING_ 
    // set new priority
    changePriority(curr_process->pid, (prior < MAX_PRIOR)? prior+1 : prior);
    #endif

    if (exp_queue[curr_process->priority].last != NULL)
        (exp_queue[curr_process->priority].last)->next_in_queue = curr_process;
    exp_queue[curr_process->priority].last = curr_process;
}

static void * getNextProcess(void * rsp) {
    while (prior <= MAX_PRIORITY && act_queue[prior].first == NULL) //go to a queue with lower priority
    prior++;

    if(prior < MAX_PRIORITY){
        curr_process = act_queue[prior].first;
        curr_process->given_time = act_queue[prior].quantum;
    } else {
        actual_queue = 1 - actual_queue; // change to expired queue
        prior = 0;
        curr_process = NULL;
        return scheduler(rsp); // do it again in expirer queue
    }
}

static void enqueueProcess(process_t * process) {
    process->next_in_queue = exp_queue[process->priority].first;
    exp_queue[process->priority].first = process;
    if(exp_queue[process->priority].last == NULL) {
        exp_queue[process->priority].last = process;
    }
}



pid_t setPid() {
    pid_t current_pid = ERROR;
    for (int current_process = 0; current_process < MAX_PROCESS_COUNT; current_process++) {
        if (processes[current_process] == NULL) {
            current_pid = current_process;
            break;
        }
    }
    return current_pid;
}

size_t set_pName(process_t * process, char *name) {
    size_t size = Strlen(name) + 1;
    process->process_name = (char *) MyMalloc(size);
    if (process->process_name == NULL) 
        return MEMORY_OVERFLOW;
    Strncpy(process->process_name, name,0,size);
    return size;
}

int getArgvCount(char **argv) {
    int count = 0;
    if (argv == NULL) 
        return count;
    while (argv[count] != NULL) {
        count++;
    }
    return count;
}

pid_t pCreate(char *name, main_func_t * main_f, size_t stack, size_t heap) { // int (*code)(int, char **) el parametro de _start_process
    //falta: back or foreground, *funcion con argc y argv, 
    draw_string("Creando proceso", 16);
    pid_t pid = setPid();
    if (pid == ERROR || main_f->f == NULL) 
        return ERROR;
    process_t* process = (process_t*) MyMalloc(sizeof(process_t));

    if (process == NULL) 
        return MEMORY_OVERFLOW;
    process->pid = pid;
    process->status = READY;

    if (set_pName(process, name) == MEMORY_OVERFLOW) 
        return MEMORY_OVERFLOW;

    if (heap == 0) {
        process->heap.base = NULL;
        process->heap.size = heap;
    }
    else {
        process->heap.base = (address_t) MyMalloc(heap);
        process->heap.size = heap;
    }

    if (stack == 0) {
        process->stack.base = NULL;
        process->stack.size = 0;
    }
    else {
        process->stack.base = (address_t) MyMalloc(stack);
        process->stack.size = stack;
    }

    process->rbp = (void*) (((uint64_t) process->stack.base + MAX_STACK) & -8);
    process->rsp = (void*) ((((uint64_t) process->stack.base + MAX_STACK) & -8) - (REGS_SIZE + STATE_SIZE) * sizeof(uint64_t)); //se quejaba si le poniamos process->rbp - ...

    //Preparar el stack
    prepareStack(main_f->f, main_f->argc, main_f->argv, process->rbp, process->rsp);

    enqueueProcess(&(processes[process->pid]));

    processes[process->pid] = process;
    process_count++;

    if (!started) started = 1;
    draw_hex(stackModel.rip);
    return process->pid;
}

static void prepareStack(int (*main)(int argc, char ** argv), int argc, char ** argv, void * rbp, void * rsp) { //no le pasamos rsp ya que es igual al rbp
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
    return set_pStatus(pid, KILLED);
}

process_t* get_process_by_id(pid_t pid) {
    return processes[pid];
}

pStatus get_pStatus(pid_t pid) {
    if(pid >= MAX_PROCESS_COUNT || pid < 0 || processes[pid] == NULL) 
        return ERROR;
    
    return processes[pid]->status;
}

pid_t set_pStatus(pid_t pid, pStatus status) {
    if(pid >= MAX_PROCESS_COUNT || pid < 0 || processes[pid] == NULL) 
        return ERROR;
    processes[pid]->status = status;
    return pid;
}

void free_process(pid_t pid) {
    if (processes[pid]==NULL)
        return;

    if (processes[pid]->process_name != NULL) 
        MyFree(processes[pid]->process_name);

    if (processes[pid]->heap.base != NULL) 
        MyFree(processes[pid]->heap.base);

    if (processes[pid]->stack.base != NULL)
        MyFree(processes[pid]->stack.base);

    MyFree(processes[pid]);
    processes[pid] = NULL;
}