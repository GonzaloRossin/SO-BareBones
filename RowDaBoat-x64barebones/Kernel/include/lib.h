#ifndef LIB_H
#define LIB_H
#include <stdint.h>
#define NULL ((void *)0)



void * memset(void * destination, int32_t character, uint64_t length);
void * memcpy(void * destination, const void * source, uint64_t length);

char *cpuVendor(char *result);

void cpuVersion(uint32_t * result);

void saveRegs();

void getRegs(uint64_t * v);

void saveMemory(uint8_t* mem);

void getMemory(uint8_t * v);

uint8_t readCMOS(uint8_t reg);// read data from register of CMOS to use real time clock

uint8_t readSeconds();

uint8_t readMinutes();

uint8_t readHours();

int Strlen(char *str);

int Strcmp(const char *str1, const char *str2);

void Strncpy(char *source, char *dest,int index, int size);

int _start_process(int (*main)(int argc, char * argv), int argc, char * argv);

void _halt_and_wait(void);

void spin_lock(char * lock);

void spin_unlock(char * lock);

#endif