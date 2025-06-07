

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
int getAddressingMode(tokenPtr operand);
int isRegisterName(char *);
int isMatrixAddressing(char *);
int checkIfHasValidLabel(tokenPtr);
int analyzeGetArray(tokenPtr, int *);
int getCommandType(tokenPtr);
int checkStringToken(tokenPtr);
void storeString(char *string);
int checkIfValidLabelName(char *labelName, int toPrint);
labelPtr checkLabelName(char *name);
void addNumberToDataMemoryBase(int newNumber);
int NumberOfLinesToSkip(int sourceAddressingMode, int destinationAddressingMode);
void codeActionCommand(int destenationOperandAddressingMode, int sourceOperandAddressingMode, int commandType);
void printInBinary(int num);
void printMemoryBase(int *memoryBase);
void addNumberToActionMemoryBase(int number);
void addAddressToActionMemoryBase(labelPtr label, externReferencePtr *externReferencesHead);
void intToBase4(char *,int num);
int handleOperand(commandLinePtr actionCommandLine, tokenPtr operandToken, int whatOperand);
int handleTwoOperands(commandLinePtr actionCommandLine, tokenPtr firstOperandToken);
int checkAddressingMode(tokenPtr operandToken, int commandType, int whatOperand);
int handleLabel(char *labelName, int address, int labelType);
int handleNotCommandLines(tokenPtr head);
void printLineError(char *lineString, int lineNumber, tokenPtr head);


#endif
