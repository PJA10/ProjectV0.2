

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
int handleExternCommand(tokenPtr, int);
int handleActionCommands(commandLinePtr);
int handleEntryCommand(commandLinePtr);
int handleCommandLine(commandLinePtr commandLine, commandLinePtr *secondPassCommandsHead);
int handleLine(char *lineString, int lineNumber, commandLinePtr *secondPassCommandsHead);


#endif
