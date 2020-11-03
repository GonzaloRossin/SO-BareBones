#include <libasm.h>
#include <shell.h>
#include <Lib.h>

#define BUFFER_SIZE 100
#define MAX_COMDESC 100
#define MAX_COMMANDS 20

//Structure of a command stored in the command List
typedef struct command
{
    char command_name[MAX_COMDESC];
    char desc[MAX_COMDESC];
    void (*cmdptr)(void);
} command;

//Store all the commands in this array.
static command commandList[MAX_COMMANDS];
static int commandsSize = 0;

//Buffer to store the input from the keyboard.
static char terminalBuffer[BUFFER_SIZE + 1] = {0}; //Non cyclic buffer
static int bufferSize = 0;


void CPUinfo(){
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
    newLine();
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
    else if(chartoadd==ENTER){
        putActioncall(0);
        return 1;
    }
    else if(chartoadd==BACKSPACE){
        if (bufferSize > 0)
        {
            terminalBuffer[--bufferSize] = 0;
            putActioncall(1);
        }
        return 0;
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

void shell()
{
    print("WELCOME TO mierdaOS, espero que le guste mucho el ajedrez");
    newLine();
    newLine();
    print("ingrese el comando help para comenzar");
    newLine();
    while(1){
        if(readNewInput()){
            CommandHandler();
            cleanBuffer();
        }
    }
}
void help(){
    print("Los comandos a disposicion del usuario son los siguientes:");
    newLine();
    for(int i=0;i<commandsSize;i++){
        print(commandList[i].command_name);
        print(commandList[i].desc);
        newLine();
    }
}

void fillCommandList()
{
    fillCommand("help",": despliega al usuario los comandos disponibles",&help);
    fillCommand("inforeg",": Imprime informacion del cpu", &inforeg);
    fillCommand("get CPUinfo",": Imprime informacion del cpu", &CPUinfo);
    fillCommand("testDivisionBy0",": ejemplo de excepcion de dividir por 0" ,&testDivisionBy0Command);

}
void CommandHandler()
{

    //Copy the command into the array. Did this to avoid a bug in which in some cases the buffer
    //represented more chars that it should.
    char potentialCommand[MAX_COMDESC] = {0};
    strncpy(terminalBuffer, potentialCommand, bufferSize);

    for (int i = 0; i < commandsSize; i++)
    {
        if (strcmp(potentialCommand, commandList[i].command_name))
        {
            (commandList[i].cmdptr)();

            //After executing the command we print a newLine and exit.
            newLine();
            return;
        }
    }

    //If command not found
    print("Not a valid command: ");
    print(potentialCommand);
    newLine();
}

void fillCommand(char* name,char *desc, void (*cmdptr)(void))
{
    command aux;
    strncpy(name,aux.command_name,strlen(name));
    strncpy(desc, aux.desc, strlen(desc));
    aux.cmdptr = cmdptr;
    commandList[commandsSize++] = aux;
}