#ifndef LIBASM_H
#define LIBASM_H
#include <stdint.h>


//syscalls that return void
uint64_t sysCall1(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx);

#endif