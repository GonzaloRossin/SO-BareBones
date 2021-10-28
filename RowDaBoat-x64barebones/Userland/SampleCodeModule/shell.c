#include "Include/shell.h"
#include "Include/Lib.h"
#include "Include/tests.h"

#define BUFFER_SIZE 100
#define MAX_COMDESC 100
#define MAX_COMMANDS 20
#define MAX_ARGS 3

int SCREEN_FLAG=0;

//Structure of a command stored in the command List
typedef struct command
{
    char command_name[MAX_COMDESC];
    char desc[MAX_COMDESC];
    void (*cmdptr)(int, int, int);
    int arg_q;//argument qty for this command
} command;

//Store all the commands in this array.
static command commandList[MAX_COMMANDS];
static int commandsSize = 0;

//Buffer to store the input from the keyboard.
static char terminalBuffer[BUFFER_SIZE + 1] = {0}; //Non cyclic buffer
int buffersize;


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
    buffersize = 0;
}

static void testDivisionBy0Command()
{
    int a =1;
    a = a/0;
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
        actionCall(0);
        return 1;
    }
    else if(chartoadd==BACKSPACE){
        if (buffersize > 0)
        {
            terminalBuffer[--buffersize] = 0;
            actionCall(1);
        }
        return 0;
    }
    //If its a regular letter.
    else
    {
        if (buffersize <= 100)
        {
            terminalBuffer[buffersize++] = chartoadd;
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
// source: https://www.felixcloutier.com/x86/ud.
static void testIvalidOpCodeCommand()
{
    __asm__("ud2");
}
void sleep(int seg){
    int actual_time=get_seconds(),aux;
    while(aux=get_seconds()-actual_time<seg){

    }
}
static void draw_Main_Screen(){
    print("Welcome to arquiOS");
    newLine();
    newLine();
    print("Ingrese el comando help para comenzar");
    newLine();
    newLine();
}

static void clean(){
    clearScreen();
    draw_Main_Screen();
}

void get_mem_info(){
    mm_stat aux=Mmem();
    print("total memory: ");
    print_num((int)(aux.total),0);
    newLine();
    print("memory ocuppied: ");
    print_num((int)(aux.occupied),0);
    newLine();
    print("free memory: ");
    print_num((int)(aux.free),0);
    newLine();
    print("number of allocations: ");
    print_num((int)(aux.successful_allocs),0);
    newLine();
    print("number of frees: ");
    print_num((int)(aux.successful_frees),0);
    newLine();
    print("memory manager: ");
    print(aux.sys_name);
    newLine();
}
 
int testProcess2Main(int argc, char ** argv) { 
    print("in tester"); 
    for (unsigned int i = 0; i < argc; i++) { 
        print("process\n");
        put_char('>');
        sleep(1);
    } 
    newLine();
    put_char('>');
    return 0; 
} 

void printLoop(int a1, int a2) { 
    main_func_t proc2 = {testProcess2Main, 100, NULL}; 
    int aux = exec(&proc2, "test Process", 0); 
    print("Process created ");
    print_num(aux, 0);
} 
static void pKill(int pid){
    kill(pid);
}
static void test_sync1(){
    test_sync();
    newLine();
    put_char('>');
}
static void test_sync2(){
    test_no_sync();
    newLine();
    put_char('>');
}
void fillCommand(char* name,char *desc, void (*cmdptr)(), int arg_q)
{
    command aux;
    strncpy(name,aux.command_name,0,strlen(name));
    strncpy(desc, aux.desc,0, strlen(desc));
    aux.cmdptr = cmdptr;
    aux.arg_q = arg_q;
    
    commandList[commandsSize++] = aux;
}

void fillCommandList()
{
    fillCommand("help",": Despliega al usuario los comandos disponibles", &help, 0);
    fillCommand("get_time",": Muestra la hora actual", &get_time, 0);
    fillCommand("inforeg",": Imprime en pantalla el valor de todos los registros (con ctrl se guardan los registros)", &inforeg, 0);
    fillCommand("test_divisionby0",": Ejemplo de excepcion de dividir por 0", &testDivisionBy0Command, 0);
    fillCommand("test_invalidop",": Ejemplo de excepcion por operacion invalida", &testIvalidOpCodeCommand, 0);
    fillCommand("printMem",": realiza en memoria un volcado de memoria de 32 bytes a partir de la direccion recibida", &printMem, 1);
    fillCommand("clean", ": Limpia la pantalla", &clean, 0);
    fillCommand("test_mem", ": Testeo de memoria", &test_mm, 0);
    fillCommand("ps", ": Imprime el estado de los procesos vivos", &ps, 0);
    fillCommand("kill", ": Mata a un proceso dado su ID", &pKill, 1);
    fillCommand("nice", ": Cambia la prioridad de un proceso dado su ID y la nueva prioridad", &nice, 2);
    fillCommand("block", ": Cambia el estado de un proceso entre bloqueado y listo dado su ID", &block, 1);
    fillCommand("mem",": muestra el estado de la memoria heap (bytes libres respecto del total)", &get_mem_info, 0);
    fillCommand("test_sync",": realiza el test de sincronizacion de semaforos de la catedra",&test_sync1,0);
    fillCommand("test_no_sync",": realiza el segundo test de sincronizacion de semaforos de la catedra",&test_sync2,0);
    fillCommand("loop",": testea la creacion de un proceso", &printLoop, 0);
}

int parse_command(char* potentialCommand, char* command, char args[MAX_ARGS][MAX_COMDESC]){
	int params_read = 0, j = 0, i = 0;

    //building command
	while(potentialCommand[i] != '\0' && potentialCommand[i] != ' ' && i < BUFFER_SIZE){
		command[i] = potentialCommand[i];
		i++;
	}
    command[i] = '\0';

    if (potentialCommand[i] == '\0')
        return params_read;

	//en potentialCommand[i] me quedo un espacio
	if(i < BUFFER_SIZE){
		i++;
	}
	//building params
	while(potentialCommand[i] != '\0' && params_read <= MAX_ARGS){
		if(potentialCommand[i] != ' '){
			args[params_read][j++] = potentialCommand[i];
		}else{
            args[params_read++][j] = '\0';
			j=0;
		}
		i++;	
    }

	if(potentialCommand[i] == '\0'){ //si corto porque se acabo el string --> me quedo un param mas
		args[params_read++][j] = '\0';
    } 

	if(params_read > MAX_ARGS){
		return -1; 
    }

	return params_read; 
}
static void CommandHandler()
{
    char potentialCommand[MAX_COMDESC] = {0};
    strncpy(terminalBuffer, potentialCommand,0, buffersize);
    char command[MAX_COMDESC];
    char args[MAX_ARGS][MAX_COMDESC];
    for(int i=0;i<MAX_ARGS;i++){
        for(int j=0;j<MAX_COMDESC;j++){
            args[i][j]=0;
        }
    }
    int args_q_read = parse_command(potentialCommand, command,args);
//
    for (int i = 0; i < commandsSize; i++)
    {
        if (strcmp(command, commandList[i].command_name))
        {
            if(args_q_read != commandList[i].arg_q){
                newLine();
                print("Cantidad invalida de argumentos para el comando: ");
                print(command);
                newLine();
                print("Argumentos recibidos: ");
                print_num(args_q_read,0);
                print(", Argumentos esperados: ");
                print_num(commandList[i].arg_q,0);
                newLine();
                return; 
            }
            if(commandList[i].arg_q == 0){
                (commandList[i].cmdptr)(0,0,0);
                newLine();
                return;
            } else if(commandList[i].arg_q == 1){
                (commandList[i].cmdptr)(strToInt(args[0]),0,0);
                newLine();
                return;
            } else if(commandList[i].arg_q == 2){
                (commandList[i].cmdptr)(strToInt(args[0]),strToInt(args[1]),0);
                newLine();
                return;
            } else if(commandList[i].arg_q == 3){
               (commandList[i].cmdptr)(strToInt(args[0]),strToInt(args[1]), strToInt(args[2]));
                newLine();
                print("done");
                newLine();
                return;
            } else {
                print("failatron");
            }
        }
    }

    //If command not found
    print("Not a valid command: ");
    print(potentialCommand);
    newLine();
}

void initializeOS(){
    draw_Main_Screen();
    put_char('>');
    buffersize=0;
}
void shell()
{
    fillCommandList();
    initializeOS();
    while(1){
        if(readNewInput()){
            CommandHandler();
            put_char('>');
            cleanBuffer();
            sleep(1);
        }
    }
}
