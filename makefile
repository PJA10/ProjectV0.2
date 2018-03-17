assambler: assistantFunctions.o dataStructures.o firstPass.o main.o prepareFiles.o secondPass.o
	gcc -g -Wall -ansi -ggdb3 -pedantic assistantFunctions.o dataStructures.o firstPass.o main.o prepareFiles.o secondPass.o -o assambler

assistantFunctions.o: assistantFunctions.c
	gcc -c -Wall -ansi -ggdb3 -pedantic assistantFunctions.c -o assistantFunctions.o

dataStructures.o: dataStructures.c
	gcc -c -Wall -ansi -ggdb3 -pedantic dataStructures.c -o dataStructures.o

firstPass.o: firstPass.c
	gcc -c -Wall -ansi -ggdb3 -pedantic firstPass.c -o firstPass.o

main.o: main.c constants.h assistantFunctions.h
	gcc -c -Wall -ansi -ggdb3 -pedantic main.c -o main.o

prepareFiles.o: prepareFiles.c 
	gcc -c -Wall -ansi -ggdb3 -pedantic prepareFiles.c -o prepareFiles.o

secondPass.o: secondPass.c 
	gcc -c -Wall -ansi -ggdb3 -pedantic secondPass.c -o secondPass.o
