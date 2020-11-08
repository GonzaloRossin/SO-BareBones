#include<shell.h>
#include<libasm.h>
#include<Lib.h>
#include<chesslib.h>
#include<chess_shell.h>

//Buffer to store the input from the keyboard.
#define BUFFER 50
#define MAX_COMMANDS 10
#define MAX_DESC 50
#define TIMER_X 850
#define TIMER_Y 40
#define ERROR_X 5
#define ERROR_Y 162
#define LOG_X 5
#define LOG_Y 214
#define GAME_DURATION_IN_SECONDS 300
#define MAX_LOGS_IN_SCREEN 64
#define CHAR_WIDTH 8
#define CHAR_HEIGHT 1
#define BACKGROUND_COLOR1 0x0000FF

#define ISCOL(c) (( (c <= 'h'&& c>='a') || (c<='H' && c>='A'))  ? 1 : 0)
#define ISROW(c) ( (c>='0' && c<='8') ? 1 : 0)

int FLAG_START=0;
int FLAG_END=0;
int SAVE_FLAG=0;
int EXIT_FLAG=0;
int TURNED_BOARD=0;
int log_x=LOG_X;
int log_y=LOG_Y;
int FILL_LIST=0;

static void change_log_page(int page);

matrix_struct * timermatrix;

typedef struct chesscommand
{
    char command_name[MAX_DESC];
    char desc[MAX_DESC];
    void (*cmdptr)(void);
} chesscommand;

typedef struct playertime{
    unsigned long time_reference;
    int total_seconds;
}playertime;

typedef struct player{
    char * name;
    playertime timer;
    int color;
}player;

typedef struct log{
    char from[3];
    char to[3];
    char * playername;
    int order;
}log;

log loghistory[200];
int logsize=0;
int log_pages=0;
int current_log_page = 0;
player players[2];
int currentplayer;

chesscommand chess_commands[MAX_COMMANDS];
int command_size = 0;

static char buffer[BUFFER+ 1] = {0}; //Non cyclic buffer
static int buffer_size = 0;

static void print_time(){
    int timedata[2][2]={{players[0].timer.total_seconds/60,players[0].timer.total_seconds%60},
                        {players[1].timer.total_seconds/60,players[1].timer.total_seconds%60}};//minutos[0],segundos[1]
    sys_cursor(TIMER_X,TIMER_Y);
    for(int j=0;j<2;j++){
        print(players[j].name);
        for(int i=0;i<2;i++){//IMPRIMO MINUTOS SI I=0 Y SEGUNDOS SI I=1
            int dec=timedata[j][i]/10;
            int res=timedata[j][i]%10;
            print_num(dec,0);
            print_num(res,0);
            if(i==0)
                put_char(':');
        }
        sys_cursor(-1,-1);//VUELVO A LA POSICION DEL SHELL
        sys_cursor(TIMER_X,TIMER_Y-25);
    }
    sys_cursor(-1,-1);
}

static void printlog(log * log){

    if(log_y>=716){
        log_x=230;
        log_y=LOG_Y;
    }
    sys_cursor(log_x,log_y);
    print(log->playername);
    print(log->from);
    print(" ---> ");
    print(log->to);
    put_char(' ');
    put_char('(');
    print_num(log->order,0);
    put_char(')');
    sys_cursor(-1, -1);
    log_y+=16;
}

static void change_log_page(int page){
    current_log_page = page;

    sys_cursor(LOG_X,LOG_Y-16);
    print("Logs: page ");
    print_num(current_log_page,0);
    sys_cursor(-1,-1);

    for (int i = LOG_Y; i <= 716; i+=16)
    {
        sys_cursor(459, i);
        putActioncall(3);
        sys_cursor(-1,-1);
    }

    log_x = LOG_X;
    log_y = LOG_Y;

    for (int j = MAX_LOGS_IN_SCREEN*page; j < MAX_LOGS_IN_SCREEN*(page+1) && j < logsize; j++)
    {
        log * aux = &loghistory[j];
        printlog(aux);
    }
}

static void draw_chess_Sreen(){
    clearScreen();
    print("WELCOME TO CHESS");
    newline();
    print("Los comandos a disposicion del usuario son los siguientes:");
    newLine();
    newLine();
    for(int i=0;i<command_size;i++){
        print(chess_commands[i].command_name);
        print(chess_commands[i].desc);
        newLine();
    }
    newLine();
    print("Presione \"0\" para girar el tablero 90 grados");
    newLine();
    print("Para mover un pieza debe escribir origen-destino, por ejemplo a1-a2 o A1-A2");
    newLine();
    newLine();
    sys_cursor(LOG_X,LOG_Y-16);
    print("Logs: page ");
    print_num(current_log_page,0);
    sys_cursor(-1,-1);
}

static void decrecetime(){
    int remaining_time=players[currentplayer].timer.total_seconds-(get_seconds()-players[currentplayer].timer.time_reference);
    int diff = abs(players[!currentplayer].timer.total_seconds-players[currentplayer].timer.total_seconds);
    players[currentplayer].timer.total_seconds=remaining_time;
    players[currentplayer].timer.time_reference=get_seconds();
    print_time();
    //Si se acaba el tiempo o la diferencia es mayor a un minuto se termina el juego.
    if(remaining_time==0 || diff > 60){
        FLAG_END=1;
    }
}

static void start(){
    if(!SAVE_FLAG){

        initialize_chess();
        currentplayer=0;
        for(int i=0;i<2;i++){
            player * aux = &players[i];
            aux->timer.total_seconds=GAME_DURATION_IN_SECONDS;
            aux->timer.time_reference=get_seconds();
        }
        players[0].name="Player 1 : ";
        players[1].name="Player 2 : ";
        players[0].color = 0xFFFFFF;
        players[1].color = 0x000000;
        FLAG_END=0;
        logsize=0;
        log_pages=0;
        current_log_page = 0;
        log_x = LOG_X;
        log_y = LOG_Y;
    }
    else{
        players[0].timer.time_reference=get_seconds();
        players[1].timer.time_reference=get_seconds();
        SAVE_FLAG = 0;
    }
    FLAG_START=1;
}

static void cleanChessBuffer(){
    for (int i = 0; i < BUFFER; i++)
    {
       buffer[i] = 0;
    }
    buffer_size = 0;
}

static void exit(){
    cleanChessBuffer();
    if (FLAG_START && !FLAG_END)
    {
        SAVE_FLAG=1;
    }
    EXIT_FLAG=1;
    FLAG_START=0;
    FLAG_END=0;
    clearScreen();
}

static void playerswap(){
    if(currentplayer==1)
        currentplayer=0;
    else
        currentplayer=1;
    players[currentplayer].timer.time_reference=get_seconds();
    turn_board_180();
}

static int readChessInput()
{
    
    char chartoadd=read_input();

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
    else if(chartoadd=='0')
    {
        if(!TURNED_BOARD){
            turn_board_90();
            TURNED_BOARD=1;
        }
        else{
            turn_board_normal();
            TURNED_BOARD=0;
        }
        return 0;
    } 
    else if (chartoadd=='l')
    {
        if (log_pages>current_log_page)
        {
            change_log_page(current_log_page+1);
        }
        return 0;
    }
    else if (chartoadd=='k')
    {
        if (current_log_page>0)
        {
            change_log_page(current_log_page-1);
        }
        return 0;
    }
    else if(chartoadd==ESC){
        exit();
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

static void resetgame(){
    clearScreen();
    draw_chess_Sreen();
    FLAG_START=0;
    FLAG_END=0;
    SAVE_FLAG=0;
    logsize=0;
    log_pages=0;
    current_log_page = 0;
    log_x = LOG_X;
    log_y = LOG_Y;
}

static void fillChessCommand(char* name,char *desc, void (*cmdptr)(void))
{
    chesscommand aux;
    strncpy(name,aux.command_name,0,strlen(name));
    strncpy(desc, aux.desc,0, strlen(desc));
    aux.cmdptr = cmdptr;
    chess_commands[command_size++] = aux;
}

static void fillChessList()
{
    fillChessCommand("start"," : inicia o reanuda el juego",&start);
    fillChessCommand("reset"," : resetea el juego y vuelve al menu de inicio",&resetgame);
}

static int CommandHandlerChess(){
    char potentialCommand[BUFFER] = {0};
    strncpy(buffer, potentialCommand,0, buffer_size);

    sys_cursor(400,ERROR_Y); //PARA BORRAR EL MENSAJE PREVIO
    putActioncall(3);
    sys_cursor(-1,-1);

    for (int i = 0; i < command_size; i++)
    {
        if (strcmp(potentialCommand, chess_commands[i].command_name))
        {
            if (strcmp(potentialCommand, "start") && FLAG_START)
            {
                sys_cursor(ERROR_X,ERROR_Y);
                print("Game already started");
                sys_cursor(-1,-1);
                return 0;
            }
            
            (chess_commands[i].cmdptr)();
            return 0;
        }
        else if(ISCOL(potentialCommand[0]) && ISROW(potentialCommand[1]) && potentialCommand[2]=='-' && ISCOL(potentialCommand[3]) && ISROW(potentialCommand[4])){

            if (FLAG_START==0)
            {
                sys_cursor(ERROR_X,ERROR_Y);
                print("Start a new game");
                sys_cursor(-1,-1);
                return 0;
            }

            chess_square * origin = get_board_tile(potentialCommand[1]-'0', potentialCommand[0]);
            chess_square * destiny = get_board_tile(potentialCommand[4]-'0', potentialCommand[3]);

            if (validate_player(origin, players[currentplayer].color)==0)
            {
                sys_cursor(ERROR_X,ERROR_Y);
                print("Invalid Move");
                sys_cursor(-1,-1);
                return 0;
            }

            int validate = validate_move(origin, destiny);
            // Imprime el movimiento
            if(validate>=0){
                char movefrom[3]={0};
                char moveto[3]={0};
                strncpy(potentialCommand,movefrom,0,2);
                strncpy(potentialCommand,moveto,3,5);
                log * aux = &loghistory[logsize++];
                strncpy(movefrom,aux->from,0,3);
                strncpy(moveto,aux->to,0,3);
                aux->playername = players[currentplayer].name;
                aux->order=logsize;
                if (current_log_page!=log_pages)
                {
                    change_log_page(log_pages);
                }
                if (logsize==MAX_LOGS_IN_SCREEN*(log_pages+1))
                {
                    change_log_page(current_log_page+1);
                    log_pages++;
                }
                printlog(aux);
            }

            // Efectua el movimiento
            if (validate == -1){
                sys_cursor(ERROR_X,ERROR_Y);
                print("Invalid Move");
                sys_cursor(-1,-1);
                return 0;
            }
            if (validate == -2)
            {
                sys_cursor(ERROR_X,ERROR_Y);
                print("Warning Check!!");
                sys_cursor(-1,-1);
                return 0;
            }
            
            if (validate == 0)
            {
                int checkmate = move(origin, destiny);
                if (checkmate == 1)
                {
                    FLAG_END = 1;
                    return 1;
                }
                sys_cursor(ERROR_X,ERROR_Y);
                print(players[!currentplayer].name);
                print("your turn to play");
                sys_cursor(-1,-1);
                return 1;
            }
            if (validate == 1)
            {
                sys_cursor(ERROR_X,ERROR_Y);
                print("Castling");
                sys_cursor(-1,-1);
                castling_move(origin, destiny);
                return 1;
            }
            if (validate == 2)
            {
                al_paso_move(origin, destiny);
                return 1;
            }
        } 
    }
    sys_cursor(ERROR_X,ERROR_Y);
    print("Invalid command ");
    sys_cursor(-1,-1);
    return 0;
}

int mini_shell(){

    if(!FILL_LIST){
        fillChessList();
        FILL_LIST=1;
    }
    draw_chess_Sreen();
    cleanChessBuffer();

    log_x=LOG_X;
    log_y=LOG_Y;

    // Si habia un juego sin terminar lo restaura.
    if(SAVE_FLAG){
        sys_cursor(ERROR_X,ERROR_Y);
        print(players[currentplayer].name);
        print("your turn to play");
        sys_cursor(-1,-1);
        change_log_page(current_log_page);
        print_time();
        draw_board();
        draw_tags();
    }
    put_char('>');
    EXIT_FLAG=0;
    while(1 && !EXIT_FLAG){
        if(FLAG_START){
            decrecetime();
        }
        if(readChessInput()){
            if(CommandHandlerChess()){
                playerswap();
            }
            put_char('>');
            cleanChessBuffer();
        }
        if (FLAG_END)
        {
            FLAG_START = 0;
            SAVE_FLAG = 0;
            FLAG_END = 0;
            sys_cursor(ERROR_X,ERROR_Y);
            print("JUEGO FINALIZADO, ");
            print("gana el jugador: ");
            print(players[!currentplayer].name);
            for(int i=0;i<2;i++){
                putActioncall(1);
            }
            sys_cursor(-1,-1);
        }
        
    }
    return 0;
}