#include "Include/tests.h"
#include "Include/shell.h"
#include "Include/lib.h"
#include "Include/test_util.h"

#define MAX_BLOCKS 128
#define MAX_MEMORY 8192 //Should be around 80% of memory managed by the MM

typedef struct MM_rq{
  void *address;
  uint32_t size;
}mm_rq;

void test_mm(){
  mm_rq mm_rqs[MAX_BLOCKS];
  uint8_t rq;
  uint32_t total;

    rq = 0;
    total = 0;
    // Request as many blocks as we can
    print("allocating space...");
    newLine();
    newLine();
    while(rq < MAX_BLOCKS && total < MAX_MEMORY){
      mm_rqs[rq].size = GetUniform(MAX_MEMORY - total - 1) + 1;
      mm_rqs[rq].address = Mmalloc(mm_rqs[rq].size); // TODO: Port this call as required
      if(mm_rqs[rq].address==NULL){
        print("malloc returns NULL");
      }
      total += mm_rqs[rq].size;
      rq++;
    }
    get_mem_info();
    // Set
    uint32_t i;
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address != NULL)
        MemSet(mm_rqs[i].address, i, mm_rqs[i].size); // TODO: Port this call as required
      else{
        print("pointer is null");
        newLine();
      }
    // Print Mem state
    newLine();
    print("freeing...");
      newLine();
    // Check
    for (i = 0; i < rq; i++){
      if (mm_rqs[i].address != NULL)
        if(!memcheck(mm_rqs[i].address, i, mm_rqs[i].size))
          print("ERROR!\n"); // TODO: Port this call as required
      for (i = 0; i < rq; i++){
        if (mm_rqs[i].address != NULL){
            Mfree(mm_rqs[i].address);  // TODO: Port this call as required
        }
      }
    }
    get_mem_info();
}

uint64_t my_create_process(char * name){
  return 0;
}

uint64_t my_sem_open(char *sem_id, uint64_t initialValue){
  return (uint64_t) s_init(sem_id,(unsigned int)initialValue);
}

uint64_t my_sem_wait(sem_id semId){
  return (uint64_t) s_wait(semId);
}

uint64_t my_sem_post(sem_id semId){
  return (uint64_t)s_post(semId);
}

uint64_t my_sem_close(sem_id semId){
  return (uint64_t) s_close(semId);
}

#define TOTAL_PAIR_PROCESSES 2
#define SEM_ID "sem"

int64_t global;  //shared memory

void slowInc(int64_t *p, int64_t inc){
  int64_t aux = *p;
  aux += inc;
  //yield();
  *p = aux;
}

void inc(uint64_t sem, int64_t value, uint64_t N){
  uint64_t i;

  if (sem && !my_sem_open(SEM_ID, 1)){
    print("ERROR OPENING SEM");
    newLine();
    return;
  }
  
  for (i = 0; i < N; i++){
    if (sem) my_sem_wait(SEM_ID);
    slowInc(&global, value);
    if (sem) my_sem_post(SEM_ID);
  }

  if (sem) my_sem_close(SEM_ID);
  
  print("Final value: ");
  print_num(global,0);
  newLine();
}

void test_sync(){
  uint64_t i;

  global = 0;

  print("CREATING PROCESSES...(WITH SEM)");
  newLine();

  for(i = 0; i < TOTAL_PAIR_PROCESSES; i++){
    //my_create_process("inc", 1, 1, 1000000);
   // my_create_process("inc", 1, -1, 1000000);
  }
}

void test_no_sync(){
  uint64_t i;

  global = 0;

  print("CREATING PROCESSES...(WITHOUT SEM)");
  newLine();

  for(i = 0; i < TOTAL_PAIR_PROCESSES; i++){
   // my_create_process("inc", 0, 1, 1000000);
    //my_create_process("inc", 0, -1, 1000000);
  }
}




//////////////////////////
// Test process  /////////
//////////////////////////


//TO BE INCLUDED
void endless_loop(){
  while(1);
}

#define MAX_PROCESSES 5 //Should be around 80% of the the processes handled by the kernel

typedef struct P_rq{
  uint32_t pid;
  unsigned int state;
} p_rq;

void test_processes(){
  p_rq p_rqs[MAX_PROCESSES];
  uint8_t rq;
  uint8_t alive = 0;
  uint8_t action;

  //while (1) {

    // Create MAX_PROCESSES processes
    for(rq = 0; rq < MAX_PROCESSES; rq++){
      main_func_t endfun = {endless_loop, 0, NULL};
      p_rqs[rq].pid = exec(&endfun, "Endless Loop", 0); // TODO: Port this call as required

      if (p_rqs[rq].pid == -1){                           // TODO: Port this as required
        print("Error creating process\n");               // TODO: Port this as required
        return;
      } else {
        p_rqs[rq].state = READY;
        alive++;
      }
    }
    // Randomly kills, blocks or unblocks processes until every one has been killed
    while (alive > 0){

      for(rq = 0; rq < MAX_PROCESSES; rq++){
        action = GetUniform(2) % 2; 

        switch(action){
          case 0:
            if (p_rqs[rq].state == READY || p_rqs[rq].state == BLOCKED) {
              if (kill(p_rqs[rq].pid) == -1){          // TODO: Port this as required
                print("Error killing process\n");        // TODO: Port this as required
                return;
              }
              p_rqs[rq].state = KILLED; 
              alive--;
            }
            break;

          case 1:
            if (p_rqs[rq].state == READY){
              if(block(p_rqs[rq].pid, BLOCKED) == -1){          // TODO: Port this as required
                print("Error blocking process\n");       // TODO: Port this as required
                return;
              }
              p_rqs[rq].state = BLOCKED; 
            }
            break;
        }
      }

      // Randomly unblocks processes
      for(rq = 0; rq < MAX_PROCESSES; rq++)
        if (p_rqs[rq].state == BLOCKED && GetUniform(2) % 2) {
          if(block(p_rqs[rq].pid, READY) == -1) {            // TODO: Port this as required
            print("Error unblocking process\n");         // TODO: Port this as required
            return;
          }
          p_rqs[rq].state = READY; 
        }
    }
    print("Finished"); 
  //}
}