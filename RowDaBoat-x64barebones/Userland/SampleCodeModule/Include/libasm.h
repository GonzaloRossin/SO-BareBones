#ifndef LIBASM_H
#define LIBASM_H
#include <stdint.h>


//SYSCALL 0 copy  count chars from the keyboard or waits until it has them, to the buffer.
void sys_read(uint64_t buffer, uint64_t count);

//SYSCALL 1 writes in screen content from buffer.
void sys_write(uint64_t buffer, uint64_t count);

#endif