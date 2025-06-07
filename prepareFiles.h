
#ifndef CODE_PREPAREFILES_H
#define CODE_PREPAREFILES_H

#include "stdio.h"
#include "dataStructures.h"

void prepareFiles(char *fileName, externReferencePtr externReferencesHead);
FILE *getFile(char *, char *);
int createOutputFiles(FILE **objFIle, FILE **externFile, FILE **entryFile, char *inputFileName);
int makeEntryFile(FILE *entryFile);
void makeObjFile(FILE *objFile);
void printNumberToFile(FILE *toPrintFile, int numberTocConvert, char *stringToPrint);
int makeExternFile(FILE *externFile, externReferencePtr externReferencesHead);
int readLineFromFile(FILE *file, char buff[MAX_LINE+1]);


#endif
