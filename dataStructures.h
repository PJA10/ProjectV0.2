

#ifndef CODE_DATASTRUCTURES_H
#define CODE_DATASTRUCTURES_H

#include "constants.h"
#include <stdio.h>

/*token linked list*/
typedef struct Token *tokenPtr;
typedef struct Token{

    char *string;
    tokenPtr next;

}token;

/*labels table list*/
typedef struct Label *labelPtr;
typedef struct Label{

    char *name;
    int address;
    int isExtarnal;
    int isAction;
    int hasEntry;
    labelPtr next;

}label;

int addLabel(labelPtr *, labelPtr);
void freeLabelList(labelPtr head);
void freeLastLabel(labelPtr head);
void setLabel(labelPtr, char *, int, int, int, int);


/*commands*/
typedef struct Command{
    char *name;
    int id;
    int sourceOperandValidAddressingModes[4];
    int destinyOperandValidAddressingModes[4];
}command;

/*command line*/
typedef struct CommandLine *commandLinePtr;
typedef struct CommandLine{
    int lineNumber;
    int commandType;
    int hasLabel;
    int sourceOperandAddressingMode;
    int destinyOperandAddressingMode;
    tokenPtr tokenListHead;
    commandLinePtr next;
}commandLine;

void addCommandLine(commandLinePtr *head, commandLinePtr new);
commandLinePtr setNewCommandLine(int lineNumber, int commandType, int hasLabel, int sourceOperandAddressingMode, int destinyOperandAddressingMode, tokenPtr tokenListHead);


void addToken(tokenPtr *, char *);
void freeTokenList(tokenPtr);

#endif
