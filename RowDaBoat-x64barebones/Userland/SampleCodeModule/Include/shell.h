#ifndef _SHELL_H
#define _SHELL_H

#define BACKSPACE 0x08
#define ENTER 0x0A
#define CTRL 0x1D
#define ESC 0x1B
#define TAB 0X09

void shell();
void get_mem_info();
void sleep(int seg);

#endif