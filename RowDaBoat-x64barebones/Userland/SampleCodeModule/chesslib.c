#include <chesslib.h>
#include <chess_draw.h>
#include <stdint.h>
#include <libasm.h>
#include <Lib.h>

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

#define initial_x 300
#define initial_y 30

#define INDEX_SIZE 2

chess_square board[8][8];
matrix_struct rows[8]={[0 ... 7] = {0,0,0,0,0,0,INDEX_SIZE,WHITE,BLACK}};
matrix_struct columns[8]={[0 ... 7] = {0,0,0,0,0,0,INDEX_SIZE,WHITE,BLACK}};

matrix_struct matrix = {0,0,0,0,CHESS_SQUARE_WIDTH, CHESS_SQUARE_HEIGHT, CHESS_DRAW_SIZE, 0, 0};
//matrix_struct pos = {initial_x,initial_y,0,0,0,0,2,WHITE,BLACK};
matrix_struct * p;

void initialize_chess(){
    clearScreen();
    int x = initial_x;
    int y = initial_y;
    // Set the pieces on the board
    for (int i = 0; i < 8; i++)
    {
        // Draw column tags
        if (i == 0)
        {
            x += CHESS_SQUARE_WIDTH*CHESS_DRAW_SIZE-8;
            for (int z = 0; z < 8; z++)
            {
                p = &columns[z];
                p->x = x;
                p->y = initial_y;
                p->caracter = 65+z;
                x += CHESS_SQUARE_WIDTH*CHESS_DRAW_SIZE*2;
                draw(1, p);
            }
            x = initial_x;
            y += 16*3;         
        }
        
        for (int j = 0; j < 8; j++)
        {
            chess_square * aux = &board[i][j];
            aux->x = x;
            aux->y = y;
            aux->row = 8-i;
            aux->column = 'a'+j;
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
        p = &rows[i];
        p->x = x+16*2;
        p->y = y+CHESS_SQUARE_WIDTH*CHESS_DRAW_SIZE-16;
        p->caracter = 56-i;
        draw(1,p);
        // Jump line
        x = initial_x;
        y += CHESS_SQUARE_HEIGHT*CHESS_DRAW_SIZE;
    }
    draw_board();
}

void draw_board(){
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            draw_square(board[i][j]);
        }
    }
}

void draw_square(chess_square square){
    matrix_struct * m = &matrix;
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

void draw_tags(){
    for (int i = 0; i < 8; i++)
    {
        p = &rows[i];
        draw(1, p);
        p = &columns[i];
        draw(1, p);
    }
    
}

void turn_board(){
    for (int i = 0; i < 4; i++)
    {
        matrix_struct aux = rows[i];
        rows[i].caracter = rows[7-i].caracter;
        rows[7-i].caracter = aux.caracter;

        aux = columns[i];
        columns[i].caracter = columns[7-i].caracter;
        columns[7-i].caracter = aux.caracter;

        for (int j = 0; j < 8; j++)
        {
            chess_square aux = board[i][j];
            board[i][j].x = board[7-i][7-j].x;
            board[i][j].y = board[7-i][7-j].y;
            board[7-i][7-j].x = aux.x;
            board[7-i][7-j].y = aux.y;
        }
    }
    draw_tags();
    draw_board();
}

void turn_board_90(){    
    for (int i = 0; i < 8; i++)
    {
        matrix_struct aux = rows[i];
        rows[i].caracter = columns[i].caracter;
        columns[i].caracter = aux.caracter;

        for (int j = 0; j < i; j++)
        {
            chess_square aux = board[i][j];
            board[i][j].x = board[j][i].x;
            board[i][j].y = board[j][i].y;
            board[j][i].x = aux.x;
            board[j][i].y = aux.y;
        }
    }
    draw_tags();
    draw_board();
}

int validate(chess_square origin, chess_square destiny){
    switch (origin.piece)
    {
    case NO_PIECE:
        return -1;
        break;
    case PAWN:
        if ( destiny.column==origin.column )
        {
            int dist = destiny.row-origin.row;
            if (dist == 1)
            {
                return 0;
            }
            if (dist == 2 && origin.row == 2)
            {
                return 0;
            }
            
        }
        return -1;
        break;
    }
    return -1;
}

void swap(chess_square origin, chess_square destiny){
    chess_square aux = origin;

    origin.x = destiny.x;
    origin.y = destiny.y;
    destiny.x = aux.x;
    destiny.y = aux.y;
}