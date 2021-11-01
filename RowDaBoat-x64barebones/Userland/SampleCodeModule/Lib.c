// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include <libasm.h>
#include "Include/Lib.h"
#include <stdint.h>

char read_input(){//syscall0
   return (char) sys_call(0,0,0,0,0,0);
}
void print(char *buffer)//syscall1
{
   int length=strlen(buffer)+1;
   sys_call(1,(uint64_t)buffer,(uint64_t)length,0,0,0);
}
void get_Memory(uint8_t* mem,uint8_t* v){//syscall2
   sys_call(2,(uint64_t) mem,(uint64_t)v,0,0,0);
}
void newLine(){//syscall3
   sys_call(3,0,0,0,0,0);
}
void get_InfoReg(uint64_t* reg_pointer){//syscall4
   sys_call(4,(uint64_t)reg_pointer,0,0,0,0);
}
void put_char(char c){//syscall5
   sys_call(5,(uint64_t)c,0,0,0,0);
}
void actionCall(int action){//syscall6
   sys_call(6,(uint64_t) action,0,0,0,0);
}
uint64_t get_RTC(uint64_t c){//syscall7
   return sys_call(7,c,0,0,0,0);
}
void print_num(int num, int base_option){//syscall8
   sys_call(8,(uint64_t)num,(uint64_t)base_option,0,0,0);
}
void clearScreen(){//syscall9
   sys_call(9,0,0,0,0,0);
}
unsigned long get_seconds(){//syscall10
   return (unsigned long) sys_call(10,0,0,0,0,0);
}
void set_cursor(int x, int y){//syscall11
   sys_call(11,(uint64_t)x,(uint64_t)y,0,0,0);
}
void set_margins(int mLeft,int mRight){//syscall12-----------desuso
   sys_call(12,(uint64_t)mLeft,(uint64_t)mRight,0,0,0);
}
void getCursor(int coords[2]){//syscall13
   sys_call(13,(uint64_t) coords,0,0,0,0);
}
void* Mmalloc(int size){
   return (void*)sys_call(14,(uint64_t)size,0,0,0,0);//syscall14
}
void Mfree(void* pointer){//syscall15
   sys_call(15,(uint64_t)pointer,0,0,0,0);
}
void* MemSet(void* ptr,uint32_t c,uint64_t string){//syscall16
   return (void*)sys_call(16,(uint64_t)ptr,(uint64_t)c,string,0,0);
}
mm_stat Mmem(){//syscall17
   mm_stat aux;
   sys_call(17,(uint64_t)&aux,0,0,0,0);
   return aux;
}

pid_t exec(main_func_t *func, char* name, int rcx, uint64_t fd[2]){ //syscall 18
   int pid;
   sys_call(18, (uint64_t)(void*)func, (uint64_t)(void*)name, (uint64_t)(void*)rcx, (uint64_t)(void *)&pid, (uint64_t)fd);
   return pid;
}

int ps(process_info* arr, unsigned int max_size) {//syscall 19
   unsigned int size;
   sys_call(19,(uint64_t)(void *) arr,(uint64_t)(void *) max_size,(uint64_t)(void *) &size,0,0);
   return size;
}

void _yield(){//syscall 20
   sys_call(20,0,0,0,0,0);
}

int kill(pid_t pid){//syscall 21
   return sys_call(21,pid,0,0,0,0);
}

int nice(pid_t pid, unsigned int priority){//syscall 22
   return sys_call(22, pid, priority, 0, 0,0);
}

int block(pid_t pid, unsigned int new_status){//syscall 23
   return sys_call(23, (uint64_t)pid, (uint64_t)new_status, 0, 0,0);
}

void s_init(){//syscall 24
   sys_call(24,0,0,0,0,0);
}

uint64_t s_open(char* name,int value){// syscall 25
   return sys_call(24,1,(uint64_t)name,(uint64_t)value,0,0);
}

uint64_t s_wait(uint64_t s_id){//syscall 26
   return sys_call(24,2,s_id,0,0,0);
}
uint64_t s_post(uint64_t s_id){
   return sys_call(24,3,s_id,0,0,0);
}
uint64_t s_close(char* semName){
   return sys_call(24,4,(uint64_t)(void *)semName,0,0,0);
}
void list_sem(){
   sys_call(24,5,0,0,0,0);
}
int getProcessStatus(int pid, unsigned int * status) {//syscall 25
   return (int) sys_call(25, (uint64_t)pid, (uint64_t)status, 0, 0,0);
}
int getPid(void) {
   int pid;
   sys_call(26, (uint64_t)&pid, 0, 0, 0,0);
   return pid;
}
void wait(unsigned int millis) {
   sys_call(27,(uint64_t) millis, 0, 0, 0,0);
}

uint64_t p_open(char* name){
   return(uint64_t)sys_call(28,(uint64_t)name,0,0,0,0);
}
uint64_t p_close(uint64_t pipeIndex){
   return(uint64_t)sys_call(29,(uint64_t)pipeIndex,0,0,0,0);
}
char p_read(uint64_t pipeIndex){
   return(char)sys_call(30,(uint64_t)pipeIndex,0,0,0,0);
}
uint64_t p_write(uint64_t pipeIndex, char* string){
   return(uint64_t)sys_call(31,(uint64_t)pipeIndex,(uint64_t)string,0,0,0);
}
uint64_t p_list(){
   return(uint64_t)sys_call(32,0,0,0,0,0);
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
      if(len > 16)
		   errorFlag = 1;
	}
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
// Function to swap two numbers
void swap(char *x, char *y) {
    char t = *x; *x = *y; *y = t;
}

// Function to reverse `buffer[i…j]`
char* reverse(char *buffer, int i, int j)
{
    while (i < j) {
        swap(&buffer[i++], &buffer[j--]);
    }

    return buffer;
}

// Iterative function to implement `itoa()` function in C
char* itoa(int value, char* buffer, int base)
{
    // invalid input
    if (base < 2 || base > 32) {
        return buffer;
    }

    // consider the absolute value of the number
    int n = abs(value);

    int i = 0;
    while (n)
    {
        int r = n % base;

        if (r >= 10) {
            buffer[i++] = 65 + (r - 10);
        }
        else {
            buffer[i++] = 48 + r;
        }

        n = n / base;
    }

    // if the number is 0
    if (i == 0) {
        buffer[i++] = '0';
    }

    // If the base is 10 and the value is negative, the resulting string
    // is preceded with a minus sign (-)
    // With any other base, value is always considered unsigned
    if (value < 0 && base == 10) {
        buffer[i++] = '-';
    }

    buffer[i] = '\0'; // null terminate string

    // reverse the string and return it
    return reverse(buffer, 0, i - 1);
}
