#include <stdint.h>
#include <font.h>
#include <video_driver.h>

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

struct vbe_mode_info_structure * screendata=0x0000000000005C00;

static int cursor_x = 0;
static int cursor_y = 0;

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

void draw_square(unsigned int x, unsigned int y, int l, int color){
	draw_rectangle(x, y, l, l, color);
}

void draw_char(char character){
	if (cursor_x + CHAR_WIDTH*FONT_SIZE > SCREEN_WIDTH)
	{
		cursor_x = 0;
		cursor_y += CHAR_HEIGHT*FONT_SIZE;
		if (cursor_y >= SCREEN_HEIGHT){
		scroll();
		}
	}

	int aux_x = cursor_x;
	int aux_y = cursor_y;

	char bitIsPresent;

	unsigned char * toDraw = charBitmap(character);

	for (int i = 0; i < CHAR_HEIGHT; i++)
	{
		for (int j = 0; j < CHAR_WIDTH; j++)
		{
			bitIsPresent = (1<<(CHAR_WIDTH-j)) & toDraw[i];

			if (bitIsPresent)
			{
				draw_square(aux_x, aux_y, FONT_SIZE, FONT_COLOR);
			}
			else
			{
				draw_square(aux_x, aux_y, FONT_SIZE, BACKGROUND_COLOR);
			}
			aux_x+=FONT_SIZE;
		}
		aux_x = cursor_x;
		aux_y += FONT_SIZE;
	}
	cursor_x += CHAR_WIDTH*FONT_SIZE;
}

void draw_string(char * buffer,int count){
	if (cursor_x + CHAR_WIDTH*FONT_SIZE*count > SCREEN_WIDTH)
	{
		cursor_x = 0;
		cursor_y += CHAR_HEIGHT*FONT_SIZE;
		if (cursor_y >= SCREEN_HEIGHT){
		scroll();
		}
	}
	for (int i = 0; i < count; i++){
		draw_char(buffer[i]);
	}
}

void newLine(){
	cursor_x = 0;
	cursor_y += CHAR_HEIGHT*FONT_SIZE;
	if (cursor_y >= SCREEN_HEIGHT){
		scroll();
	}
}

void delete_char(){
	if (cursor_x == 0)
	{
		cursor_x = SCREEN_WIDTH;
		cursor_y -= CHAR_HEIGHT*FONT_SIZE;
	}
	cursor_x -= CHAR_WIDTH*FONT_SIZE;
	draw_char(' ');
	cursor_x -= CHAR_WIDTH*FONT_SIZE;
}

void delete_line(){
	while (cursor_x > 0)
	{
		delete_char();
	}
}

void scroll(){
  
    for(int i=0 ; i<SCREEN_WIDTH ; i++){
        for(int j=0 ; j<SCREEN_HEIGHT ; j++){
            char * current = get_pixel( i, j);
            char * replace = get_pixel( i, j+CHAR_HEIGHT*FONT_SIZE);
            current[0] = replace[0];
            current[1] = replace[1];
            current[2] = replace[2];
        }
    }
	cursor_y -= CHAR_HEIGHT*FONT_SIZE;
	cursor_x = 0;
}

void clean(){
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
	char buffer[10] = { '0' };
	int i = 0;
    valueToBase(value, buffer, base);
    while (buffer[i]!=0)
	{
		draw_char(buffer[i]);
	}
}

void valueToBase(uint64_t value, char * buffer, uint32_t base)
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
}

void draw_decimal(uint64_t value)
{
	draw_number(value, 10);
}

void draw_hex(uint64_t value)
{
	draw_number(value, 16);
}