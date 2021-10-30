#include "phylo.h"

#define MAX_PHYL 15
#define MIN_PHYL 2
#define INIT_PHYL 5
#define FOREGROUND 1
#define BACKGROUND 0
#define SEM_PHYL "semPhylos"
#define LEN 20
#define QUANTUM 1000//en milis

typedef struct
{
    uint64_t pid;
    int state;
    uint64_t semIndex;
    char semName[LEN];
} phylo_t;

enum state
{
    THINKING,
    EATING,
    WAITING
};

static phylo_t phylos[MAX_PHYL];
static int seated;
uint64_t sem;

static int addPhylo(int pIndex);
static int removePhylo(int pIndex);
void endTable();
void phyloProcess(int argc, char **argv);
static void printState();
void putChopstick();
void takeChopstick(int pIndex);
void update(int pIndex);
int left(int pIndex);
int right(int pIndex);

/**
 * Ejecuta el dilema de los filosofos.
 */
void phylo(int argc, char **argv)
{
    print("WELCOME TO PHYLO!\n");
    if ((sem = s_open(SEM_PHYL, 1)) == -1)
    {
        print("Error opening main semaphore in phylo.\n");
        return;
    }
    seated = 0;
    for (int i = 0; i < INIT_PHYL; i++)
    {
        if (addPhylo(i) == -1)
        {
            print("Error adding initial philosophers.\n");
        }
    }
    print("Press 'a' to add or 'r' to remove a philosopher. Press 'q' to exit.\n");
    char c;
    while ((c = read_input()) != '\n')
    {
        switch (c)
        {
            case 'a':
            case 'A':
                if (addPhylo(seated) == -1){
                    print("Error adding philosopher.\n");
                }
            break;
            case 'r':
            case 'R':
            if (removePhylo(seated - 1) == -1)
            {
                print("Error removing philosopher.\n");
            }
            break;
        }
    }
    endTable();
    if (s_close(sem) == -1)
        print("Error closing main semaphore in Phylo.\n");
}

/**
 * Se suma un filosofo a la mesa, prepara sus cosas para comer.
 * Retorna -1 en caso de error.
 */
int addPhylo(int pIndex){
    if (pIndex > MAX_PHYL || pIndex < 0)
    {
        return -1;
    }
    s_wait(sem);
    seated++;
    char semName[LEN] = " phyl";
    itoa(pIndex, phylos[pIndex].semName,10);
    strcat(phylos[pIndex].semName, semName);
    if ((phylos[pIndex].semIndex = s_open(phylos[pIndex].semName, 1)) == -1)
    {
        print("Error opening sem in addPhylo.\n");
        return -1;
    }
    char currSeated[LEN];
    itoa(seated,currSeated,10);
    //uintToBase(seated, currSeated, 10);
    char *argv[] = {"phi", currSeated};
    phylos[pIndex].state = THINKING;
    main_func_t aux = {phyloProcess, 1, argv}; 
    if ((phylos[pIndex].pid = exec(&aux,phylos[pIndex].semName , BACKGROUND)) < 0) //TODO Cambiar al implementar pipes
    {
        print("Error creating philosopher process");
        return -1;
    }
    s_post(sem);
    return 0;
}

/**
 * Proceso que hace que los filosofos lleguen a la mesa,
 * busquen sus chopsticks y se sienten, si no pueden,
 * deben esperar a su turno.
 */
void phyloProcess(int argc, char **argv)
{
    int index = strToInt(argv[1]);
    while (1)
    {
        wait(QUANTUM);
        // Busca chopsticks ya que tiene hambre.
        takeChopstick(index);
        wait(QUANTUM);
        // Termino de comer, deja sus chopsticks.
        putChopstick(index);
    }
}

/**
 * El filosofo desea comer, toma unos palillos y espera su turno. 
 */
void takeChopstick(int pIndex)
{
    s_wait(sem);
    phylos[pIndex].state = WAITING;
    update(pIndex);
    s_post(sem);
    s_wait(phylos[pIndex].semIndex);
}

/**
 *  El filosofo piensa y se ve junto a sus vecinos, quienes se preguntan si pueden comer.
 */
void putChopstick(int pIndex)
{
    s_wait(sem);
    phylos[pIndex].state = THINKING;
    update(left(pIndex));
    update(right(pIndex));
    s_post(sem);
}

// Devuelve el vecino izquierdo del indice.
int left(int pIndex)
{
    return (pIndex == 0) ? seated - 1 : pIndex - 1;
}

// Devuelve el vecino derecha del indice.
int right(int pIndex)
{
    return (pIndex == seated - 1) ? 0 : pIndex + 1;
}

// Actualiza la mesa con los filosofos que desean y se encuentran comiendo.
void update(int pIndex)
{
    if (phylos[pIndex].state == WAITING && phylos[left(pIndex)].state != EATING && phylos[right(pIndex)].state != EATING)
    {
        phylos[pIndex].state = EATING;
        printState();
        s_post(phylos[pIndex].semIndex);
    }
}

/**
 * El filosofo se retira de la mesa, no quiere comer más entonces
 * su lugar es liberado y se reordena el resto.
 */
static int removePhylo(int pIndex)
{
    if (pIndex > MAX_PHYL || pIndex < MIN_PHYL)
        return -1;
    s_wait(sem);
    seated--;
    int eats = (phylos[pIndex].state == EATING);
    if (s_close(phylos[pIndex].semIndex) == -1)
        print("Error in closing phylo's sem.\n");
    kill(phylos[pIndex].pid);
    if (eats && pIndex)
    {
        update(pIndex - 1);
        update(0);
    }
    s_post(sem);
    return 0;
}

/**
 * Se cierra la mesa, todo filosofo es echado.
 */
void endTable()
{
    while (seated > 0)
    {
        if (s_close(phylos[seated - 1].semIndex) == -1)
        {
            print("Error in closing phylo's sem while ending.\n");
            return;
        }
        kill(phylos[seated - 1].pid);
        seated--;
    }
}

static void printState()
{
    for (int i = 0; i < seated; i++)
    {
        (phylos[i].state == EATING) ? print(" E ") : print(" . ");
    }
    print("\n");
}