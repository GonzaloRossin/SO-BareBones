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
    while(1){
        if(readNewInput())
            inforeg();
    }
    return 0;
}



