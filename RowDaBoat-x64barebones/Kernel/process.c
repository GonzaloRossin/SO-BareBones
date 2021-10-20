#include "include/process.h"
#include "interrupts/int80.h"
#include "asm/libasm.asm"

process_t* processes[MAX_PROCESS_COUNT] = {NULL};
size_t process_count = 0;

static stackProcess stackModel = {15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0x8, 0x202, 0, 0};

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

pid_t pCreate(char *name, int (*code)(int, char **), char **argv, size_t stack, size_t heap) {
    //falta: back or foreground, *funcion con argc y argv, 

    pid_t pid = setPid();
    if (pid == ERROR || code == NULL) 
        return ERROR;
    process_t* process = (process_t*) MyMalloc(sizeof(process_t));

    if (process == NULL) 
        return MEMORY_OVERFLOW;
    process->pid = pid;
    process->status = ACTIVE;

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
    process->code_pos = code;
    process->arguments.argv = argv;
    process->arguments.argc = getArgvCount(argv);

    process->rbp = ((uint64_t) process->stack.base + MAX_STACK) & -8;
    process->rsp = ((uint64_t) ((process->stack.base + MAX_STACK) & -8) - (REGS_SIZE + STATE_SIZE) * sizeof(uint64_t)); //se quejaba si le poniamos process->rbp - ...

    //Preparar el stack
    prepareStack(code, process->arguments.argc, process->arguments.argv, process->rbp, process->rsp);

    processes[process->pid] = process;
    process_count++;
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