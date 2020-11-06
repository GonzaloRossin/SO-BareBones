#ifndef CHESSLIB_H
#define CHESSLIB_H

#include <stdint.h>

typedef struct chess_square
{
    int x;
    int y;
    int row;
    char column;
    int color;
    int backgroundcolor;
    int piece;
}chess_square;

void draw_square(chess_square square);
void initialize_chess();
void draw_board();
void draw_tags();
void turn_board_180();
void turn_board_90();
void turn_board_normal();
int abs(int i);
chess_square * get_board_tile(int row, char column);
int obstacles(chess_square origin, chess_square destiny);
int validate_move(chess_square origin, chess_square destiny);
void swap(chess_square origin, chess_square destiny);
void minishell();

#endif