
#ifndef CODE_GLOBALVARS_H
#define CODE_GLOBALVARS_H

#include "dataStructures.h"

extern int IC;
extern int DC;
extern int logIsOn;
extern labelPtr labelTable;
extern const commandInfo actionCommandsArray[];
extern const char *registers[];
extern int actionMemoryBase[];
extern int dataMemoryBase[];
extern const char *addressingModes[];
extern const char base32[];

#endif