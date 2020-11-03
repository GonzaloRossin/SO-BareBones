#include <libasm.h>
#include <Lib.h>
#include <stdint.h>


char getInput()
{
   return read();
}

void print(char *buffer)
{
   int length=strlen(buffer);
   sys_write((uint64_t)buffer, (uint64_t)length);
}
void putChar(char c){
   char chartoadd[1];
   chartoadd[0]=c;
   print(chartoadd);
}
void newLine(){
   newline();
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

void strncpy(char *source, char *dest, int size)
{
   int i = 0;
   while (i < size && source[i] != 0)
   {
      dest[i] = source[i];
      i++;
   }
   dest[i] = 0;
}
static void reverse(char * c, int l){
    int start = 0; 
    int end = l -1; 
    while (start < end) 
    { 
        char aux = c[start];
        c[start] = c [end];
        c[end] = aux;
        start++; 
        end--; 
    } 
}
void numToChar(double num, char * c){
    int entera = num;
    double decimal = num - entera;
    int i = 0;
    if(num < 0){
        entera = entera * -1;
        decimal = decimal * -1;
    }
    if(entera == 0){
        c[i] = '0';
        i++;
    }
    for(; i < 99 && entera > 0; i++){
        int valor = entera % 10;
        c[i] = valor + '0';
        entera = entera/10;
    }
    if(num < 0){
        c[i] = '-';
        i++;
    }
    reverse(c,i);
    if(decimal != 0){
        c[i] = '.';
      	i++;
        for(int j = 0;i < 100 && j < 4; i++, j++){
            int aux = decimal * 10;
            c[i] = (aux % 10) + '0';
            decimal = decimal * 10;
        }
    }
    c[i] = 0;
}

