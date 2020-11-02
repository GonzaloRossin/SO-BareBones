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
    get_CPUinfo();
    testDivisionBy0Command();
    while(1){
    }
    return 0;
}
void get_CPUinfo(){
   char CPU[70];
   char MODEL[70];
   char FAMILY[70];
   uint32_t num;
   get_CPUvendor(CPU, &num);
   int model = (num & 0xFF0) >> 4;
   int family = model >> 4;
   model = model & 0x0f;
    numToChar(model, MODEL);
    numToChar(family, FAMILY);
    print("CPU:");
    print(CPU);
    newLine();
    print("Model:");
    print(MODEL);
    newLine();
    print("Family:");
    print(FAMILY);
    newLine();
}
void cleanBuffer(){
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        terminalBuffer[i] = 0;
    }
    bufferSize = 0;
}
void testDivisionBy0Command()
{
    int a = 4 / 0;
}