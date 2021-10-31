#include "pipe.h"

static space pipes[MAX_PIPES];
uint64_t semPipeManager;

static uint64_t validIndex(uint64_t pipeIndex);
//static uint64_t createPipe(char *name);
static uint64_t findPipe(char *name);
static uint64_t findAvailable();


//Retorna -1 en caso de error.
uint64_t initPipes(){
    if ((semPipeManager = semOpen("Pipe Manager",1)) == -1)
    {
        printf("Error en initPipe");
        return -1;
    }
    
    for (int i = 0; i < MAX_PIPES; i++)
    {
        pipes[i].available = TRUE;
    }
    return 0;
}

static uint64_t findAvailable(){
    for (int i = 1; i < MAX_PIPES; i++)
    {
        if (pipes[i].available == TRUE)
        {
            pipes[i].available = FALSE;
            return i;
        }
    }
    return -1; // están todos ocupados
}

//Retorna el id del pipe en caso de exito, -1 si se encontro algun tipo de error
static uint64_t createPipe(char *name){
    //printf("Creando el pipe: ");
    //printf(name);

    int len = Strlen(name);
    if (len <= 0 || len >= MAX_NAME_LENGTH)
    {
        printf("Nombre del pipe es demasiado largo\n");
        return -1;
    }

    uint64_t pos;
    if ((pos = findAvailable()) != -1)    {
        pipe_t *newPipe = &pipes[pos].pipe;
        // Inicializamos la estructura
        memcpy(newPipe->name, name, len);
        newPipe->rIndex = 0;
        newPipe->wIndex = 0;

        char nameR[MAX_NAME_LENGTH];
        memcpy(nameR, name, len);
        nameR[len] = 'R';
        nameR[len + 1] = 0;

        uint64_t semRead = semOpen(nameR, 0);

        char nameW[MAX_NAME_LENGTH];
        memcpy(nameW, name, len);
        nameW[len] = 'W';
        nameR[len + 1] = 0;
        uint64_t semWrite = semOpen(nameW, PIPE_BUFFER_SIZE);

        if (semRead == -1 || semWrite == -1)        {
            printf("Error en los sem del pipe");
            return -1;
        }
        newPipe->semRead = semRead;
        newPipe->semWrite = semWrite;
        newPipe->id = pos;
    }
    return pos;
}

//si no existe retorna -1, si existe retorna la posición
static uint64_t findPipe(char *name){
    for (int i = 1; i < MAX_SEM; i++){
        if (pipes[i].available == FALSE && Strcmp(name, pipes[i].pipe.name)){
            return i;
        }
    }
    return -1;
}

//retorna -1 en caso de error
uint64_t pipeOpen(char *name){
    if (semWait(semPipeManager)){
        printf("error semWait abriendo el pipe\n");
        return -1;
    }

    int id = findPipe(name);
    if (id == -1){
        //Si no existe un pipe con ese nombre
        id = createPipe(name);
        //printf("\ncreated pipe ");
        //draw_decimal(id);
        newLine();
    } else {
        //printf("\nopened pipe ");
        //draw_decimal(id);
        newLine();
    }
    
    //si el create dió error, id sigue en -1
    if (id == -1){
        printf("error en pipeOpen, id=-1\n");
        semPost(semPipeManager);
        return -1;
    }
    if (semPost(semPipeManager)){
        printf("error semPost abriendo el pipe\n");
        return -1;
    }
    pipes[id].pipe.amountProcesses++;
    return id;
}

//returns TRUE if pipe exists and is being used by someone
static uint64_t validIndex(uint64_t pipeIndex){
    if(pipeIndex < 0 || pipeIndex > MAX_PIPES){
        printf("invalid pipe index\n");
        return FALSE;
    } else if (pipes[pipeIndex].available){
        printf("apipe not being used\n");
        return FALSE;
    } else {
        //available and occupied pipe
        return TRUE;
    }
}

uint64_t pipeClose(uint64_t pipeIndex){
    if (!validIndex(pipeIndex)){
        printf("invalid pipe to close\n");
        return -1;
    }
    if (semWait(semPipeManager) == -1){
        printf("error semWait en pipeClose\n");
        return -1;
    }

    pipes[pipeIndex].available = TRUE;
    int closeRead = semClose("semRead");
    int closeWrite = semClose("semWrite");

    if (closeRead == -1 || closeWrite == -1){
        printf("error en los sem close del pipe\n");
        return -1;
    }

    if (semPost(semPipeManager) == -1){
        printf("error semPost en pipeClose\n");
        return -1;
    }
    return 1;
}

uint64_t writeChar(uint64_t pipeIndex, char c){
    if (!validIndex(pipeIndex))
        return -1;

    pipe_t * pipe = &pipes[pipeIndex].pipe;

    if (semWait(pipe->semWrite) == -1){
        printf("Error semWait en writeChar\n");
        return -1;
    }
    pipe->buffer[pipe->wIndex % PIPE_BUFFER_SIZE] = c;
    pipe->wIndex++;
    if (semPost(pipe->semRead) == -1){
        printf("Error semPost en writeChar\n");
        return -1;
    }
    return 1;
}

uint64_t writePipe(uint64_t pipeIndex, char *string){
    //printf(" writing on pipe");
    //draw_decimal(pipeIndex);
    
    if (!validIndex(pipeIndex))
        return -1;

    while (*string != 0){
        if ((writeChar(pipeIndex, *string++)) == -1)
            return -1;
    }
    return 0;
}

char readPipe(uint64_t pipeIndex){
    if (!validIndex(pipeIndex))
        return -1;

    pipe_t *pipe = &pipes[pipeIndex].pipe;
    if (semWait(pipe->semRead) == -1){
        //printf("Error semWait en readPipe\n");
        return -1;
    }
    char c = pipe->buffer[pipe->rIndex % PIPE_BUFFER_SIZE];
    pipe->rIndex++;
    if (semPost(pipe->semWrite) == -1){
        printf("Error semPost en readPipe\n");
        return -1;
    }
    return c;
}

void pipe(){
    printf("NOMBRE\tID\tESTADO\t\tSEM USADO\t\t\t\tPROCESOS BLOQUEADOS\n");
    int i;
    for(i = 0; i < MAX_PIPES; i++){
        if(!(pipes[i].available)){
            printPipe(pipes[i].pipe);
        }
    }
    printf("\n");
}
void printPipe(pipe_t pipe){
    printf(pipe.name);
    printf("\t");
    draw_decimal(pipe.id);
    printf("\t\t\t");
    printf("Activo\t\t");
    printSemsInvovled(pipe);
    printf("\t");
    printProcesses(pipe);
    printf("\n");
}

void printSemsInvovled(pipe_t pipe){
    printf(getSemName(pipe.semRead));
    printf(", ");
    printf(getSemName(pipe.semWrite));
    printf(", ");
    printf(getSemName(semPipeManager));
}
void printProcesses(pipe_t pipe){
    printProcessesSem(pipe.semRead);
    printf(" ");
    printProcessesSem(pipe.semWrite);
    printf(" ");
    printProcessesSem(semPipeManager);
}