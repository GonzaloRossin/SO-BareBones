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

#include <stdint.h>

#define N 100

uint64_t my_create_process(int (f)(int, char**), char * name){
  uint64_t pid = 0; 
  main_func_t f_ps = {f, 0, NULL};
  int aux = exec( &f_ps, name,1); //CHECKEAR ERROR!!!!!
  if(aux < 0) {
    print("ERROR: createProcess!\n");
  }
  return aux;
}

uint64_t my_sem_open(char *sem_id, uint64_t initialValue){
  int aux=(int)s_open(sem_id,initialValue);
  if(aux<0)
    print("sem_open_error");
  return aux;
}

uint64_t my_sem_wait(uint64_t sem_id){
  int aux;
  if((aux=(int)s_wait(sem_id))<0)
    print("sem_wait_error");
  return aux;
}

uint64_t my_sem_post(uint64_t sem_id){
  int aux;
  if((aux=(int)s_post(sem_id))<0)
    print("sem_post_error");
  return s_post(sem_id);
}

uint64_t my_sem_close(uint64_t sem_id){
  int aux;
  if((aux=(int)my_sem_close(sem_id))<0)
    print("sem_close_error");
  return s_close(sem_id);
}

#define TOTAL_PAIR_PROCESSES N
#define SEM_NAME "sem"

int64_t global;  //shared memory

void slowInc(int64_t *p, int64_t inc){
  uint64_t aux = *p;
  aux += inc;
  for(int i = 0; i < 100 ; i++){}
  *p = aux;
}

static int my_process_inc(int argc, char ** argv){
  uint64_t i;
  sem_id sem;

  if ((sem = my_sem_open(SEM_NAME, 1)) < 0){
    print("ERROR OPENING SEM\n");
    return -1;
  }
  
  for (i = 0; i < N; i++){
    my_sem_wait(sem);
    slowInc(&global, 1);
    my_sem_post(sem);
  }

  my_sem_close(sem);
  
  print("SEM Final value: ");
  print_num(global,0);
  newLine();

  return 0;
}

static int my_process_dec(int argc, char ** argv){
  uint64_t i;
  sem_id sem; 

  if ((sem = my_sem_open(SEM_NAME, 1)) < 0){
    print("ERROR OPENING SEM\n");
    return -1;
  }
  
  for (i = 0; i < N; i++){
    my_sem_wait(sem);
    slowInc(&global, -1);
    my_sem_post(sem);
    print("Variable is: ");
    print_num(global,0);
    newLine();
  }

  my_sem_close(sem);

  print("SEM Final value: ");
  print_num( global,0);

  return 0;
}

void test_sync(){
  uint64_t i;
  
  global = 0;

  print("CREATING PROCESSES...\n");
  s_init();

  for(i = 0; i < TOTAL_PAIR_PROCESSES; i++){
    my_create_process(my_process_inc, "my_process_inc");
    my_create_process(my_process_dec,"my_process_dec");
  }
  
  // The last one should print 0
}

static int my_process_inc_no_sem(int argc, char ** argv){
  uint64_t i;
  for (i = 0; i < N; i++){
    slowInc(&global, 1);
  }

  print("NO Final value: ");
  print_num(global,0);
  newLine();

  return 0;
}

static int my_process_dec_no_sem(int argc, char ** argv){
  uint64_t i;
  for (i = 0; i < N; i++){
    slowInc(&global, -1);
  }

  print("NO Final value:");
  print_num(global,0);
  newLine();

  return 0;
}

void test_no_sync(){
  uint64_t i;

  global = 0;

  print("CREATING PROCESSES...(WITHOUT SEM)\n");

  for(i = 0; i < TOTAL_PAIR_PROCESSES; i++){
    my_create_process(my_process_inc_no_sem, "my_process_inc_no_sem");
    my_create_process(my_process_dec_no_sem, "my_process_dec_no_sem");
  }
}

