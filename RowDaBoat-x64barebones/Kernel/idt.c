#include <defs.h>
#include <naiveConsole.h>
#include <idt.h>
#include <lib.h>
#include <interrupt_routines.h>

// TODO: add all type_attr constants here
#define TATTR_INT_32 0x0E
#define TATTR_PRE 0x80
#define TATTR_INTERRRUPT_32 (TATTR_INT_32 | TATTR_PRE)
void irqDispatcher(int n)
{
	switch (n)
	{
	case 0:
		interruptRoutine1();
		break;
	case 1:
		interruptRoutine2();
	}
}

#pragma pack(push)
#pragma pack(1)
//the struct of a entry in the interrupt descriptor table
typedef struct idtEntry
{
	// offset is "the position of the function to execute"
	uint16_t offset_1; // offset bits 0..15. the 16 least significant bytes
	uint16_t selector; // a code segment selector in GDT or LDT
	// bits 0..2 holds Interrupt Stack Table offset, rest of bits zero.
	// in our case they are all set to zero
	uint8_t ist;
	// type and attributes. https://wiki.osdev.org/Interrupt_Descriptor_Table
	// describes what this byte does. |p|dpl|dpl|z|gateType|gateType|gateType|gateType|
	uint8_t type_attr;
	uint16_t offset_2; // offset bits 16..31 continues offset
	uint32_t offset_3; // offset bits 32..63 continues offset
	uint32_t zero;	 // reserved. should be all zeros
} idtEntry;
#pragma pack(pop)

// bootloader sets the idt to start at 0 and have 255 entries
static idtEntry *table = (idtEntry *)0;
static void setupEntry(int index, uint64_t offset)
{

	table[index].offset_1 = offset & 0xffff;
	table[index].offset_2 = (offset >> 16) & 0xffff;
	table[index].offset_3 = (offset >> 32) & 0xffffffff;
	table[index].selector = 0x08;
	table[index].ist = 0x0;
	// TODO: add support so that interrupts can have different privilege levels
	table[index].type_attr = TATTR_INTERRRUPT_32;
	table[index].zero = 0x0;
}
void loadIDT()
{
	//disable interrupts
	_cli();
	setupEntry(0x20, (uint64_t) &irq0Handler);  //Interrupt del timertick
	setupEntry(0x21,(uint64_t) &irq1Handler); // Interrupt del teclado
	setupEntry(0x80, (uint64_t)&int80); //Int 80
	// all bits in one disable that irq
	// example 00000001 disables irq0
	// http://stanislavs.org/helppc/int_table.html for more info
	picMasterMask(0xFC);
	// here we disable all slave interrupts
	picSlaveMask(0x00);
	// reenable interrupts
	_sti();
}
