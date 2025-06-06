CFLAGS= -Wall -ansi -ggdb3 -pedantic

OBJS=\
assistantFunctions.o \
secondPass.o \
prepareFiles.o \
main.o \
firstPass.o \
dataStructures.o \

.PHONY: all clean install
.SUFFIXES: .o .c

install all: assembler

assembler: $(OBJS)
	$(CC) -g $(OBJS) -o $@ $(CFLAGS)

.c.o:
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -f assembler
	rm -f $(OBJS) *.o
