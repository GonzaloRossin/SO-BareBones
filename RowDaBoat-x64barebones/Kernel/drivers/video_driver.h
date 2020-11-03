#ifndef VIDEO_DRIVER_H
#define VIDEO_DRIVER_H

#define FONT_COLOR 0xFFFFFF
#define FONT_SIZE 1
#define BACKGROUND_COLOR 0x000000
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

char * get_pixel(unsigned int x, unsigned int y);
void draw_pixel(unsigned int x, unsigned int y,int color);
void draw_rectangle(unsigned int x, unsigned int y, int b, int h, int color);
void draw_square(unsigned int x, unsigned int y, int l, int color);
void draw_char(char character);
void draw_string(char* buffer,int count);
void newLine();
void delete_char();
void delete_line();
void scroll();

#endif