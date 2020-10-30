include ../Makefile.inc

MODULE=0000-sampleCodeModule.bin
SOURCES=$(wildcard [^_]*.c)
SOURCES_ASM=$(wildcard *.asm)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
OBJECTS=$(SOURCES:.c=.o)


all: $(MODULE)

$(MODULE): $(OBJECTS) $(OBJECTS_ASM)  
	$(GCC) $(GCCFLAGS) -T sampleCodeModule.ld _loader.c  $(OBJECTS) $(OBJECTS_ASM)  -o ../$(MODULE)

%.o: %.c
	$(GCC) $(GCCFLAGS)  -I./Include  -c $< -o $@

%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@


clean:
	rm -rf *.o

.PHONY: all clean print
