
#ifndef CODE_PREPAREFILES_H
#define CODE_PREPAREFILES_H

#include "stdio.h"
#include "dataStructures.h"

void prepareFiles(char *fileName, externReferencePtr externReferencesHead);
FILE *getFile(char *, char *);
int createOutputFiles(FILE **objFIle, FILE **externFile, FILE **entryFile, char *inputFileName);
void makeEntryFile(FILE *entryFile);
void makeObjFile(FILE *objFile);
void printToFile(FILE *toPrintFile, int numberTocConvert, char *stringToPrint);
void makeExternFile(FILE *externFile, externReferencePtr externReferencesHead);


#endif
