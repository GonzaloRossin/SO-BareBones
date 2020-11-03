#ifndef _SHELL_H
#define _SHELL_H

#define BACKSPACE 0x08
#define ENTER 0x0A
#define CTRL 0x1D

void shell();
void get_CPUinfo();
void inforeg();
void cleanBuffer();
void testDivisionBy0Command();
int readNewInput();

#endif