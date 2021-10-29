#include "../include/lib.h"
#include "../sem/sem.h"

#define BUFFER_SIZE 1024
#define MAX_LEN 20
#define MAX_PIPES 10

typedef struct
{
    uint64_t rIndex;
    uint64_t wIndex;
    sem_id semRead;  // id del sem para leer en este pipe
    sem_id semWrite; // id del sem para escribir en este pipe
    char buffer[BUFFER_SIZE];
    char name[MAX_LEN];
    int amountProcesses; // cantidad de procesos que estan usando el pipe
} pipe_t;

typedef struct
{
    pipe_t pipe;
    int available;
} space;


uint64_t initPipes();
uint64_t pipeOpen(char *name);
uint64_t pipeClose(uint64_t pipeIndex);
uint64_t writePipe(uint64_t pipeIndex, char *string);
char readPipe(uint64_t pipeIndex);
void pipe();

static uint64_t validIndex(uint64_t pipeIndex);
static uint64_t createPipe(char *name);
static uint64_t findPipe(char *name);
static uint64_t findAvailable();
uint64_t writeChar(uint64_t pipeIndex, char c);

void pipe();
void printPipe(pipe_t pipe);
void printSemsInvovled(pipe_t pipe);
void printProcesses(pipe_t pipe);