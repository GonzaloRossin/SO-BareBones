#ifndef LIB_H
#define LIB_H

#include <stdint.h>

void print(char *buffer);
void putChar(char c);
int strlen(char *str);
int strcmp(const char *str1, const char *str2);
void strncpy(char *source, char *dest,int index, int size);
void newLine();
void numToChar(double num, char * c);
void putChar(char c);
void actionCall(int action);
uint8_t * strToNumHex(char * str);
int abs(int i);

typedef struct matrix_struct
{
	int x;
	int y;
	unsigned char * matrix;
	char caracter;
	char * buffer;
	int count;
	int width;
	int height;
	int draw_size;
	int color;
	int backgroundcolor;
} matrix_struct;

void draw(int option, matrix_struct * m);
void set_cursor(int x, int y);

#endif