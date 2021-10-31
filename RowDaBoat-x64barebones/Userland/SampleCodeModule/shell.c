#include "Include/shell.h"
#include "Include/Lib.h"
#include "phylo.h"
#include "Include/tests.h"

#define BUFFER_SIZE 100
#define MAX_COMDESC 100
#define MAX_COMMANDS 30
#define MAX_ARGS 3

int SCREEN_FLAG=0;

//Structure of a command stored in the command List
typedef struct command
{
    char command_name[MAX_COMDESC];
    char desc[MAX_COMDESC];
    void (*cmdptr)(char*,char*, uint64_t[2]); //2 args y FD al final

    int arg_q;//argument qty for this command
} command;

//Store all the commands in this array.
static command commandList[MAX_COMMANDS];
static int commandsSize = 0;

//Buffer to store the input from the keyboard.
static char terminalBuffer[BUFFER_SIZE + 1] = {0}; //Non cyclic buffer
int buffersize;

static int foreground = 1;


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

static void printMem(char* mem_char){
    int mem_int = strToInt(mem_char);
    uint8_t* mem = (uint8_t*)mem_int;
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
void sleep(int seg) {
    int actual_time=get_seconds(),aux;
    while(aux=get_seconds()-actual_time<seg){

    }
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

int loopMain() {
    int pid = getPid();
    for (unsigned int i = 0; 1; i++) {
        wait(1000);
        print("hola soy pid: "); print_num(pid, 0); newLine();
    }
    newLine();
    put_char('>');
    return 0;
}

void loop(int nada, int nada2, uint64_t fd[2]) {
    main_func_t proc2 = {loopMain, NULL, NULL};
    int aux = exec(&proc2, "test Process", 0, fd);
    print("Process created ");
    print_num(aux, 0);
}

static void pKill(char* pid_char){
    int pid = strToInt(pid_char);
    kill(pid);
    print("process: ");
    print_num(pid,0);
    print(" killed");
}
static void pNice(char* pid_char, char* priority_char){
    int pid = strToInt(pid_char);
    int priority = strToInt(priority_char);
    nice((pid_t)pid,(unsigned int)priority);
}


static void test_sync1(){
    main_func_t aux = {main_test_sync, 0, NULL};
    int pid = exec(&aux, "test sync", 0, NULL);
    newLine();
    put_char('>');
}
static void test_sync2(){
    main_func_t aux = {main_test_no_sync, 0, NULL};
    int pid = exec(&aux, "test no sync", 0, NULL);
    newLine();
    put_char('>');
}
static void list_semaphores(){
    list_sem();
}

static void list_pipes(){
    p_list();
}

void cat_main(){
    int i = 0; //NO BORRAR ESTA LINEAAAAA
    char c;
    while((c = read_input()) != '\t'){//while(!strcmp(&c, "\t")){
        if(c == 8){
            actionCall(1);
        }else if(c != 0){
            put_char(c);
            //print(&c);
        }
    }
}

void cat(int nada, int nada2, uint64_t fd[2])
{
    main_func_t proc2 = {cat_main, NULL, NULL}; //argc, argv
    int aux = exec(&proc2, "CAT", 1, fd);
    newLine();
}


void wc_main(){
    int i = 0; //NO BORRAR
    int lines = 0;
    char c;
    while((c = read_input()) != '\t'){
        if (c == '\n'){
            lines++;
            print("new line\n");
            //newLine();
        }
    }
    print("Total amount of lines: ");
    print_num(lines,0);
    print("\n");
}

void wc(int nada, int nada2, uint64_t fd[2])
{
    main_func_t proc2 = {wc_main, NULL, NULL}; //argc, argv
    int aux = exec(&proc2, "WC", 1, fd);
    newLine();
}

int isVowel(char c){
    return (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' ||
            c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U')
               ? 1 : 0;
}

void filter_main(){
    int i = 0; //NO BORRAR
    char c;
     while((c = read_input()) != '\t'){
        if(c == 8){
            actionCall(1);
        }else if(c != 0){
            if(!isVowel(c)){
                put_char(c);
            }
        }
    }
}

void filter(int nada, int nada2, uint64_t fd[2])
{
    main_func_t proc2 = {filter_main, NULL, NULL}; //argc, argv
    int aux = exec(&proc2, "filter", 1, fd);
    newLine();
}

void blockProcess(char* pid_char) {
    int pid = strToInt(pid_char);
    if(pid == 1){
         print("Soy la shell hermano");
    } else {
        unsigned int status = 0;
        int ans = getProcessStatus(pid, &status);

        if(ans == -1 || status == KILLED) //it is KILLED or does not exist
            print("Process not found\n");

        else if ( status == READY ) {
            block(pid, BLOCKED);
            print("process "); print_num(pid, 0); print(" blocked\n");
        }
        else if( status == BLOCKED) {
            block(pid, READY);
            print("process "); print_num(pid, 0); print(" ready\n");
        }
    }
}
void execute_phylo(int nada, int nada2, uint64_t fd[2]){
    main_func_t aux = {phylo, 0, NULL}; 
    int pid = exec(&aux, "phylo", foreground, fd);
    newLine();
    put_char('>');
}

static void test_process(int nada, int nada2, uint64_t fd[2]){ 
    main_func_t aux = {test_processes, 0, NULL}; 
    int pid = exec(&aux, "test processes", 0, fd); 
    newLine(); 
    put_char('>'); 
} 

void printProcesses(void) {
    process_info info[50];
    int amount = ps(info, 50);
    print("Process number\tProcess Name\tPID\tPPID\tPriority\tRBP\tRSP\tState\tForeground\tTime left\tQuantums\n\n");
    for (unsigned int i = 0; i < amount; i++) {
        print(""); print_num(i, 0); print("\t");
        print(""); print(info[i].name); print("\t");
        print(""); print_num(info[i].pid, 0); print(" \t");
        print(""); print_num(info[i].ppid, 0); print(" \t");
        print(""); print_num(info[i].priority, 0); print(" \t");
        print("  "); print_num(info[i].rbp, 0); print("  ");
        print(""); print_num(info[i].rsp, 0); print(" \t");
        print(""); print_num(info[i].status, 0); print(" \t");
        print(""); print_num(info[i].foreground, 0); print(" \t");
        print(""); print_num(info[i].given_time, 0); print(" ticks \t");
        print(""); print_num(info[i].aging, 0); print("\n");
   }
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
    fillCommand("ps", ": Imprime el estado de los procesos vivos", &printProcesses, 0);
    fillCommand("kill", ": Mata a un proceso dado su ID", &pKill, 1);
    fillCommand("nice", ": Cambia la prioridad de un proceso dado su ID y la nueva prioridad", &pNice, 2);
    fillCommand("block", ": Cambia el estado de un proceso entre bloqueado y listo dado su ID", &blockProcess, 1);
    fillCommand("mem",": muestra el estado de la memoria heap (bytes libres respecto del total)", &get_mem_info, 0);
    fillCommand("loop",": testea la creacion de un proceso", &loop, 0);
    fillCommand("test_no_sync",": realiza el segundo test de sincronizacion de semaforos de la catedra",&test_sync2,0);
    fillCommand("test_sync",": realiza el test de sincronizacion de semaforos de la catedra",&test_sync1,0);
    fillCommand("test_process",": realiza el test de procesos de la catedra", &test_process, 0);
    fillCommand("sem",": enlista los semaforos abiertos en ese momento",&list_semaphores,0);
    fillCommand("phylo",": ejecuta el problema de los filosofos", &execute_phylo, 0);
    fillCommand("pipe",": Imprime la lista de todos los pipes con sus propiedades",&list_pipes,0);
    fillCommand("cat",": Imprime el input",&cat,0);
    fillCommand("wc",": Cuenta la cantidad de lineas del input",&wc,0);
    fillCommand("filter",": Filtra las vocales del input",&filter,0);
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
    foreground = 1;
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
            if(strcmp(args[0], ".")){//ej.. ps | cat    cat | loop
                uint64_t pipeId = (int)p_open("|");
                //print(" id: ");
                //print_num(pipeId,0);
                if(pipeId < 0){
                    print("error abriendo pipe |\n");
                } else {
                   // print("\nusing pipe ");
                   // print_num(pipeId, 0);
                }

                //en args[1] está el segundo comando                                
                //ahora voy a buscar el segundo command
                int found = 0;
                for (int j = 0; j < commandsSize; j++){
                    if (strcmp(args[1], commandList[j].command_name)){
                        uint64_t fd[2] = {0, pipeId};
                        (commandList[i].cmdptr)(0,0,fd);

                        char* str = "\t";
                        p_write(pipeId, str); //le agrego al final del pipe así el segundo comando corta cuando termina de leer el pipe buffer


                        uint64_t fd2[2] = {pipeId,0};
                        (commandList[j].cmdptr)(0,0,fd2);   
                        found = 1;

                        
                    }
                }

                //p_close(pipeId);

                if(!found){
                    print(" comando ");
                    print(args[1]);
                    print(" no encontrado\n");
                } else { return; }

                newLine();


            } else if (strcmp(args[1], ".")){//ej  loop 2 | filter
                uint64_t pipeId = p_open("|2");
                if(pipeId < 0){
                    print("error abriendo pipe |2 \n");
                }
                //en args[2] está el segundo comando
                uint64_t fd[2] = {0, pipeId};
                (commandList[i].cmdptr)(args[0],0,fd);
                //ahora voy a buscar el segundo command
                char* str = "\t";
                p_write(pipeId, str);
                int found = 0;
                for (int j = 0; j < commandsSize; j++){
                    if (strcmp(args[2], commandList[j].command_name)){
                        uint64_t fd2[2] = {pipeId,0};

                        (commandList[j].cmdptr)(0,0,fd2);   
                        found = 1;
                    }
                }
                if(!found){
                    print("comando ");
                    print(args[1]);
                    print(" no encontrado\n");
                } else { return; }

                newLine();

            } else if(strcmp(args[0], "&")) {
                foreground = 0;
                (commandList[i].cmdptr)(0,0,NULL);
                return;
            } else {                
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
                    (commandList[i].cmdptr)(0,0,NULL);
                    newLine();
                    return;
                } else if(commandList[i].arg_q == 1){
                    (commandList[i].cmdptr)(args[0],0,NULL);
                    newLine();
                    return;
                } else if(commandList[i].arg_q == 2){
                    (commandList[i].cmdptr)(args[0],args[1],NULL);
                    newLine();
                    return;
                } else if(commandList[i].arg_q == 3){
                (commandList[i].cmdptr)(args[0],args[1], NULL);
                    newLine();
                    print("done");
                    newLine();
                    return;
                } else {
                    print("failatron");
                }
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
    nice(getPid(), 0);
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
