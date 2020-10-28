#ifndef __INTERRUPTACTIONS80__
#define __INTERRUPTACTIONS80__
#include <stdint.h>

// the int 80h dispatcher
uint64_t int80Dispatcher(uint64_t callNumber, uint64_t arg1, uint64_t arg2, uint64_t arg3);
void sys_exit(int error);				                        //SYS_CALL 0
void sys_read(char * buffer, int count);				        //SYS_CALL 1
void sys_write(char * buffer, int count);						//SYS_CALL 2
void sys_put_char(char c);										//SYS_CALL 3
void sys_get_char(char c);									    //SYS_CALL 4
void sys_write_number(int number, int option);                  //SYS_CALL 5
void sys_print_records();                                       //SYS_CALL 6
uint64_t sys_get_ticks();										//SYS_CALL 7
uint8_t sys_get_clock_info(int option);                         //SYS_CALL 8
#endif