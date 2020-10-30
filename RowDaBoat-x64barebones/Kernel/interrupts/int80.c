#include "interrupts/int80.h"
#include "drivers/video_driver.h"
#include "interrupts/interrupt_routines.h"
#include <stdint.h>
#include <font.h>

int cursor_x = 0;
int cursor_y = 0;

//Software interrupt used for interaction between user and kernel space
uint64_t int80Dispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rax)
{

	switch (rax)
	{
	//sys_read. Just reads from the keyboard and copies to the buffer.
	case 0:
		sys_read(rdi, (char *) rsi, rdx);
		break;
	// sys_write: writes stdout the content of buffer.
	case 1:
		sys_write(rdi, (char *) rsi, rdx);
		break;
	}

	return 0;
}

//SYS_CALL 0
void sys_read (uint64_t rdi, char * rsi, uint64_t rdx){
	
}

//SYS_CALL 1
void sys_write(uint64_t rdi, char * rsi, uint64_t rdx)
{
	if (cursor_x + CHAR_WIDTH*FONT_SIZE*rdx > SCREEN_WIDTH)
	{
		cursor_x = 0;
		cursor_y += CHAR_HEIGHT*FONT_SIZE;
	}
	for (int i = 0; i < rdx; i++){
		draw_char(cursor_x, cursor_y, rsi[i], FONT_SIZE, FONT_COLOR, BACKGROUND_COLOR);
        cursor_x += CHAR_WIDTH*FONT_SIZE;
		if (rsi[i]=='h')
		{
			cursor_x++;
		}
		
	}
	cursor_x += LINE_SPACE*FONT_SIZE;
}