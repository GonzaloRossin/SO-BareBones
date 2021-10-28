#include <tests.h>
#include "Include/shell.h"

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
  //return (pid_t)exec();
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

