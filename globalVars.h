
#ifndef CODE_GLOBALVARS_H
#define CODE_GLOBALVARS_H

#include "dataStructures.h"

extern int IC; /*the instruction counter*/
extern int DC; /*the data counter*/
extern int logIsOn; /*a flag for log print*/
extern labelPtr labelTable; /*a linked list of all the labels*/
extern const commandInfo actionCommandsArray[]; /*an array with every action command with info*/
extern const char *registers[]; /*a array of all the registers names*/
extern int actionMemoryBase[]; /*the action memory base*/
extern int dataMemoryBase[]; /*the data memory base*/
extern const char *addressingModes[]; /*a array of all the addressing modes names - for printing*/
extern const char base32[]; /*the strange 32 base*/

#endif
