#include <libasm.h>
#include <Lib.h>
#include <shell.h>
#include <chesslib.h>
#include <chess_draw.h>
#define BUFFER_SIZE 100
#define MAX_COMDESC 100
#define MAX_COMMANDS 20

//Buffer to store the input from the keyboard.
static char terminalBuffer[BUFFER_SIZE + 1] = {0}; //Non cyclic buffer
static int bufferSize = 0;


int main() {
    /*
    fillCommandList();
    print("WELCOME TO chessOS, espero que le guste mucho el ajedrez");
    newLine();
    newLine();
    print("ingrese el comando help para comenzar");
    newLine();
    shell();
    matrix_struct * m;
            m->x = TIME_X;
            m->y = TIME_Y;
            m->draw_size = 1;
            m->color = 0xFFFFFF;
            m->backgroundcolor = 0x000000;
            for (int i = 0; i < 4; i++)
            {
                m->caracter = '0' + tiempo[i];
                draw(1, m);
                m->x+=8;
            }
    */
    initialize_chess();
    urn_board_90();
    return 0;
}
