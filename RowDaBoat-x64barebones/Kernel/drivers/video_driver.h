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
void draw_char_personalized(int x, int y, char character, int fontsize, int fontcolor, int backgroundcolor);
void draw_string(char* buffer,int count);
void draw_string_personalized(int x, int y, char * buffer, int count, int fontsize, int fontcolor, int backgroundcolor);
void newLine();
void delete_char();
void delete_line();
void scroll(int size);
void clean();
void clean_all();
void draw_number(uint64_t value, uint32_t base);
void draw_decimal(uint64_t value);
void draw_hex(uint64_t value);
void draw_matrix(int x, int y, unsigned char * matrix, int width, int height, int draw_size, int color, int backgroundcolor);
void set_cursor(int x, int y);
void set_margins(int marginL,int marginR);
void return_last_cursor();
void getCoords(int* coords);


#endif