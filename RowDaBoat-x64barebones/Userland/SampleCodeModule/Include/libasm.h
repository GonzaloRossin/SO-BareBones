#ifndef LIBASM_H
#define LIBASM_H
#include <stdint.h>


//SYSCALL 0 copy  count chars from the keyboard or waits until it has them, to the buffer.
char read();

//SYSCALL 1 writes in screen content from buffer.
void sys_write(uint64_t buffer, uint64_t count);

//SYSCALL 2
void get_Memory(uint8_t mem, uint8_t * v); 
//SYSCALL 3,leaves a goes to the line below
void newline();
//SYSCALL 4,get cpuinfo
void getCpuVendor(char * c, uint32_t * v);
//SYSCALL 5,get infoReg
void get_InfoReg( uint32_t * reg_pointer);
//SYSCALL 6
void put_char( char c);
//SYSCALL 7
void putActioncall(int action);


#endif