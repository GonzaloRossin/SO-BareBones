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

