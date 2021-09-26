#include <libasm.h>
#include <Lib.h>
#include <stdint.h>

char read_input(){//syscall0
   return (char) sys_call(0,0,0,0);
}
void print(char *buffer)//syscall1
{
   int length=strlen(buffer);
   sys_call(1,(uint64_t)buffer,(uint64_t)length,0);
}
void get_Memory(uint8_t* mem,uint8_t* v){//syscall2
   sys_call(2,(uint64_t) mem,(uint64_t)v,0);
}
void newLine(){//syscall3
   sys_call(3,0,0,0);
}
void get_InfoReg(uint64_t* reg_pointer){//syscall4
   sys_call(4,(uint64_t)reg_pointer,0,0);
}
void put_char(char c){//syscall5
   sys_call(5,(uint64_t)c,0,0);
}
void actionCall(int action){//syscall6
   sys_call(6,(uint64_t) action,0,0);
}
uint64_t get_RTC(uint64_t c){//syscall7
   return sys_call(7,c,0,0);
}
void print_num(int num, int base_option){//syscall8
   sys_call(8,(uint64_t)num,(uint64_t)base_option,0);
}
void clearScreen(){//syscall9
   sys_call(9,0,0,0);
}
unsigned long get_seconds(){//syscall10
   return (unsigned long) sys_call(10,0,0,0);
}
void set_cursor(int x, int y){//syscall11
   sys_call(11,(uint64_t)x,(uint64_t)y,0);
}
void set_margins(int mLeft,int mRight){//syscall12-----------desuso
   sys_call(12,(uint64_t)mLeft,(uint64_t)mRight,0);
}
void getCursor(int coords[2]){//syscall13
   sys_call(13,(uint64_t) coords,0,0);
}
void save_screenCords(screenShell* shell){//desuso
   int aux[2]={0};
   getCursor(aux);
   shell->coordX=aux[0];
   shell->coordY=aux[1];
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
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
int abs(int i){
    return i >= 0 ? i : -i;
}