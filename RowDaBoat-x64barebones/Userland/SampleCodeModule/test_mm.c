#include <test_mm.h>
#include "Include/Lib.h"
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
      get_mem_info();
      if(mm_rqs[rq].address==NULL){
        print("malloc returns NULL");
      }
      total += mm_rqs[rq].size;
      rq++;
    }
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
            get_mem_info();
        }
      }
    }
}

void test_mem(){
  print("just begun");
  newLine();
  Mmem();
  newLine();

  mm_rq mm_rqs[3];
  for (int i=0; i<3; i++){
    newLine();
    print("iter ");
    print_num(i,0);
    newLine();

    mm_rqs[i].size = 5;
    mm_rqs[i].address = Mmalloc(1024);
    if(mm_rqs[i].address==NULL){
        print("malloc returns NULL");
    }
    Mmem();
  }

    for (int i=0; i<3; i++){
    newLine();
    print("iter ");
    print_num(i,0);
    newLine();

    Mfree(mm_rqs[i].address);
    if(mm_rqs[i].address==NULL){
        print("malloc returns NULL");
    }
    Mmem();
  }
  

}
