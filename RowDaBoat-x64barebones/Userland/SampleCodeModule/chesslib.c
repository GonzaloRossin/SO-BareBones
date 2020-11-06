#include <chesslib.h>
#include <chess_draw.h>
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

#define initial_x 300
#define initial_y 30

#define INDEX_SIZE 2

chess_square board[8][8];
matrix_struct rows[8]={[0 ... 7] = {0,0,0,0,0,0,0,0,INDEX_SIZE,WHITE,BLACK}};
matrix_struct columns[8]={[0 ... 7] = {0,0,0,0,0,0,0,0,INDEX_SIZE,WHITE,BLACK}};

matrix_struct matrix = {0,0,0,0,0,0,CHESS_SQUARE_WIDTH, CHESS_SQUARE_HEIGHT, CHESS_DRAW_SIZE, 0, 0};
matrix_struct * m = &matrix;
matrix_struct * p;

//Buffer to store the input from the keyboard.
#define BUFFER 50
#define MAX_COMMANDS 10
#define MAX_DESC 50

typedef struct chesscommand
{
    char command_name[MAX_DESC];
    char desc[MAX_DESC];
    void (*cmdptr)(void);
} chesscommand;

static chesscommand commandList[MAX_COMMANDS];
static int commandsSize = 0;

static char buffer[BUFFER+ 1] = {0}; //Non cyclic buffer
static int buffer_size = 0;

void initialize_chess(){
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
    for (int i = 0; i < 8; i++)
    {
        matrix_struct aux = rows[i];
        rows[i].caracter = columns[8-i].caracter;
        columns[8-i].caracter = aux.caracter;
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
    return &board[8-row][column-'a'];
}

int abs(int i){
    return i >= 0 ? i : -i;
}
// returns 0 if its ok, -1 if there's an obstacle adn -2 if movement is not valid.
int obstacles(chess_square origin, chess_square destiny){
    if (origin.column == destiny.column)
    {
        int dist = destiny.row - origin.row;
        int row = origin.row;
        for (int i = 0; i < abs(dist)-1; i++)
        {  
            if (dist>0)
            {
                row++;
            }else
            {
                row--;
            }
            chess_square * aux = get_board_tile(row, origin.column);
            if (aux->piece != 0)
            {
                return -1;
            }
        }
        return 0;
    }
    else if (origin.row == destiny.row)
    {
        int dist = destiny.column - origin.column;
        int column = origin.column;
        for (int i = 0; i < abs(dist)-1; i++)
        {  
            if (dist>0)
            {
                column++;
            }else
            {
                column--;
            }
            chess_square * aux = get_board_tile(origin.row, column);
            if (aux->piece != 0)
            {
                return -1;
            }
        }
        return 0;
    }
    else
    {
        int dist_row = destiny.row - origin.row;
        int dist_column = destiny.column - origin.column;
        if (abs(dist_row)!=abs(dist_column))
        {
            // Not a valid movement to search obstacles()
            return -2;
        }
        
        int row = origin.row;
        int column = origin.column;

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

// return 0 if valid movement
int validate_move(chess_square origin, chess_square destiny){
    if (origin.row == destiny.row || origin.column == destiny.column)
    {
        return -1;
    }
    int dist_row = destiny.row - origin.row;
    int dist_column = destiny.column - origin.column;
    switch (origin.piece)
    {
    case NO_PIECE:
        break;

    case PAWN:
        if ( dist_column == 0 )
        {
            if (dist_row == 1)
            {
                return 0;
            }
            if (dist_row == 2)
            {
                return 0;
            }
            
        }
        break;

    case KNIGHT:
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
        break;
    }
    return -1;
}

static int readChessInput()
{
    
    char chartoadd=read_input();

    //If there is nothing new or its not a valid character...
    if (chartoadd == 0)
    {
        return 0;
    }
    else if(chartoadd==ENTER){
        putActioncall(3);
        return 1;
    }
    else if(chartoadd==BACKSPACE){
        if (buffer_size > 0)
        {
            buffer[--buffer_size] = 0;
            putActioncall(1);
        }
        return 0;
    }
    //If its a regular letter.
    else
    {
        if (buffer_size <= 100)
        {
            buffer[buffer_size++] = chartoadd;
            putChar(chartoadd);
            return 0;
        }
    }

    //Just in case
    return 0;
}
static void cleanChessBuffer(){
    for (int i = 0; i < BUFFER; i++)
    {
       buffer[i] = 0;
    }
    buffer_size = 0;
}
// void CommandHandlerChess(){
//     char potentialCommand[BUFFER] = {0};
//     strncpy(buffer, potentialCommand,0, buffer_size);
//     for (int i = 0; i < command_size && potentialCommand[i]!= ' '; i++)
//     {
//         if (strcmp(potentialCommand, commandList[i].command_name))
//         {
//             (commandList[i].cmdptr)();
//             newLine();
//             return;
//         }
//     }
// }
void minishell(){
    put_char('>');
    while(1){
        if(readChessInput()){
            //CommandHandlerChess();
            put_char('>');
            cleanChessBuffer();
        }
    }
}