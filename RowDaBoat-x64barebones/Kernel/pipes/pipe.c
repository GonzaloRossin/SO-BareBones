#include "pipe.h"

static space pipes[MAX_PIPES];
sem_id semPipeManager;

static uint64_t validIndex(uint64_t pipeIndex);
static uint64_t createPipe(char *name);
static uint64_t findPipe(char *name);
static uint64_t findAvailable();


//Retorna -1 en caso de error.
uint64_t initPipes(){
    if ((semPipeManager = sem_open("Pipe Manager")) == -1)
    {
        print("Error en initPipe");
        return -1;
    }
    
    for (int i = 0; i < MAX_SEMAPHORES; i++)
    {
        pipes[i].available = TRUE;
    }
    return 0;
}

static uint64_t findAvailable(){
    for (int i = 0; i < MAX_PIPES; i++)
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
    print("Creando el pipe: ");

    int len = strlen(name);
    if (len <= 0 || len >= MAX_NAME_LENGTH)
    {
        print("Nombre del pipe es demasiado largo\n");
        return -1;
    }

    uint64_t pos;
    if ((pos = findAvailable()) != -1)    {
        pipe_t *newPipe = &pipes[pos].pipe;
        // Inicializamos la estructura
        memcpy(newPipe->name, name, len);
        newPipe->rIndex = 0;
        newPipe->wIndex = 0;

        sem_id semRead = sem_open("semRead");
        sem_id semWrite = sem_open("semWrite");
        if (semRead == -1 || semWrite == -1)        {
            print("Error en los sem del pipe");
            return -1;
        }
        newPipe->semRead = semRead;
        newPipe->semWrite = semWrite;
    }
    return pos + 1;
}

//si no existe retorna -1, si existe retorna la posición
static uint64_t findPipe(char *name){
    for (int i = 0; i < MAX_SEMAPHORES; i++){
        if (pipes[i].available == FALSE && Strcmp(name, pipes[i].pipe.name)){
            return i;
        }
    }
    return -1;
}

//retorna -1 en caso de error
uint64_t pipeOpen(char *name){
    if (sem_wait(semPipeManager)){
        print("error semWait abriendo el pipe\n");
        return -1;
    }

    int id = findPipe(name);
    if (id == -1){
        //Si no existe un pipe con ese nombre
        id = createPipe(name);
    }
    //si el create dió error, id sigue en -1
    if (id == -1){
        print("error en pipeOpen, id=-1\n");
        sem_post(semPipeManager);
        return -1;
    }
    if (sem_post(semPipeManager)){
        print("error semPost abriendo el pipe\n");
        return -1;
    }
    return id;
}

//returns TRUE if pipe exists and is being used by someone
static uint64_t validIndex(uint64_t pipeIndex){
    if(pipeIndex < 0 || pipeIndex > MAX_PIPES){
        print("invalid pipe index\n");
        return FALSE;
    } else if (pipes[pipeIndex - 1].available){
        print("pipe not being used\n");
        return FALSE;
    } else {
        //available and occupied pipe
        return TRUE;
    }
}

uint64_t pipeClose(uint64_t pipeIndex){
    if (!validIndex(pipeIndex)){
        print("invalid pipe to close\n");
        return -1;
    }
    if (sem_wait(semPipeManager) == -1){
        print("error semWait en pipeClose\n");
        return -1;
    }

    pipes[pipeIndex].available = TRUE;
    int closeRead = sem_close("semRead");
    int closeWrite = sem_close("semWrite");

    if (closeRead == -1 || closeWrite == -1){
        print("error en los sem close del pipe\n");
        return -1;
    }

    if (sem_post(semPipeManager) == -1){
        print("error semPost en pipeClose\n");
        return -1;
    }
    return 1;
}

uint64_t writeChar(uint64_t pipeIndex, char c){
    if (!validIndex(pipeIndex))
        return -1;

    pipe_t pipe = pipes[pipeIndex].pipe;

    if (sem_wait(pipe.semWrite) == -1){
        print("Error semWait en writeChar\n");
        return -1;
    }
    pipe.buffer[pipe.wIndex % PIPE_BUFFER_SIZE] = c;
    pipe.wIndex++;
    if (sem_post(pipe.semWrite) == -1){
        print("Error semPost en writeChar\n");
        return -1;
    }
    return 1;
}

uint64_t writePipe(uint64_t pipeIndex, char *string){
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

    pipe_t pipe = pipes[pipeIndex].pipe;
    if (sem_wait(pipe.semRead) == -1){
        print("Error semWait en readPipe\n");
        return -1;
    }
    char c = pipe.buffer[pipe.rIndex % PIPE_BUFFER_SIZE];
    pipe.rIndex--;
    if (sem_post(pipe.semRead) == -1){
        print("Error semPost en readPipe\n");
        return -1;
    }
    return c;
}

void pipe(){
    print("NOMBRE\t\tESTADO\t\tSEM USADO\t\t\t\tPROCESOS BLOQUEADOS\n");
    int i;
    for(i = 0; i < MAX_PIPES; i++){
        if(!(pipes[i].available)){
            printPipe(pipes[i].pipe);
        }
    }
    print("\n");
}
void printPipe(pipe_t pipe){
    print(pipe.name);
    print("\t\t\t");
    print("Activo\t\t");
    printSemsInvovled(pipe);
    print("\t");
    printProcesses(pipe);
    print("\n");
}

void printSemsInvovled(pipe_t pipe){
   // print(getSemName(pipe.semRead));
    print(", ");
    //print(getSemName(pipe.semWrite));
    print(", ");
   // print(getSemName(semPipeManager));
}
void printProcesses(pipe_t pipe){
  //  printProcessesSem(pipe.semRead);
    print(" ");
  //  printProcessesSem(pipe.semWrite);
    print(" ");
  //  printProcessesSem(semPipeManager);
}