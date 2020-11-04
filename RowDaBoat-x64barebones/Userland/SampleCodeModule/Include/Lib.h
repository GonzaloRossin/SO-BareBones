#ifndef LIB_H
#define LIB_H

#include <stdint.h>

void print(char *buffer);
void putChar(char c);
int strlen(char *str);
int strcmp(const char *str1, const char *str2);
void strncpy(char *source, char *dest, int size);
void newLine();
void numToChar(double num, char * c);
void putChar(char c);
void actionCall(int action);

//Structure for drawing
//#pragma pack(push)
//#pragma pack(1)
typedef struct matrix_struct
{
	unsigned char * matrix;
	int width;
	int height;
	int draw_size;
	int color;
	int backgroundcolor;
	int contourcolor;
} matrix_struct;
//#pragma pack(push)

void draw(int option, matrix_struct * m);

#endif