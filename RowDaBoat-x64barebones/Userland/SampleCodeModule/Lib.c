#include <libasm.h>
#include <Lib.h>
#include <stdint.h>



void print(char *buffer)
{
   int length=strlen(buffer);
   sys_write((uint64_t)buffer, (uint64_t)length);
}
void putChar(char c){
   put_char(c);
}
void newLine(){
   newline();
}
void actionCall(int action){
    putActioncall(action);
}
void draw(int option, matrix_struct * m){
    sys_draw(option, (uint64_t) m);
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
