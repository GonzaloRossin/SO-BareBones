#include<shell.h>
#include<libasm.h>
#include<Lib.h>

//Buffer to store the input from the keyboard.
#define BUFFER 50
#define MAX_COMMANDS 10
#define MAX_DESC 50

typedef struct chesscommand
{
    char command_name[MAX_DESC];
    char desc[MAX_DESC];
    void (*cmdptr)(void);
} chesscommand;

typedef struct playertime{
    unsigned long time_reference;
    unsigned long seconds_passed;
    int total_seconds;
}playertime;


typedef struct player{
    playertime timer;
}player;

static int FLAG_START=0;
static player players[2];
static int currentplayer;

static chesscommand chess_commands[MAX_COMMANDS];
static int command_size = 0;

static char buffer[BUFFER+ 1] = {0}; //Non cyclic buffer
static int buffer_size = 0;

void decrecetime(player player){
    player.timer.seconds_passed=get_seconds();
    print_time(player);
}
void start(){
    setTimereference();
    FLAG_START=1;
    currentplayer=0;
}
void setTimereference(){
    player aux;
    for(int i=0;i<2;i++){
        aux.timer.time_reference=0;
        aux.timer.seconds_passed=0;
        aux.timer.total_seconds=300;// ACA SE SETEA LA DURACION DE LA PARTIDA EN SEGUNDOS
        players[i]=aux;
    }
}
void playerswap(){
    currentplayer=!currentplayer;
}
void print_time(player player){
    int remaining_time=player.timer.total_seconds-(player.timer.seconds_passed-player.timer.time_reference);
    int mins=remaining_time/60;
    int seconds=remaining_time % 60;
    if(mins<10){
        put_char('0');
    }
    print_num(mins,0);
    put_char(':');
    if(seconds<10){
        put_char('0');
    }
    print_num(seconds,0);
}
static void chess_help(){
    print("Los comandos a disposicion del usuario son los siguientes:");
    newLine();
    newLine();
    for(int i=0;i<command_size;i++){
        print(chess_commands[i].command_name);
        print(chess_commands[i].desc);
        newLine();
    }
}
static int readChessInput()
{
    
    char chartoadd=read_input();

    //If there is nothing new or its not a valid character...
    if (chartoadd == 0)
    {
        return 0;
    }
    else if(chartoadd==ENTER){
        putActioncall(3);
        return 1;
    }
    else if(chartoadd==BACKSPACE){
        if (buffer_size > 0)
        {
            buffer[--buffer_size] = 0;
            putActioncall(1);
        }
        return 0;
    }
    //If its a regular letter.
    else
    {
        if (buffer_size <= 100)
        {
            buffer[buffer_size++] = chartoadd;
            put_char(chartoadd);
            return 0;
        }
    }

    //Just in case
    return 0;
}
static void cleanChessBuffer(){
    for (int i = 0; i < BUFFER; i++)
    {
       buffer[i] = 0;
    }
    buffer_size = 0;
}
void fillChessCommand(char* name,char *desc, void (*cmdptr)(void))
{
    chesscommand aux;
    strncpy(name,aux.command_name,0,strlen(name));
    strncpy(desc, aux.desc,0, strlen(desc));
    aux.cmdptr = cmdptr;
    chess_commands[command_size++] = aux;
}
void fillCommandsChess()
{
    fillChessCommand("start",": inicia el tiempo y el juego",&start);
    fillChessCommand("help",": muestra comandos disponibles",&chess_help);

}
void CommandHandlerChess(){
    char potentialCommand[BUFFER] = {0};
    strncpy(buffer, potentialCommand,0, buffer_size);
    for (int i = 0; i < command_size; i++)
    {
        if (strcmp(potentialCommand, chess_commands[i].command_name))
        {
            (chess_commands[i].cmdptr)();
            putActioncall(3);
            playerswap();
            return;
        }
    }
}

void mini_shell(){
    put_char('>');
    while(1){
        if(readChessInput()){
            CommandHandlerChess();
            put_char('>');
            cleanChessBuffer();
        }
    }
}