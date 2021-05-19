#include <libasm.h>
#include <shell.h>
#include <Lib.h>
#include <chess_shell.h>

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

static void printMem(uint8_t* mem){
    uint8_t vec[32] = {0};
    get_Memory(mem, vec);
    for(int i=0;i<32;i++){
        if(i!=0 && i%4==0){
            newLine();
        }
        print_num(vec[i],1);
        print("  ");
    } 
    newLine();
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
    else if(chartoadd==TAB){
        print("hola");
        return 0;
    }
    //If its a regular letter.
    else
    {
        if (bufferSize <= 100)
        {
            terminalBuffer[bufferSize++] = chartoadd;
            put_char(chartoadd);
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
    print("Current time (UTC): ");
    print_num(get_RTC(2),0);//horas
    print(":");
    print_num(get_RTC(1),0);//minutos
    print(":");
    print_num(get_RTC(0),0);//segundos
}

static void testIvalidOpCodeCommand()
{
    void (*punt)(void) = (void *)0x400000;
    *((uint64_t *)punt) = 0xffffffff;
    (punt)();
}

static void draw_Main_Sreen(){
    print("WELCOME TO chessOS, espero que le guste mucho el ajedrez");
    newLine();
    newLine();
    print("Ingrese el comando help para comenzar");
    newLine();
    newLine();
}

static void clean(){
    clearScreen();
    draw_Main_Sreen();
}

void fillCommand(char* name,char *desc, void (*cmdptr)())
{
    command aux;
    strncpy(name,aux.command_name,0,strlen(name));
    strncpy(desc, aux.desc,0, strlen(desc));
    aux.cmdptr = cmdptr;
    commandList[commandsSize++] = aux;
}

void fillCommandList()
{
    fillCommand("help",": Despliega al usuario los comandos disponibles",&help);
    fillCommand("get_time",": Muestra la hora actual",&get_time);
    fillCommand("inforeg",": Imprime en pantalla el valor de todos los registros (con ctrl se guardan los registros)", &inforeg);
    fillCommand("get_CPUinfo",": Imprime informacion del cpu", &CPUinfo);
    fillCommand("test_divisionby0",": Ejemplo de excepcion de dividir por 0" ,&testDivisionBy0Command);
    fillCommand("test_invalidop",": Ejemplo de excepcion por operacion invalida" ,&testIvalidOpCodeCommand);
    fillCommand("printMem",": realiza en memoria un volcado de memoria de 32 bytes a partir de la direccion recibida", &printMem);
    fillCommand("clean", ": Limpia la pantalla", &clean);
    fillCommand("chess",": Juego de ajedrez", &mini_shell);
}

static void CommandHandler()
{
    char potentialCommand[MAX_COMDESC] = {0};
    strncpy(terminalBuffer, potentialCommand,0, bufferSize);
    for (int i = 0; i < commandsSize; i++)
    {
        if (strcmp(potentialCommand, commandList[i].command_name))
        {

            (commandList[i].cmdptr)();
            if(strcmp(potentialCommand,"chess")){
                draw_Main_Sreen();
                return;
            }
            newLine();
            return;
        }
    }
    char printmemcommand[MAX_COMDESC]={0};
    char arg[MAX_COMDESC] = {0};
    strncpy(potentialCommand,printmemcommand,0,9);
    if(strcmp(printmemcommand,"printMem ")){
        strncpy(potentialCommand,arg,11,strlen(potentialCommand));
        uint8_t* pointer=strToNumHex(arg);
        printMem(pointer);
        return;
    }
    //If command not found
    print("Not a valid command: ");
    print(potentialCommand);
    newLine();
}

void shell()
{
    fillCommandList();
    draw_Main_Sreen();
    put_char('>');
    while(1){
        if(readNewInput()){
            CommandHandler();
            put_char('>');
            cleanBuffer();
        }
    }
}
