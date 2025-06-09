


#ifndef CODE_PREASSEMBLER_H
#define CODE_PREASSEMBLER_H

#include "dataStructures.h"
#include "constants.h"
#include <stdio.h>

int cleanupExtendedSourceFile(char *fileBasename);
int openPreassemblerFiles(char *fileBasename, FILE **initialSourceFilePtr, FILE **extendedSourceFilePtr);
int handleMacroDef(tokenPtr lineTokenList,char *lineString, int lineNumber, macroPtr *activeMacroPtr);
int preassembler(char *fileBasename);
int preassembleLine(FILE *extendedSourceFile, char *lineString, int lineNumber, macroPtr *activeMacroPtr);

#endif