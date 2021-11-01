// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include <stdint.h>
#include <font.h>
#include <video_driver.h>
#include "../include/lib.h"
#include "../pipes/pipe.h"


struct vbe_mode_info_structure {
	uint16_t attributes;		// deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
	uint8_t window_a;			// deprecated
	uint8_t window_b;			// deprecated
	uint16_t granularity;		// deprecated; used while calculating bank numbers
	uint16_t window_size;
	uint16_t segment_a;
	uint16_t segment_b;
	uint32_t win_func_ptr;		// deprecated; used to switch banks from protected mode without returning to real mode
	uint16_t pitch;			// number of bytes per horizontal line
	uint16_t width;			// width in pixels
	uint16_t height;			// height in pixels
	uint8_t w_char;			// unused...
	uint8_t y_char;			// ...
	uint8_t planes;
	uint8_t bpp;			// bits per pixel in this mode
	uint8_t banks;			// deprecated; total number of banks in this mode
	uint8_t memory_model;
	uint8_t bank_size;		// deprecated; size of a bank, almost always 64 KB but may be 16 KB...
	uint8_t image_pages;
	uint8_t reserved0;

	uint8_t red_mask;
	uint8_t red_position;
	uint8_t green_mask;
	uint8_t green_position;
	uint8_t blue_mask;
	uint8_t blue_position;
	uint8_t reserved_mask;
	uint8_t reserved_position;
	uint8_t direct_color_attributes;

	uint32_t framebuffer;		// physical address of the linear frame buffer; write here to draw to the screen
	uint32_t off_screen_mem_off;
	uint16_t off_screen_mem_size;	// size of memory in the framebuffer but not being displayed on the screen
	uint8_t reserved1[206];
} __attribute__ ((packed));

struct vbe_mode_info_structure * screendata=(void *)0x0000000000005C00;

static int cursor_x = 0;
static int cursor_y = 0;
int marginLeft=0;
int marginRight=SCREEN_WIDTH;
static int aux_x = 0;
static int aux_y = 0;
static uint32_t valueToBase(uint64_t value, char * buffer, uint32_t base);
char buffer[10] = { '0' };

char * get_pixel(unsigned int x, unsigned int y){
	return screendata->framebuffer+((y*screendata->width)+x)*3;
}

void draw_pixel(unsigned int x,unsigned int y,int color){
    char * curpos = screendata->framebuffer+((y*screendata->width)+x)*3;
    int b= color & 0x0000FF;
    int g= (color >> 8) &  0x0000FF;
    int r= (color >> 16) &  0x0000FF;
    *curpos=r;
    curpos++;
    *curpos=g;
    curpos++;
    *curpos=b;
    curpos++;
}

void draw_rectangle(unsigned int x, unsigned int y, int b, int h, int color){
	for (int i = 0; i < b; i++)
	{
		for (int j = 0; j < h; j++)
		{
			draw_pixel(x+i, y+j, color);
		}

	}

}
int show_cursor(int is_shown){
	if(is_shown){
		draw_rectangle(cursor_x,cursor_y,CHAR_WIDTH,CHAR_HEIGHT,BACKGROUND_COLOR);
		return 0;
	}else
		draw_rectangle(cursor_x,cursor_y,CHAR_WIDTH,CHAR_HEIGHT,FONT_COLOR);
	return 1;
}
void draw_square(unsigned int x, unsigned int y, int l, int color){
	draw_rectangle(x, y, l, l, color);
}

void draw_char(char caracter){
	uint64_t fd = getFdOut();
	if(fd > 0){
		//scroll(FONT_SIZE);
		//writePipe(fd, &caracter);
		/*if (cursor_x + CHAR_WIDTH*FONT_SIZE > marginRight)
		{
			cursor_x = marginLeft;
			cursor_y += CHAR_HEIGHT*FONT_SIZE;
			if (cursor_y >= SCREEN_HEIGHT){
				scroll(FONT_SIZE);
			}
		}
		draw_char_personalized(cursor_x, cursor_y, caracter, FONT_SIZE, FONT_COLOR, BACKGROUND_COLOR);
		cursor_x += CHAR_WIDTH*FONT_SIZE;*/
		writePipe(fd, &caracter);	
	} else {
		if(caracter == '\n')
			newLine();
		if (cursor_x + CHAR_WIDTH*FONT_SIZE > marginRight)
		{
			cursor_x = marginLeft;
			cursor_y += CHAR_HEIGHT*FONT_SIZE;
			if (cursor_y >= SCREEN_HEIGHT){
				scroll(FONT_SIZE);
			}
		}
		draw_char_personalized(cursor_x, cursor_y, caracter, FONT_SIZE, FONT_COLOR, BACKGROUND_COLOR);
		cursor_x += CHAR_WIDTH*FONT_SIZE;
	}
}

void draw_char_personalized(int x, int y, char character, int fontsize, int fontcolor, int backgroundcolor){
	if (x + CHAR_WIDTH*fontsize > marginRight)
	{
		x = marginLeft;
		y += CHAR_HEIGHT*fontsize;
		if (y >= SCREEN_HEIGHT){
			//scroll(CHAR_HEIGHT*fontsize);
			return;
		}
	}

	int aux_x = x;
	int aux_y = y;

	char bitIsPresent;

	unsigned char * toDraw = charBitmap(character);

	for (int i = 0; i < CHAR_HEIGHT; i++)
	{
		for (int j = 0; j < CHAR_WIDTH; j++)
		{
			bitIsPresent = (1<<(CHAR_WIDTH-j-1)) & toDraw[i];

			if (bitIsPresent)
			{
				draw_square(aux_x, aux_y, fontsize, fontcolor);
			}
			else
			{
				draw_square(aux_x, aux_y, fontsize, backgroundcolor);
			}
			aux_x+=fontsize;
		}
		aux_x = x;
		aux_y += fontsize;
	}
}

void draw_string_personalized(int x, int y, char * buffer, int count, int fontsize, int fontcolor, int backgroundcolor){
	if (x + CHAR_WIDTH*fontsize*count > marginRight)
	{
		x = marginLeft;
		y += CHAR_HEIGHT*fontsize;
		if (y >= SCREEN_HEIGHT){
			//scroll(fontsize);
			return;
		}
	}
	for (int i = 0; i < count; i++){
		draw_char_personalized(x, y, buffer[i], fontsize, fontcolor, backgroundcolor);
		x+=CHAR_WIDTH*fontsize;
	}
}

void draw_string(char * buffer, int count){
	for (int i = 0; i < count; i++){
		if(buffer[i]=='\n'){
			draw_char('\n');
			newLine();
		}
		else if(buffer[i]=='\t'){
			for(int i=0;i<4;i++){
				draw_char(' ');
			}
		}
		else
			draw_char(buffer[i]);
	}
}

void newLine(){
	show_cursor(1);
	cursor_x = marginLeft;
	cursor_y += CHAR_HEIGHT*FONT_SIZE;
	if (cursor_y >= SCREEN_HEIGHT){
		scroll(FONT_SIZE);
	}
}

void delete_char(){
	show_cursor(1);
	if (cursor_x == marginLeft)
	{
		cursor_x = marginRight;
		cursor_y -= CHAR_HEIGHT*FONT_SIZE;
	}
	cursor_x -= CHAR_WIDTH*FONT_SIZE;
	draw_char(' ');
	cursor_x -= CHAR_WIDTH*FONT_SIZE;
}

void delete_line(){
	while (cursor_x >= marginLeft)
	{
		delete_char();
	}
}

void scroll(int size){

  for(int j=0 ; j<SCREEN_HEIGHT-CHAR_HEIGHT*size ; j++){
   		for(int i=marginLeft ; i<marginRight ; i++){
            char * current = get_pixel( i, j);
            char * replace = get_pixel( i, j+CHAR_HEIGHT*size);
            current[0] = replace[0];
            current[1] = replace[1];
            current[2] = replace[2];
			replace[0]=0;
			replace[1]=0;
			replace[2]=0;
        }
    }
	cursor_y -= CHAR_HEIGHT*FONT_SIZE;
	cursor_x = marginLeft;
}

void clean(){
	for(int i=marginLeft ; i<=marginRight ; i++){
        for(int j=0 ; j<=SCREEN_HEIGHT ; j++){
            draw_pixel(i, j, BACKGROUND_COLOR);
        }
    }
	cursor_x = marginLeft;
	cursor_y = 0;
}
void clean_all(){
	for(int i=0 ; i<=SCREEN_WIDTH ; i++){
        for(int j=0 ; j<=SCREEN_HEIGHT ; j++){
            draw_pixel(i, j, BACKGROUND_COLOR);
        }
    }
	cursor_x = 0;
	cursor_y = 0;
}

void draw_number(uint64_t value, uint32_t base)
{
    int size = valueToBase(value, buffer, base);
    draw_string(buffer, size);
}

static uint32_t valueToBase(uint64_t value, char * buffer, uint32_t base)
{
	char *p = buffer;
	char *p1, *p2;
	uint32_t digits = 0;

	//Calculate characters for each digit
	do
	{
		uint32_t remainder = value % base;
		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
		digits++;
	}
	while (value /= base);

	// Terminate string in buffer.
	*p = 0;

	//Reverse string in buffer.
	p1 = buffer;
	p2 = p - 1;
	while (p1 < p2)
	{
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}

	return digits;
}
void draw_decimal(uint64_t value)
{
	draw_number(value, 10);
}

static void _64Hexfill(int n, char * buffer) {
	for (int i = 15; i >= 0; i--) {
		if (i >= n)
			buffer[i] = buffer[i - n];
		else
			buffer[i] = '0';
	}
	buffer[16] = 0;
}

void draw_hex(uint64_t value)
{
	int digits = valueToBase(value, buffer, 16);
	_64Hexfill(16 - digits, buffer);
	draw_string(buffer, Strlen(buffer));
}



void set_cursor(int x, int y){
	aux_x = cursor_x;
	aux_y = cursor_y;
	cursor_x = x;
	cursor_y = y;
}

void set_margins(int marginL,int marginR){
	marginLeft=marginL;
	marginRight=marginR;
}

void return_last_cursor(){
	cursor_x = aux_x;
	cursor_y = aux_y;
}

void getCoords(int* coords){
	*coords=cursor_x;
	coords++;
	*coords=cursor_y;
	coords--;
}
