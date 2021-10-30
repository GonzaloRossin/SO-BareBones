#include <stdint.h>
#include <keyboard_driver.h>
#include <int80.h>
#include <lib.h>
#include "../include/process.h"

static int waiting_pid;
static int processWaiting = 0;

static int waiting_pid;
static int processWaiting = 0;


//Chose 100 as maximum size
#define BUFFER_SIZE 100
#define KEYS 59
#define END_OF_PRESSING_KEYS 0x80

#define toLowerCase(c) (c >= 'A' && c<= 'Z'? c +'a' - 'A' : c)
#define toUpperCase(c) (c >= 'a' && c<= 'z'? c -'a' + 'A' : c)

//Buffer for all the codes
static uint8_t buffer[BUFFER_SIZE] = {0};

static int startPosition = 0; //To know where in the buffer to start reading from
static int endPosition = 0;   //End of the buffer(las position is endPosition-1)
static int size = 0;

//Short cuts for special keys
#define CTRL		0x1D
#define LSHIFT		0x2A
#define RSHIFT		0x36
#define CAPS		0x3A
#define CTRL_UP		0x9D
#define LSHIFT_UP	0xAA
#define RSHIFT_UP	0xB6

//Function to handle shift pressed.
static int LSHIFT_LCKD = 0;
static int RSHIFT_LCKD = 0;
static int CAPS_LCKD = 0;
static int CTRL_LCKD = 0;

void addToBuffer(char charToAdd);

static const unsigned char asciiMap[KEYS][2] =
    {{0, 0}, {0x1B, 0}, {'1', '!'}, {'2', '@'}, {'3', '#'}, {'4', '$'},//0X1B is esc
    {'5', '%'}, {'6','~'}, {'7', '&'}, {'8', '*'}, {'9', '('},
    {'0', ')'}, {'-', '_'}, {'=', '+'}, {0x08, 0}, {0x09, 0}, {'q', 'Q'}, //0x08 is backspace, 0x09 is tab key
    {'w', 'W'}, {'e', 'E'}, {'r', 'R'}, {'t', 'T'}, {'y', 'Y'},
    {'u', 'U'}, {'i', 'I'}, {'o', 'O'}, {'p', 'P'}, {'[', '{'},
    {']', '}'}, {0x0A, 0}, {0, 0}, {'a', 'A'}, {'s', 'S'}, // 0x0A is enter
    {'d', 'D'}, {'f', 'F'}, {'g', 'G'}, {'h', 'H'}, {'j', 'J'},
    {'k', 'K'}, {'l', 'L'}, {';', ':'}, {'\'', '"'}, {'`', '~'},
    {0, 0}, {'\\', '|'}, {'z', 'Z'}, {'x', 'X'}, {'c', 'C'},
    {'v', 'V'}, {'b', 'B'}, {'n', 'N'}, {'m', 'M'}, {',', '<'},
    {'.', '>'}, {'/', '?'}, {0, 0}, {0, 0}, {0, 0}, {' ', 0}, {0, 0}};

//Add an element to the buffer.
void keyboard_handler(uint8_t code)
{

	char charToAdd = 0;

	//If the buffer is already full
	if (size >= 100)
	{
		sys_write("ERROR: buffer overflow",22);
	}

	if (code < END_OF_PRESSING_KEYS)
	{ //If we are pressing the keyboard(not releasing)
		//Cases where we are pressing an special key
		switch (code)
		{
		case CTRL:
			CTRL_LCKD = 1;
			break;
		case CAPS:
			CAPS_LCKD = !CAPS_LCKD;
			break;
		case LSHIFT:
			LSHIFT_LCKD = 1;
			break;
		case RSHIFT:
			RSHIFT_LCKD = 1;
			break;
		}

		//Add the letter based on the state of our special letters
		if (LSHIFT_LCKD || RSHIFT_LCKD)
		{
			charToAdd = asciiMap[code][1];
			if (charToAdd != 0)
			{ //If it is a valid letter.
				if (CAPS_LCKD)
				{
					//Case when the shift is pressed and we are in CAPS as well.
					addToBuffer(toLowerCase(charToAdd));
				}
				else
				{
					addToBuffer(charToAdd);
				}
			}
		}
		else if (CAPS_LCKD)
		{
			charToAdd =asciiMap[code][0];
			if (charToAdd != 0)
			{
				addToBuffer(toUpperCase(charToAdd));
			}
		}
		else if(CTRL_LCKD){
			saveRegs();//Special key CTRL saves registers for Inforeg
		}
		//Standard case, just adding the letter.
		else
		{
			charToAdd = asciiMap[code][0];
			if (charToAdd != 0)
			{
				addToBuffer(charToAdd);
			}
		}
	}
	//This means a key was released, we only care if the key is Shift or Ctrl
	else
	{
		switch (code)
		{
		case CTRL_UP:
			CTRL_LCKD = 0;
			break;
		case LSHIFT_UP:
			LSHIFT_LCKD = 0;
			break;
		case RSHIFT_UP:
			RSHIFT_LCKD = 0;
			break;
		}
	}
}

//Adds to buffer the current letter.
void addToBuffer(char charToAdd)
{
	buffer[endPosition] = charToAdd;
	endPosition = (endPosition + 1) % BUFFER_SIZE; //As its cyclic iterator
	size++;
	if (processWaiting) {
		processWaiting = 0;
		changeStatus(waiting_pid, READY);
    }
}

//Function to return a uint8 from the buffer and delete it. Return 0 if empty
uint8_t getChar()
{
	uint64_t fd = getFdIn();
	if(fd > 0){
		//printf("trying to get char from pipe");
		return (uint8_t)readPipe(fd);
	} else {
		if (size <= 0)
		{ //Nada en el buffer
			int pid;
			getPid(&pid);
			if (isCurrentForeground()) {
				processWaiting = 1;
				waiting_pid = pid;
			}
			changeStatus(pid, BLOCKED);
			return 0;
		}
		else
		{
			uint8_t to_return = buffer[startPosition];
			startPosition = (startPosition + 1) % BUFFER_SIZE;
			size--;
			return to_return;
		}
	}
}

//Return the last input read from the keyboard and consumes it. Returns 0 if buffer empty
uint8_t getLastInput()
{
	if (size <= 0)
	{//EMPTY BUFFER
		return 0;
	}
	else if(endPosition == 0)
	{
		endPosition = BUFFER_SIZE-1;
		size--;
		return buffer[endPosition];
	}
	else
	{
		endPosition--;
		size--;
		return buffer[endPosition];
	}
}
