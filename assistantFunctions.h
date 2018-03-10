

#ifndef CODE_ASSISTANTFUNCTIONS_H
#define CODE_ASSISTANTFUNCTIONS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "constants.h"
#include "dataStructures.h"
#include <ctype.h>
#include <math.h>


char* concat(char *, char *);
tokenPtr splitToTokens(char *);
void checkFail(void *);
void splitIfNeeded(tokenPtr  *, tokenPtr *, char *);
void printLog(char *);
int checkIfHasValidLabel(tokenPtr);
int isNumber(char *toCheck);
int checkIfValidAddressingMode(int addressingMode, const int validAddressingModes[]);
int checkAddressingMode(tokenPtr operand);
int checkIfHasValidLabel(tokenPtr);
int analyzeGetArray(tokenPtr, int *);
int getCommandType(tokenPtr);
int checkStringToken(tokenPtr);
void storeString(char *string);
int checkIfValidLabelName(char *labelName);
labelPtr checkLabelName(char *name);
void addNumberToDataMemoryBase(int newNumber);



#endif