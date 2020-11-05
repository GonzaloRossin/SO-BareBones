#include <libasm.h>
#include <Lib.h>
#include <shell.h>
#define BUFFER_SIZE 100
#define MAX_COMDESC 100
#define MAX_COMMANDS 20

//Buffer to store the input from the keyboard.
static char terminalBuffer[BUFFER_SIZE + 1] = {0}; //Non cyclic buffer
static int bufferSize = 0;

int main() {
    fillCommandList();
    print("WELCOME TO chessOS, espero que le guste mucho el ajedrez");
    newLine();
    newLine();
    print("ingrese el comando help para comenzar");
    newLine();
    shell();
    return 0;
}
