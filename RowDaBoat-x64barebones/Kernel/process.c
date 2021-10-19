#include "include/process.h"
#include "interrupts/int80.h"

process_t* processes[MAX_PROCESS_COUNT] = {NULL};
size_t process_count = 0;

pid_t setPid() {
    pid_t current_pid = ERROR;
    for(int current_process = 0; current_process < MAX_PROCESS_COUNT; current_process++) {
        if(processes[current_process] == NULL) {
            current_pid = current_process;
            break;
        }
    }
    return current_pid;
}

size_t set_pName(process_t * process, char *name) {
    size_t size=strlen(name)+1;
    process->process_name = (char *) MyMalloc(size);
    if(process->process_name == NULL) 
        return MEMORY_OVERFLOW;
    strncpy(process->process_name, name,0,size);
    return size;
}

int getArgvCount(char **argv) {
    int count = 0;
    if(argv == NULL) 
        return count;
    while(argv[count] != NULL) {
        count++;
    }
    return count;
}

pid_t pCreate(char *name, address_t code, char **argv, size_t stack, size_t heap){

    pid_t pid = setPid();
    if(pid == ERROR || code == NULL) 
        return ERROR;
    process_t* process = (process_t*) MyMalloc(sizeof(process_t));

    if(process == NULL) 
        return MEMORY_OVERFLOW;
    process->pid = pid;
    process->status = ACTIVE;
    if(set_pName(process, name) == MEMORY_OVERFLOW) 
        return MEMORY_OVERFLOW;
    if(heap==0){
        process->heap.base=NULL;
        process->heap.size=heap;
    }
    else{
        process->heap.base=(address_t) MyMalloc(heap);
        process->heap.size=heap;
    }
    if(stack==0){
        process->stack.base=NULL;
        process->stack.size=0;
    }
    else{
        process->stack.base=(address_t) MyMalloc(stack);
        process->stack.size=stack;
    }
    process->code_pos=code;
    process->arguments.argv=argv;
    process->arguments.argc=getArgvCount(argv);
    processes[process->pid] = process;
    process_count++;
    return process->pid;
}

process_t* get_process_by_id(pid_t pid){
    return processes[pid];
}

pStatus get_pStatus(pid_t pid){
    if(pid >= MAX_PROCESS_COUNT || pid < 0 || processes[pid] == NULL) 
        return ERROR;
    
    return processes[pid]->status;
}

pid_t set_pStatus(pid_t pid, pStatus status){
    if(pid >= MAX_PROCESS_COUNT || pid < 0 || processes[pid] == NULL) 
        return ERROR;
    processes[pid]->status = status;
    return pid;
}

void free_process(pid_t pid) {
    if(processes[pid]==NULL)
        return;
    if(processes[pid]->process_name != NULL) 
        MyFree(processes[pid]->process_name);
    if(processes[pid]->heap.base != NULL) 
        MyFree(processes[pid]->heap.base);
    if(processes[pid]->stack.base!=NULL)
        MyFree(processes[pid]->stack.base);
    MyFree(processes[pid]);
    processes[pid] = NULL;
}