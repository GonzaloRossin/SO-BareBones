#ifndef LIB_H
#define LIB_H

#include <stdint.h>

#define START_ADDRESS 0x0000000000600000
#define END_ADDRESS 0xFFFFFFFFFFFFFFFF

void initializeManager();
void* malloc(int size);

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

#endif