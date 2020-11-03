#include <libasm.h>
#include <Lib.h>
#include <shell.h>
#define BUFFER_SIZE 100
#define MAX_COMDESC 100
#define MAX_COMMANDS 20

//Buffer to store the input from the keyboard.
static char terminalBuffer[BUFFER_SIZE + 1] = {0}; //Non cyclic buffer
static int bufferSize = 0;

void get_CPUinfo();
void inforeg();
void testDivisionBy0Command();
void cleanBuffer();

int main() {
    while(1){
        if(readNewInput())
            inforeg();
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
void inforeg(){
    char regs[16][7] = {"rax: ", "rbx: ", "rcx: ", "rdx: ", "rbp: ", "rdi: ", "rsi: ", "r8: ", "r9: ", "r10: ", "r11: ", "r12: ", "r13: ", "r14: ", "r15: ", "rsp: "};
    uint64_t v[16] = {0};
    get_InfoReg(v);
    char text[70];
    for(int i=0;i<16;i++){
        numToChar(v[i], text);
        print(regs[i]);
        print(text);
        newLine();
    }
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
int readNewInput()
{
    
    char chartoadd=getInput();

    //If there is nothing new or its not a valid character...
    if (chartoadd == 0)
    {
        return 0;
    }
    else if(chartoadd=='c'){
        return 1;
    }
    //If its a regular letter.
    else
    {
        if (bufferSize <= 100)
        {
            terminalBuffer[bufferSize++] = chartoadd;
            putChar(chartoadd);
            return 0;
        }
    }

    //Just in case
    return 0;
}