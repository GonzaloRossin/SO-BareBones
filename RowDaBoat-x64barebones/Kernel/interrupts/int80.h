#ifndef __INT80__
#define __INT80__
#include <stdint.h>

// Structure for drawing
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

// the int 80h dispatcher
uint64_t int80Dispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx);

char sys_read();			//sys call 0
void sys_write(char * rsi, int rdx);			//sys call 1
void sys_getMemory(uint8_t rsi, uint8_t* rdx); //sys call 2
void sys_newline();                         //sys call 3
void sys_cpuInfo(char* rsi , uint32_t* rdx);//sys call 4
void sys_get_InfoReg(uint64_t* rsi);//sys call 5
void sys_put_char(char rsi); //sys call 6
void sys_action_call(int rsi);//sys call 7
uint8_t sys_get_clock(int rsi);//sys call 8
void sys_print_num(int rsi,int rdx);//sys call 9
void sys_clear_screen();//sys call 10
void sys_draw(int rsi, matrix_struct * rdx);//sys call 11

#endif