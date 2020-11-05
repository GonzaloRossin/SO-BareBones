#ifndef CHESSLIB_H
#define CHESSLIB_H

#include <stdint.h>

typedef struct chess_square
{
    int x;
    int y;
    int color;
    int backgroundcolor;
    int piece;
}chess_square;

void draw_square(chess_square square);
void initialize_chess();
void draw_board();
void turn_board();

#endif