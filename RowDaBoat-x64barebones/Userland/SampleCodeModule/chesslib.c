#include <chesslib.h>
#include <chess_draw.h>
#include <chess_shell.h>
#include <stdint.h>
#include <libasm.h>
#include <Lib.h>
#include <shell.h>

#define BACKGROUND_COLOR1 0x9AE1CD
#define BACKGROUND_COLOR2 0x3F7E7A
#define WHITE 0xFFFFFF
#define BLACK 0x000000
#define NO_PIECE 0
#define PAWN 1
#define KNIGHT 2
#define BISHOP 3
#define ROOK 4
#define QUEEN 5
#define KING 6

#define inicial_x 300
#define inicial_y 30

chess_square board[8][8];
matrix_struct matrix = {0,0,0,0,CHESS_SQUARE_WIDTH, CHESS_SQUARE_HEIGHT, CHESS_DRAW_SIZE, 0, 0};
matrix_struct pos = {inicial_x,inicial_y,0,0,0,0,2,WHITE,BLACK};
matrix_struct * m = &matrix;
matrix_struct * p = &pos;



void draw_board(){
    // PONER ACA LETRAS DE COLUMNAS
    for (int i = 0; i < 8; i++)
    {
        // draw_number PONER EL NUMERO DE FILA
        for (int j = 0; j < 8; j++)
        {
            draw_square(board[i][j]);
        }
    }
}

void draw_square(chess_square square){
    m->x = square.x;
    m->y = square.y;
    m->matrix = chessBitmap(square.piece, 0);
    m->color = square.color;
    m->backgroundcolor = square.backgroundcolor;
    draw(0, m);
    m->x += CHESS_SQUARE_WIDTH*CHESS_DRAW_SIZE;
    m->matrix = chessBitmap(square.piece, 1);
    draw(0,m);
}

void turn_board(){
    for (int i = 0; i < 4; i++)
    {
        // draw_number PONER EL NUMERO DE FILA
        for (int j = 0; j < 8; j++)
        {
            chess_square aux = board[i][j];
            board[i][j].x = board[7-i][7-j].x;
            board[i][j].y = board[7-i][7-j].y;
            board[7-i][7-j].x = aux.x;
            board[7-i][7-j].y = aux.y;
        }
    }
    draw_board();
}

void initialize_chess(){
    clearScreen();
    int x = inicial_x;
    int y = inicial_y;
    fillCommandsChess();
    // Set the pieces on the board
    for (int i = 0; i < 8; i++)
    {
        // Draw column tags
        if (i == 0)
        {
            x += CHESS_SQUARE_WIDTH*CHESS_DRAW_SIZE-8;
            for (char i = 65; i < 73; i++)
            {
                p->x = x;
                p->caracter = i;
                x += CHESS_SQUARE_WIDTH*CHESS_DRAW_SIZE*2;
                draw(1, p);
            }
            x = inicial_x;
            y += 16*3;         
        }
        
        for (int j = 0; j < 8; j++)
        {
            chess_square * aux = &board[i][j];
            aux->x = x;
            aux->y = y;
            aux->row = j+1;
            aux->column = 'a'+i;
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
            x += CHESS_SQUARE_WIDTH*CHESS_DRAW_SIZE*2;
        }
        // Draw row tag
        p->x = x+16*2;
        p->y = y+CHESS_SQUARE_WIDTH*CHESS_DRAW_SIZE-16;
        p->caracter = 49+i;
        draw(1,p);
        // Jump line
        x = inicial_x;
        y += CHESS_SQUARE_HEIGHT*CHESS_DRAW_SIZE;
    }
    draw_board();
    mini_shell();
}