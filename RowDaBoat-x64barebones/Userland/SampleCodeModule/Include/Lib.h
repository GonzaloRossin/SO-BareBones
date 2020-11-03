#ifndef LIB_H
#define LIB_H

#include <stdint.h>

char getInput();
void print(char *buffer);
void putChar(char c);
int strlen(char *str);
int strcmp(const char *str1, const char *str2);
void strncpy(char *source, char *dest, int size);
void newLine();
void numToChar(double num, char * c);
void putChar(char c);
void actionCall(int action);

#endif