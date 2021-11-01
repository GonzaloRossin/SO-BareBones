// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include <libasm.h>
#include <Lib.h>
#include <shell.h>
#define BUFFER_SIZE 100
#define MAX_COMDESC 100
#define MAX_COMMANDS 20



int main() {
    nice(getPid(), 0);
    shell();    
    return 0;
}
