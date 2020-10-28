#include <int80.h>
#include <stdint.h>
#include <naiveConsole.h>
#include <video_driver.h>

//Software interrupt used for interaction between user and kernel space
//order of registers in standard rdi -> call number,rsi -> arg1 ,rdx -> arg2 ,rcx -> arg3
uint64_t interruptAction80Dispatcher(uint64_t callNumber, uint64_t arg1, uint64_t arg2, uint64_t arg3)
{

	switch (callNumber)
	{
    //sys_exit. Exits with error number.
	case 0:
		sys_exit((int)arg1);
		break;
	//sys_read. Just reads from the keyboard and copies to the buffer.
	case 1:
		sys_read((char *)arg1, (int)arg2);
		break;
	// sys_write: writes stdout the content of buffer.
	case 2:
		sys_write((char *)arg1, (int)arg2);
		break;
	//sys_put_char: prints a char in the screen.
	case 3:
		sys_put_char((char)arg1);
		break;
    //sys_get_char: gets a char.
	case 4:
		sys_get_char((char)arg1);
		break;
    //sys_write_number: prints an int.
	case 5:
		sys_write_number((int)arg1,(int) arg2);
		break;
    //sys_print_records: prints all records.
	case 6:
		return (uint64_t) sys_print_records();
		break;
	//sys_get_ticks
	case 7:
		return (uint64_t) sys_get_ticks();
		break;
	//sys_get_clock_info: gets the the seconds, minutes or hours.
	case 8:
		return (uint64_t) sys_get_clock_info((int)arg1);
		break;

	return 0;
}

//SYS_CALL 0
void sys_exit(int error){
    return error;
}

//SYS_CALL 1
void sys_read(char *buffer, int count)
{

	int i = 0;
	char letter;
	while (i < count)
	{
		letter = getChar();
		if (letter != 0)
		{
			*(buffer + i) = letter;
			i++;
		}
		else
		{
			//If there is nothing on the buffer we stop until we get an interruption.
			//haltFunction();
		}
	}
}

//SYS_CALL 2
void sys_write(char *buffer, int count)
{
	ncPrint(buffer);
}

//SYS_CALL 3
void sys_put_char(char c)
{
	if (c != 0)
		ncPrintChar(c);
}

//SYS_CALL 4
char sys_get_char()
{
	//return getLastInput();
}

//SYS_CALL 5
void sys_write_number(int number, int option){
	switch(option){
		case 0:
			ncPrintDec(number);
			break;
		case 1:
			ncPrintHex(number);
			break;

	}
}

//SYS_CALL 6
void sys_print_records(
	
}

//SYS_CALL 7
uint64_t sys_get_ticks()
{
	return getTicks();
}

//SYS_CALL 8
uint8_t sys_get_clock_info(int option){
	switch(option){
		case 0:
			return readSeconds();
		case 1:
			return readMinutes();
		case 2:
			return readHours();	
	}

	return 0;

}