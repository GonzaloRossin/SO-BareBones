#ifndef VIDEO_DRIVER_H
#define VIDEO_DRIVER_H

void draw_pixel(int x,int y,int color);
void draw_rectangle(unsigned int x, unsigned int y, int b, int h, int color);
void draw_square(unsigned int x, unsigned int y, int l, int color);
void draw_char(int x, int y, char character, int fontSize, int fontColor, int backgroundColor);


#endif