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

#include <stdint.h>


static uint64_t my_create_process(int (f)(int, char**), char * name){
  uint64_t pid = 0;
  main_func_t f_ps = {f, 0, NULL};
  int aux = exec( &f_ps, name, 0); //CHECKEAR ERROR!!!!!
  if(aux < 0) {
    print("ERROR: createProcess!\n");
  }
  return pid;
}

static uint64_t my_sem_open(char *name, uint64_t initialValue){
  return s_open(name, initialValue);
}

static uint64_t my_sem_wait(uint64_t sem_id){
  int ret = (int)s_wait(sem_id);
  if (ret < 0)
    print("Se rompio el wait\n");
  return ret;
}

static uint64_t my_sem_post(uint64_t sem_id){
  int ret =(int) s_post(sem_id);
  if (ret < 0)
    print("Se rompio el post\n");
  return ret;
}

static uint64_t my_sem_close(uint64_t sem_id){
 return s_close(sem_id);
}

#define N 1000
#define SEM_NAME "sem"
#define TOTAL_PAIR_PROCESSES 2

uint64_t global;  //shared memory

static void slowInc(uint64_t *p, int inc){
  uint64_t aux = *p;
  aux += inc;
  *p = aux;
  _yield();
}

static int my_process_inc(int argc, char ** argv){
  uint64_t i;
  uint64_t sem;

  if ((sem = my_sem_open(SEM_NAME,1)) < 0){
    print("ERROR OPENING SEM\n");
    return -1;
  }
  for (i = 0; i < N; i++){
    my_sem_wait(sem);
    slowInc(&global, 1);
    my_sem_post(sem);
  }

  print("SEM Final value: ");
  print_num(global,0);
  newLine();

  return 0;
}

static int my_process_dec(int argc, char ** argv){
  uint64_t i;
  uint64_t sem;

  if ((sem = my_sem_open(SEM_NAME, 1)) < 0){
    print("ERROR OPENING SEM\n");
    return -1;
  }

  for (i = 0; i < N; i++){
    my_sem_wait(sem);
    slowInc(&global, -1);
    my_sem_post(sem);
  }

  print("SEM Final value: ");
  print_num(global,0);
  newLine();

  return 0;
}

static void test_sync(){
  uint64_t i;

  global = 0;

  print("CREATING PROCESSES...\n");

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

  print("SEM Final value: ");
  print_num(global,0);
  newLine();

  return 0;
}

static int my_process_dec_no_sem(int argc, char ** argv){
  uint64_t i;
  for (i = 0; i < N; i++){
    wait(500);
    slowInc(&global, -1);
  }

  print("SEM Final value: ");
  print_num(global,0);
  newLine();

  return 0;
}

static void test_no_sync(){
  uint64_t i;

  global = 0;

  print("CREATING PROCESSES...\n");

  for(i = 0; i < TOTAL_PAIR_PROCESSES; i++){
    my_create_process(my_process_inc_no_sem, "my_process_inc_no_sem");
    my_create_process(my_process_dec_no_sem, "my_process_dec_no_sem");
  }
  // The last one should not print 0
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

int main_test_sync(int argc, char ** argv){
  test_sync();
  return 0;
}

int main_test_no_sync(int argc, char **argv){
  test_no_sync();
  return 0;
}
