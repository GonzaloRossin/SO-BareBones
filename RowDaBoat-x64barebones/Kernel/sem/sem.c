#include "sem.h"

typedef struct
{
    sem_t sem;
    uint64_t available;
} sem_space;
static sem_space semSpaces[MAX_SEM];

// Funciones internas
static int createSem(char *name, uint64_t initValue);
static uint64_t findAvailableSpace();
static uint64_t lockSem; // Para bloquear al momento de un open o close de cualquier sem.
static uint64_t findSem(char *name);
static uint64_t enqeueProcess(uint64_t pid, sem_t *sem);
static uint64_t dequeueProcess(sem_t *sem);
void printSem(sem_t sem);
void printProcessesBlocked(processNode *process);

// Se inicializa el vector para que todos los lugares esten disponibles
void initSems()
{
    for (int i = 0; i < MAX_SEM; i++)
    {
        semSpaces[i].available = TRUE;
    }
}

static uint64_t findAvailableSpace()
{
    for (int i = 0; i < MAX_SEM; i++)
    {
        if (semSpaces[i].available == TRUE)
        {
            semSpaces[i].available = FALSE;
            return i;
        }
    }
    return -1; // No hay mas espacio en el vector para crear otro semaforo
}

// Retorna la posicion dentro de la estructura donde esta guardado
static int createSem(char *name, uint64_t initValue)
{
    int pos;
    if ((pos = findAvailableSpace()) != -1)
    {
        // Inicializamos la estructura
        memcpy(semSpaces[pos].sem.name, name, Strlen(name)+1);
        semSpaces[pos].sem.value = initValue;
        semSpaces[pos].sem.lock = 0; // Inicialmente no esta lockeado.
        semSpaces[pos].sem.firstProcess = NULL;
        semSpaces[pos].sem.lastProcess = semSpaces[pos].sem.firstProcess;
        semSpaces[pos].sem.size = 0;
        semSpaces[pos].sem.sizeList = 0;
    }
    return pos;
}

// Retorna un puntero al semaforo al igual que en POSIX. En caso de error retorna NULL
uint64_t semOpen(char *name, uint64_t initValue)
{
    while (_xchg(&lockSem, 1) != 0); // esperando a que el lock este disponible
    // Primero me fijo si ya existe el sem por nombre
    // Si no existe debo crear el sem por primera vez
    int semIndex = findSem(name);
    if (semIndex == -1) // Si no existe el sem, hay que crearlo
    {
        semIndex = createSem(name, initValue);
        if (semIndex == -1)
        {
            _xchg(&lockSem, 0);
            return -1; // No habia mas lugar para crear sem.
        }
    }
    semSpaces[semIndex].sem.size++;
    _xchg(&lockSem, 0);
    return semIndex; // Retornamos el indice del sem.
}

// Retorna -1 en caso de error
uint64_t semClose(char *name)
{
    while (_xchg(&lockSem, 1) != 0)
        ;
    int semIndex = findSem(name);
    if (semIndex == -1)
    {
        return -1; // No se encontro el semaforo pedido.
    }
    if ((--semSpaces[semIndex].sem.size) <= 0)
        semSpaces[semIndex].available = TRUE;
    _xchg(&lockSem, 0);
    return 1;
}

// Retorna 0 en caso de exito y -1 si fracasa. Blockea el sem.
uint64_t semWait(uint64_t semIndex)
{
    if (semIndex >= MAX_SEM)
        return -1;
    sem_t *sem = &semSpaces[semIndex].sem;

    while (_xchg(&sem->lock, 1) != 0)
        ;

    if (sem->value > 0)
    {
        sem->value--;
        _xchg(&sem->lock, 0);
    }
    else
    {
        // Si el valor es 0 entonces debo poner al proceso a dormir (encolarlo)
        int pid;
        getPid(&pid);
        if (enqeueProcess(pid, sem) == -1)
        {
            _xchg(&sem->lock, 0);
            return -1;
        }

        _xchg(&sem->lock, 0);
        if (changeStatus(pid,BLOCKED) == -1)
        {
            return -1;
        }
        sem->value--;
    }
    return 0;
}

uint64_t semPost(uint64_t semIndex)
{
    if (semIndex >= MAX_SEM)
    {
        return -1;
    }

    sem_t *sem = &semSpaces[semIndex].sem;
    while (_xchg(&sem->lock, 1) != 0)
        ;
    sem->value++;
    int pid = 0;
    if (sem->sizeList > 0)
    {
        if ((pid = dequeueProcess(sem)) == -1)
        {
            _xchg(&sem->lock, 0);
            return -1;
        }
    }
    _xchg(&sem->lock, 0);
    changeStatus(pid,READY) ?: forceTimer();
    return 0;
}

// Retorna -1 en caso de no encontrar el sem
static uint64_t findSem(char *name)
{
    for (int i = 0; i < MAX_SEM; i++)
    {
        if (semSpaces[i].available == FALSE && Strcmp(name, semSpaces[i].sem.name))
        {
            return i;
        }
    }
    return -1;
}

// Agrega un proceso a la lista, en caso de fallar retorna -1
uint64_t enqeueProcess(uint64_t pid, sem_t *sem)
{
    processNode *process = RTOSMalloc(sizeof(processNode));
    if (process == NULL)
    {
        return -1;
    }
    process->pid = pid;
    if (sem->sizeList == 0)
    {
        sem->firstProcess = process;
        sem->lastProcess = sem->firstProcess;
        process->next = NULL;
    }
    else
    {
        sem->lastProcess->next = process;
        process->next = NULL;
        sem->lastProcess = process;
    }
    sem->sizeList++;
    return 0;
}

uint64_t dequeueProcess(sem_t *sem)
{
    if (sem == NULL || sem->firstProcess == NULL)
        return -1;
    processNode *current = sem->firstProcess;
    int pid = current->pid;
    sem->firstProcess = current->next;
    if (sem->firstProcess == NULL)
    {
        sem->lastProcess = NULL;
    }
    RTOSFree(current);
    sem->sizeList--;
    return pid;
}

void listSem()
{
    printf("SEM'S NAME\t\tSTATE\t\tBLOCKED PROCESSES\n");
    for (int i = 0; i < MAX_SEM; i++)
    {
        int toPrint = !(semSpaces[i].available);
        if (toPrint)
        {
            printSem(semSpaces[i].sem);
        }
    }
}

void printSem(sem_t sem)
{
    printf(sem.name);
    if (Strlen(sem.name) > 10)
        printf("\t\t");
    else
        printf("\t\t\t\t");
    draw_decimal(sem.value);
    printf("\t\t\t");
    printProcessesBlocked(sem.firstProcess);
    printf("\n");
}

void printProcessesBlocked(processNode *process)
{
    while (process != NULL)
    {
        draw_decimal(process->pid);
        printf(" ");
        process = process->next;
    }
    printf("-");
}

char *getSemName(uint64_t semIndex)
{
    if (semIndex >= MAX_SEM)
    {
        printf("Wrong Index in getSemName\n");
        return NULL;
    }
    return semSpaces[semIndex].sem.name;
}

void printProcessesSem(uint64_t semIndex)
{
    if (semIndex >= MAX_SEM)
    {
        printf("Wrong Index in printProcessesSem\n");
        return;
    }
    sem_t sem = semSpaces[semIndex].sem;
    printProcessesBlocked(sem.firstProcess);
}
