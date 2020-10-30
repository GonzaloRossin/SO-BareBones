#ifndef VIDEO_DRIVER_H
#define VIDEO_DRIVER_H

#define FONT_COLOR 0xFFFFFF
#define FONT_SIZE 1
#define BACKGROUND_COLOR 0x000000
#define LINE_SPACE 5
#define SCREEN_WIDTH 1010

void draw_pixel(int x,int y,int color);
void draw_rectangle(unsigned int x, unsigned int y, int b, int h, int color);
void draw_square(unsigned int x, unsigned int y, int l, int color);
void draw_char(unsigned int x, unsigned int y, char character, int fontSize, int fontColor, int backgroundColor);

#endif