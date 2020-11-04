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


static void CPUinfo(){
    char text[70];
    char text2[70];
    uint32_t num;
    get_CPUvendor(text, &num);
    int modelo = (num & 0xFF0) >> 4;
    int familia = modelo >> 4;
    modelo = modelo & 0x0f;
    print("CPU:");
    print(text);
    newLine();
    print("Modelo:");
    print_num(modelo,0);
    newLine();
    print("Familia:");
    print_num(familia,0);
    newLine();
}
static void inforeg(){
    char regs[16][7] = {"rax: ", "rbx: ", "rcx: ", "rdx: ", "rbp: ", "rdi: ", "rsi: ", "r8: ", "r9: ", "r10: ", "r11: ", "r12: ", "r13: ", "r14: ", "r15: ", "rsp: "};
    uint64_t v[16] = {0};
    get_InfoReg(v);
    newLine();
    for(int i=0;i<16;i++){
        print(regs[i]);
        print_num(v[i],1);
        newLine();
    }
}
static void cleanBuffer(){
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        terminalBuffer[i] = 0;
    }
    bufferSize = 0;
}
static void testDivisionBy0Command()
{
    int a = 4 / 0;
}
static int readNewInput()
{
    
    char chartoadd=read_input();

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
static void help(){
    print("Los comandos a disposicion del usuario son los siguientes:");
    newLine();
    newLine();
    for(int i=0;i<commandsSize;i++){
        print(commandList[i].command_name);
        print(commandList[i].desc);
        newLine();
    }
}
static void get_time(){
    print("current time (UTC): ");
    print_num(get_RTC(2),0);
    print(":");
    print_num(get_RTC(1),0);
    print(":");
    print_num(get_RTC(0),0);
}
void testIvalidOpCodeCommand()
{
    void (*punt)(void) = (void *)0x400000;
    *((uint64_t *)punt) = 0xffffffff;
    (punt)();
}

void fillCommandList()
{
    fillCommand("help",": Despliega al usuario los comandos disponibles",&help);
    fillCommand("get_time",": Muestra la hora actual",&get_time);
    fillCommand("inforeg",": Imprime en pantalla el valor de todos los registros", &inforeg);
    fillCommand("get_CPUinfo",": Imprime informacion del cpu", &CPUinfo);
    fillCommand("test_divisionby0",": Ejemplo de excepcion de dividir por 0" ,&testDivisionBy0Command);
    fillCommand("test_invalidop",": Ejemplo de excepcion por operación invalida" ,&testIvalidOpCodeCommand);

}
static void CommandHandler()
{
    char potentialCommand[MAX_COMDESC] = {0};
    strncpy(terminalBuffer, potentialCommand, bufferSize);

    for (int i = 0; i < commandsSize; i++)
    {
        if (strcmp(potentialCommand, commandList[i].command_name))
        {
            (commandList[i].cmdptr)();
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
void shell()
{
    put_char('>');
    while(1){
        if(readNewInput()){
            CommandHandler();
            put_char('>');
            cleanBuffer();
            
        }
    }
}