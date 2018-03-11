
#ifndef CODE_PREPAREFILES_H
#define CODE_PREPAREFILES_H

#include "stdio.h"

void prepareFiles(char *fileName);
FILE *getFile(char *, char *);
int createOutputFiles(FILE **objFIle, FILE **externFile, FILE **entryFile, char *inputFileName);


#endif
