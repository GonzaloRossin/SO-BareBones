#include <chesslib.h>
#include <chess_draw.h>
#include <stdint.h>
#include <libasm.h>
#include <Lib.h>

#define BACKGROUND_COLOR1 0x9AE1CD
#define BACKGROUND_COLOR2 0x3F7E7A
#define CONTOUR_COLOR 0x000000
#define WHITE 0xFFFFFF
#define BLACK 0x000000
#define NO_PIECE 0
#define PAWN 1
#define KNIGHT 2
#define BISHOP 3
#define ROOK 4
#define QUEEN 5
#define KING 6

typedef struct chess_square
{
    int color;
    int backgroundcolor;
    int piece;
}chess_square;

chess_square board[8][8] = {'0'};
matrix_struct * m;

void draw_square(int color, int backgroundcolor, int piece){
    m->matrix = chessBitmap(piece, 0);
    m->width = CHESS_SQUARE_WIDTH;
    m->height = CHESS_SQUARE_HEIGHT;
    m->draw_size = CHESS_DRAW_SIZE;
    m->color = color;
    m->backgroundcolor = backgroundcolor;
    m->contourcolor = CONTOUR_COLOR;
    draw(0, m);
    m->matrix = chessBitmap(piece, 1);
    draw(0,m);
}

void initialize_chess(){
    // Set the pieces on the board
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            chess_square * aux = &board[i][j];
            if (i==1 || i==6)
            {
                aux->piece = PAWN;
            }
            else if (i==7 || i==0)
            {
                if (j==7 || j==0)
                {
                    aux->piece = ROOK;
                }
                else if (j==1 || j==6)
                {
                    aux->piece = KNIGHT;
                }
                else if (j==2 || j==5)
                {
                    aux->piece = BISHOP;
                }
                else if (j==3)
                {
                    aux->piece = QUEEN;
                }
                else if (j==4)
                {
                    aux->piece = KING;
                }
            }
            else
            {
                aux->piece = NO_PIECE;
            }
            
            // Set piece color
            if (i==0 || i ==1)
            {
                aux->color = BLACK;
            }else
            {
                aux->color = WHITE;
            }
            // Set background color
            if ((i+j) % 2 == 0)
            {
                aux->backgroundcolor = BACKGROUND_COLOR1;
            }else
            {
                aux->backgroundcolor = BACKGROUND_COLOR2;
            }
            
            
        }
    }
    draw_board();
}

void draw_board(){
    // draw_letters PONER LAS LETRAS DE LAS COLUMNAS ACA O AL FINAL
    for (int i = 0; i < 8; i++)
    {
        // draw_number PONER EL NUMERO DE FILA
        for (int j = 0; j < 8; j++)
        {
            chess_square * square = &board[i][j];
            draw_square(square->color, square->backgroundcolor, square->piece);
        }
        //draw_jump SALTAR A FILA DE ABAJO
    }
}