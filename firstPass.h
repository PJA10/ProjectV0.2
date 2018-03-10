

#ifndef CODE_FIRSTPASS_H
#define CODE_FIRSTPASS_H

#include "dataStructures.h"
#include "constants.h"
#include <stdio.h>

int firstPass(FILE *, commandLinePtr *);
int handleDataCommands(tokenPtr, int, int);
int handleDotData(tokenPtr);
int handleDotString(tokenPtr commandToken);
int handleDotStruct(tokenPtr commandToken);
int handleExternCommand(tokenPtr, int, int);
int handleActionCommands(tokenPtr, int, int, int);

#endif
