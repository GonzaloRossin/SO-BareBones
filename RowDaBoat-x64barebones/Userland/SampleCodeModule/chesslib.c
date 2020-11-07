#include <chesslib.h>
#include <chess_draw.h>
#include <chess_shell.h>
#include <stdint.h>
#include <libasm.h>
#include <Lib.h>
#include <shell.h>
#include <math.h>

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

#define initial_x 460
#define initial_y 150

#define INDEX_SIZE 1.5
#define TIME_X 50
#define TIME_Y 500

chess_square board[8][8]={{0}};
matrix_struct rows[8]={[0 ... 7] = {0,0,0,0,0,0,0,0,INDEX_SIZE,WHITE,BLACK}};
matrix_struct columns[8]={[0 ... 7] = {0,0,0,0,0,0,0,0,INDEX_SIZE,WHITE,BLACK}};

matrix_struct matrix = {0,0,0,0,0,0,CHESS_SQUARE_WIDTH, CHESS_SQUARE_HEIGHT, CHESS_DRAW_SIZE, 0, 0};
matrix_struct * m = &matrix;
matrix_struct * p;

chess_square * casilla_al_paso;

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
    int x = initial_x;
    int y = initial_y;
    clearScreen();
    // Set the pieces on the board
    for (int i = 0; i < 8; i++)
    {
        // Draw column tags
        if (i == 0)
        {
            x += CHESS_SQUARE_WIDTH*CHESS_DRAW_SIZE-8*INDEX_SIZE/2;
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
            y += 16*(INDEX_SIZE);         
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
        p->x = x+8*INDEX_SIZE;
        p->y = y+CHESS_SQUARE_WIDTH*CHESS_DRAW_SIZE-8*INDEX_SIZE;
        p->caracter = 56-i;
        draw(1,p);
        // Jump line
        x = initial_x;
        y += CHESS_SQUARE_HEIGHT*CHESS_DRAW_SIZE;
    }
    draw_board();
    fillChessList();
    mini_shell();
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

void turn_board_180(){
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
    char aux[8];
    for (int i = 0; i < 8; i++)
    {   
        aux[i] = columns[i].caracter;
        columns[i].caracter = rows[7-i].caracter;
    }
    for (int i = 0; i < 8; i++)
    {   
        rows[i].caracter = aux[i];
    }      
    for (int i = 0; i < 4; i++)
    {
        for (int j = i; j < 7-i; j++)
        {
            chess_square aux2 = board[i][j];   
            board[i][j].x = board[j][7-i].x;
            board[i][j].y = board[j][7-i].y;
            board[j][7-i].x = board[7-i][7-j].x;
            board[j][7-i].y = board[7-i][7-j].y;
            board[7-i][7-j].x = board[7-j][i].x;
            board[7-i][7-j].y = board[7-j][i].y;
            board[7-j][i].x = aux2.x;
            board[7-j][i].y = aux2.y;
        }
    }
    draw_tags();
    draw_board();
}

void turn_board_normal(){  
    char aux[8];
    for (int i = 0; i < 8; i++)
    {   
        aux[i] = columns[i].caracter;
        columns[i].caracter = rows[i].caracter;
    }
    for (int i = 0; i < 8; i++)
    {   
        rows[i].caracter = aux[7-i];
    } 
      
    for (int i = 0; i < 4; i++)
    {
        for (int j = i; j < 7-i; j++)
        {
            chess_square aux2 = board[i][j];   
            board[i][j].x = board[7-j][i].x;
            board[i][j].y = board[7-j][i].y;
            board[7-j][i].x = board[7-i][7-j].x;
            board[7-j][i].y = board[7-i][7-j].y;
            board[7-i][7-j].x = board[j][7-i].x;
            board[7-i][7-j].y = board[j][7-i].y;
            board[j][7-i].x = aux2.x;
            board[j][7-i].y = aux2.y;
        }
    }
    draw_tags();
    draw_board();
}

chess_square * get_board_tile(int row, char column){
    if (column < 'a')
    {
        return &board[8-row][column-'A'];
    }
    return &board[8-row][column-'a'];
}

// returns 0 if its ok, -1 if there's an obstacle, -2 if movement is not valid.
int obstacles(chess_square * origin, chess_square * destiny){
    if (origin->column == destiny->column)
    {
        int dist = destiny->row - origin->row;
        int row = origin->row;
        for (int i = 0; i < abs(dist)-1; i++)
        {  
            if (dist>0)
            {
                row++;
            }else
            {
                row--;
            }
            chess_square * aux = get_board_tile(row, origin->column);
            if (aux->piece != 0)
            {
                return -1;
            }
        }
        return 0;
    }
    else if (origin->row == destiny->row)
    {
        int dist = destiny->column - origin->column;
        int column = origin->column;
        for (int i = 0; i < abs(dist)-1; i++)
        {  
            if (dist>0)
            {
                column++;
            }else
            {
                column--;
            }
            chess_square * aux = get_board_tile(origin->row, column);
            if (aux->piece != 0)
            {
                return -1;
            }
        }
        return 0;
    }
    else
    {
        int dist_row = destiny->row - origin->row;
        int dist_column = destiny->column - origin->column;
        if (abs(dist_row)!=abs(dist_column))
        {
            // Not a valid movement to search in obstacles()
            return -2;
        }
        
        int row = origin->row;
        int column = origin->column;

        for (int i = 0; i < abs(dist_column)-1; i++)
        {  
            if (dist_column > 0)
            {
                column++;
            }else
            {
                column--;
            }
            if (dist_row > 0)
            {
                row++;
            }else
            {
                row--;
            }
            chess_square * aux = get_board_tile(row, column);
            if (aux->piece != 0)
            {
                return -1;
            }
        }
        return 0;
    }
}

// Retorna: 0 valid movement, 1 castling valid movement, 2 al paso movement, -1 invalid, -2 check warning
int validate_move(chess_square * origin, chess_square * destiny){
    // Inavlida movimiento a la misma casilla o si no se esta moviendo una pieza.
    if (origin->piece == NO_PIECE || (origin->row == destiny->row && origin->column == destiny->column))
    {
        return -1;
    }
    // Invalida movimiento sobre una pieza del mismo color, a excepcion del enroque.
    if (origin->color == destiny->color && destiny->piece!=NO_PIECE)
    {
        // Verifica enroque.
        if (origin->piece == KING && destiny->piece == ROOK && origin->flag_castling == 0 && destiny->flag_castling == 0 && obstacles(origin, destiny) == 0)
        {
            return 1;
        }
        return -1;    
    }
    
    int dist_row = destiny->row - origin->row;
    int dist_column = destiny->column - origin->column;

    switch (origin->piece)
    {
    case NO_PIECE:
        break;

    case PAWN:
        // Movimiento de avance de peon
        if ( dist_column == 0 && destiny->piece == NO_PIECE )
        {
            if (origin->color == WHITE)
            {
                if (dist_row == 1 )
                {
                    return 0;
                }
                if (dist_row == 2 && origin->row == 2)
                {
                    return obstacles(origin, destiny);
                }
            }
            else
            {
                if (dist_row == -1)
                {
                    return 0;
                }
                if (dist_row == -2 && origin->row == 7)
                {
                    return obstacles(origin, destiny);
                }
            }     
        }
        // Movimiento para comer piezas del peon + chequeo de peon al paso.
        if (origin->color == WHITE && abs(dist_column) == 1 && dist_row == 1)
        {
            chess_square * aux = get_board_tile(destiny->row-1, destiny->column);
            if (aux->piece==PAWN && aux->color == BLACK && aux->flag_alpaso == 1)
            {
                return 2;
            }
            if (destiny->piece != 0)
            {
                return 0;
            }
        }
        if (origin->color == BLACK && abs(dist_column) == 1 && dist_row == -1)
        {
            chess_square * aux = get_board_tile(destiny->row+1, destiny->column);
            if (aux->piece==PAWN && aux->color == WHITE && aux->flag_alpaso == 1)
            {
                return 2;
            }
            if (destiny->piece != 0)
            {
                return 0;
            }
        }
        break;

    case KNIGHT:
        if (dist_row!=0 && dist_column!=0 && abs(dist_row)+abs(dist_column)==3)
        {
            return 0;
        }
        break;

    case BISHOP:
        if (abs(dist_row) == abs(dist_column))
        {
            return obstacles(origin, destiny);
        }
        break;

    case ROOK:
        if (dist_column==0 || dist_row==0)
        {
            return obstacles(origin,destiny);
        }
        break;

    case QUEEN:
        return obstacles(origin, destiny);
        break;

    case KING:
        if (abs(dist_column)+abs(dist_row)==1 || (dist_column==1&&dist_row==1))
        {
            return validate_check(destiny, origin->color);
        }
        break;
    }
    return -1;
}

int validate_player(chess_square * origin, int player_color){
    return origin->color == player_color;
}

int move(chess_square * origin, chess_square * destiny){
    casilla_al_paso->flag_alpaso = 0;
    if (destiny->piece == KING)
    {
        return 1;
    }
    if (origin->piece == PAWN && abs(destiny->row-origin->row)==2)
    {
        casilla_al_paso = destiny;
        casilla_al_paso->flag_alpaso = 1;
    }
    
    destiny->piece = origin->piece;
    destiny->color = origin->color;
    origin->piece = NO_PIECE;
    origin->color = 0;
    origin->flag_castling++;
    draw_board();
    return 0;
}

// Retorna -1 si hay jaque en ese lugar, 0 si no lo hay.
int validate_check(chess_square * place, int color){
    int enemy_color;
    if (color==WHITE)
    {
        enemy_color = BLACK;
    }
    else
    {
        enemy_color = WHITE;
    }
    
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            chess_square * aux = &board[i][j];
            if (aux->piece != NO_PIECE && aux->color == enemy_color)
            {
                if (validate_move(aux, place)==0)
                {
                    return -2;
                }
            }
        }
    }
    return 0;
}

void castling_move(chess_square * origin,  chess_square * destiny){
    chess_square * aux;
    if (origin->column < destiny->column)
    {
        aux = get_board_tile(origin->row, origin->column+2);
        move(origin, aux);
        
        aux = get_board_tile(destiny->row, destiny->column-2);
        move(destiny, aux);
    }
    else
    {
        aux = get_board_tile(origin->row, origin->column-2);
        move(origin, aux);
        aux = get_board_tile(destiny->row, destiny->column+3);
        move(destiny, aux);
    }
    origin->flag_castling++;
    destiny->flag_castling++;
}

void al_paso_move(chess_square * origin, chess_square * destiny){
    move(origin, destiny);
    casilla_al_paso->piece = NO_PIECE;
    casilla_al_paso->color = 0;
}
