#include <libasm.h>
#include <lib.h>
#include <stdint.h>


void read(char *buffer, int length)
{
   sys_read((uint64_t)buffer, (uint64_t)length);
}

void print(char *buffer, int length)
{
   sys_write((uint64_t)buffer, (uint64_t)length);
}
