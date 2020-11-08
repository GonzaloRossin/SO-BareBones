#include <libasm.h>
#include <Lib.h>
#include <shell.h>
#include <chesslib.h>
#include <chess_draw.h>
#define BUFFER_SIZE 100
#define MAX_COMDESC 100
#define MAX_COMMANDS 20

/*
//Buffer to store the input from the keyboard.
static char terminalBuffer[BUFFER_SIZE + 1] = {0}; //Non cyclic buffer
static int bufferSize = 0;
*/


int main() {
    shell();
    return 0;
}
