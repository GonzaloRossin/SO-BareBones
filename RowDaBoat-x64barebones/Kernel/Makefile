include Makefile.inc

KERNEL=kernel.bin
SOURCES=$(wildcard *.c)
SOURCES_ASM=$(wildcard asm/*.asm)
SOURCES_INTERRUPTS=$(wildcard interrupts/*.c)
SOURCES_INTERRUPTS_ASM=$(wildcard interrupts/*.asm)
SOURCES_DRIVERS=$(wildcard drivers/*.c)
SOURCES_MEM_MANAGER=$(wildcard memory/*.c)
OBJECTS=$(SOURCES:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
OBJECTS_INTERRUPTS=$(SOURCES_INTERRUPTS:.c=.o)
OBJECTS_INTERRUPTS_ASM=$(SOURCES_INTERRUPTS_ASM:.asm=.o)
OBJECTS_DRIVERS=$(SOURCES_DRIVERS:.c=.o)
OBJECTS_MEM_MANAGER=$(SOURCES_MEM_MANAGER:.c=.o)
LOADERSRC=loader.asm

LOADEROBJECT=$(LOADERSRC:.asm=.o)
STATICLIBS=

all: $(KERNEL)

$(KERNEL): $(LOADEROBJECT) $(OBJECTS) $(STATICLIBS) $(OBJECTS_ASM) $(OBJECTS_INTERRUPTS) $(OBJECTS_INTERRUPTS_ASM) $(OBJECTS_DRIVERS) $(OBJECTS_MEM_MANAGER)
	$(LD) $(LDFLAGS) -T kernel.ld -o $(KERNEL) $^

%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include -I./interrupts -I./drivers -I./memory -c $< -o $@

%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJECT):
	$(ASM) $(ASMFLAGS) $(LOADERSRC) -o $(LOADEROBJECT)

clean:
	rm -rf interrupts/*.o asm/*.o drivers/*.o *.o *.bin

.PHONY: all clean
