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

install all: assambler

assambler: $(OBJS)
	$(CC) -g $(OBJS) -o $@ $(CFLAGS)

.c.o:
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -f assambler
	rm -f $(OBJS) *.o
