#include <syscalls.h>
#include <video_driver.h>
#include <font.h>

void print_word(unsigned int x, unsigned int y, char * word, int lenght, int fontSize, int fontColor, int backgroundColor){

	for (int i = 0; i < lenght; i++){
		draw_char(x, y, word[i], fontSize, fontColor, backgroundColor);
        x += CHAR_WIDTH*FONT_SIZE;
	}
    
}
