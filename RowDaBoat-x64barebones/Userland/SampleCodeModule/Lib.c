#include <libasm.h>
#include "Include/Lib.h"
#include <stdint.h>

char read_input(){//syscall0
   return (char) sys_call(0,0,0,0,0);
}
void print(char *buffer)//syscall1
{
   int length=strlen(buffer);
   sys_call(1,(uint64_t)buffer,(uint64_t)length,0,0);
}
void get_Memory(uint8_t* mem,uint8_t* v){//syscall2
   sys_call(2,(uint64_t) mem,(uint64_t)v,0,0);
}
void newLine(){//syscall3
   sys_call(3,0,0,0,0);
}
void get_InfoReg(uint64_t* reg_pointer){//syscall4
   sys_call(4,(uint64_t)reg_pointer,0,0,0);
}
void put_char(char c){//syscall5
   sys_call(5,(uint64_t)c,0,0,0);
}
void actionCall(int action){//syscall6
   sys_call(6,(uint64_t) action,0,0,0);
}
uint64_t get_RTC(uint64_t c){//syscall7
   return sys_call(7,c,0,0,0);
}
void print_num(int num, int base_option){//syscall8
   sys_call(8,(uint64_t)num,(uint64_t)base_option,0,0);
}
void clearScreen(){//syscall9
   sys_call(9,0,0,0,0);
}
unsigned long get_seconds(){//syscall10
   return (unsigned long) sys_call(10,0,0,0,0);
}
void set_cursor(int x, int y){//syscall11
   sys_call(11,(uint64_t)x,(uint64_t)y,0,0);
}
void set_margins(int mLeft,int mRight){//syscall12-----------desuso
   sys_call(12,(uint64_t)mLeft,(uint64_t)mRight,0,0);
}
void getCursor(int coords[2]){//syscall13
   sys_call(13,(uint64_t) coords,0,0,0);
}
void* Mmalloc(int size){
   return (void*)sys_call(14,(uint64_t)size,0,0,0);//syscall14
}
void Mfree(void* pointer){//syscall15
   sys_call(15,(uint64_t)pointer,0,0,0);
}
void* MemSet(void* ptr,uint32_t c,uint64_t string){//syscall16
   return (void*)sys_call(16,(uint64_t)ptr,(uint64_t)c,string,0);
}
mm_stat Mmem(){//syscall17
   mm_stat aux;
   sys_call(17,(uint64_t)&aux,0,0,0);
   return aux;
}

pid_t exec(main_func_t *func, char* name, int rcx){ //syscall 18
int pid;
   sys_call(18, (uint64_t)(void*)func, (uint64_t)(void*)name, (uint64_t)(void*)rcx, (uint64_t)(void *)&pid);
   return pid;
}

int ps(process_info* arr, unsigned int max_size) {//syscall 19
   unsigned int size;
   sys_call(19,(uint64_t)(void *) arr,(uint64_t)(void *) max_size,(uint64_t)(void *) &size,0);
   return size;
}

//loop = 20

void kill(pid_t pid){//syscall 21
   sys_call(21,pid,0,0,0);
}

void nice(pid_t pid, unsigned int priority){//syscall 22
   sys_call(22,pid,priority,0,0);
}

void block(pid_t pid, unsigned int new_status){//syscall 23
   sys_call(23, (uint64_t)pid, (uint64_t)new_status, 0, 0);
}

sem_id s_init(char* name,unsigned int init_size){//syscall 24
   return(sem_id)sys_call(24,0,(uint64_t)name,(uint64_t)init_size,0);
}

sem_id s_open(char* name){// syscall 25
   return(sem_id)sys_call(24,1,(uint64_t)name,0,0);
}

int s_wait(sem_id s_id){//syscall 26
   return(int) sys_call(24,2,(uint64_t)s_id,0,0);
}
int s_post(sem_id s_id){
   return(int) sys_call(24,3,(uint64_t)s_id,0,0);
}
int s_close(sem_id s_id){
   return(int) sys_call(24,4,(uint64_t)s_id,0,0);
}
int s_getValue(sem_id s_id,int* value_pointer){
   return(int) sys_call(24,5,(uint64_t)s_id,(uint64_t) value_pointer,0);
}

int getProcessStatus(int pid, unsigned int * status) {//syscall 25
   return (int) sys_call(25, (void *)(uint64_t)pid, (void *)status, 0, 0);
}
int getPid(void) {
   int pid;
   sys_call(26, (void* )&pid, 0, 0, 0);
   return pid;
}
void wait(unsigned int millis) {
   sys_call(27,(void *)(uint64_t) millis, 0, 0, 0);
}

uint64_t pipeOpen(char* name){
   return(uint64_t)sys_call(28,(uint64_t)name,0,0,0);
}
uint64_t pipeClose(uint64_t pipeIndex){
   return(uint64_t)sys_call(29,(uint64_t)pipeIndex,0,0,0);
}
uint64_t readPipe(uint64_t pipeIndex){
   return(uint64_t)sys_call(30,(uint64_t)pipeIndex,0,0,0);
}
uint64_t writePipe(uint64_t pipeIndex, char* string){
   return(uint64_t)sys_call(31,(uint64_t)pipeIndex,(uint64_t)string,0,0);
}
//----------------------------------------------------------------------------------------------
void save_screenCords(screenShell* shell){//desuso
   int aux[2]={0};
   getCursor(aux);
   shell->coordX=aux[0];
   shell->coordY=aux[1];
}

int strlen(char *str)
{
   int i = 0;
   while (str[i] != 0)
   {
      i++;
   }
   return i;
}

int strcmp(const char *str1, const char *str2)
{

   int i = 0;
   while (str1[i] != 0 && str2[i] != 0)
   {
      if (str1[i] != str2[i])
         return 0;
      i++;
   }
   //If one of them still has characters.
   if (str1[i] != str2[i])
      return 0;

   return 1;
}

void strncpy(char *source, char *dest, int index, int size)
{
   int i = index;
   int j=0;
   while (i < size && source[i] != 0)
   {
      dest[j++] = source[i];
      i++;
   }
   dest[j] = 0;
}
uint8_t* strToNumHex(char * str){
   int errorFlag = 0;
	uint64_t number = 0;
	unsigned int len = 0;
	while(*str != 0 && *str != ' ' && !errorFlag) {
		number *= 16; //Multiplico por 16 para correr todo un lugar a la izquierda
		if(*str >= '0' && *str <= '9')
			number += (*str - '0');
		else if(*str >= 'A' && *str <= 'F')
			number += (*str - 'A');
		else
			number += (*str - 'a');
		str++;
		len++;
	}
	if(len > 16)
		errorFlag = 1;
	return (uint8_t *)number;
}
int strToInt(char* str)
{
   // Initialize result
    int res = 0;

    // Initialize sign as positive
    int sign = 1;

    // Initialize index of first digit
    int i = 0;

    // If number is negative,
    // then update sign
    if (str[0] == '-') {
        sign = -1;

        // Also update index of first digit
        i++;
    }

    // Iterate through all digits
    // and update the result
    for (; str[i] != '\0'; ++i)
        res = res * 10 + str[i] - '0';

    // Return result with sign
    return sign * res;
}
int abs(int i){
    return i >= 0 ? i : -i;
}
