#ifndef LIBASM_H
#define LIBASM_H
#include <stdint.h>

uint64_t sys_call(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx);

#endif