include Makefile.inc

#MEM_MAN_H o BUDDY_H
MM=-D BUDDY_H
#MM:={ifneq (,$(findstring i, $(MAKEFLAGS))) BUDDY_H else MEM_MAN_H endif}


KERNEL=kernel.bin
SOURCES=$(wildcard *.c)
SOURCES_ASM=$(wildcard asm/*.asm)
SOURCES_INTERRUPTS=$(wildcard interrupts/*.c)
SOURCES_INTERRUPTS_ASM=$(wildcard interrupts/*.asm)
SOURCES_DRIVERS=$(wildcard drivers/*.c)

SOURCES_MEM_MANAGER=$(wildcard memory/*.c)
OBJECTS_MEM_MANAGER=$(SOURCES_MEM_MANAGER:.c=.o)

OBJECTS_SEM=$(SOURCES_SEM:.c=.o)
SOURCES_SEM=$(wildcard sem/*.c)
SOURCES_PIPES=$(wildcard pipes/*.c)
OBJECTS_PIPES=$(SOURCES_PIPES:.c=.o)

OBJECTS=$(SOURCES:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
OBJECTS_INTERRUPTS=$(SOURCES_INTERRUPTS:.c=.o)
OBJECTS_INTERRUPTS_ASM=$(SOURCES_INTERRUPTS_ASM:.asm=.o)
OBJECTS_DRIVERS=$(SOURCES_DRIVERS:.c=.o)

LOADERSRC=loader.asm

LOADEROBJECT=$(LOADERSRC:.asm=.o)
STATICLIBS=


all: $(KERNEL)
#ifneq (,$(findstring i, $(MAKEFLAGS)))
#	echo "se le pasó -i al make, acá habría que hacer MM = BUDDY_H"
#else
#	echo "no se le pasó -i, acá MM = MEM_MAN_H"
#endif #y despues correr el kernel. Tema que no se puede setear la variable MM de arriba desde acá

$(KERNEL): $(LOADEROBJECT) $(OBJECTS) $(STATICLIBS) $(OBJECTS_ASM) $(OBJECTS_INTERRUPTS) $(OBJECTS_INTERRUPTS_ASM) $(OBJECTS_DRIVERS) $(OBJECTS_MEM_MANAGER) $(OBJECTS_SEM) $(OBJECTS_PIPES)
	$(LD) $(LDFLAGS) -T kernel.ld -o $(KERNEL) $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(OBJECTS_MEM_MANAGER) $(OBJECTS_DRIVERS) $(OBJECTS_INTERRUPTS) $(OBJECTS_INTERRUPTS_ASM) $(OBJECTS_SEM) $(OBJECTS_PIPES) $(STATICLIBS)
	$(LD) $(LDFLAGS) -T kernel.ld --oformat=elf64-x86-64 -o kernel.elf $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(OBJECTS_MEM_MANAGER) $(OBJECTS_DRIVERS) $(OBJECTS_INTERRUPTS) $(OBJECTS_INTERRUPTS_ASM) $(OBJECTS_SEM) $(OBJECTS_PIPES) $(STATICLIBS)


%.o: %.c
	$(GCC) $(GCCFLAGS) $(MM) -I./include -I./interrupts -I./drivers -I./memory -I./sem -I.pipes -c $< -o $@

%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJECT):
	$(ASM) $(ASMFLAGS) $(LOADERSRC) -o $(LOADEROBJECT)

clean:
	rm -rf interrupts/*.o asm/*.o drivers/*.o *.o *.bin */*.o

.PHONY: all clean
