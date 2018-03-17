

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


void addToken(tokenPtr *, char *);
void freeTokenList(tokenPtr);


/*labels table list*/
typedef struct Label *labelPtr;
typedef struct Label{

    char *name;
    int address;
    int type;
    int hasEntry;
    labelPtr next;

}label;

int addLabel(labelPtr *, labelPtr);
void freeLabelList(labelPtr head);
void freeLastLabel(labelPtr head);
void setLabel(labelPtr, char *, int, int, int);


/*commands*/
typedef struct ActionCommand{
    char *name;
    int id;
    int commandCode;
    int sourceOperandValidAddressingModes[4];
    int destinyOperandValidAddressingModes[4];
}actionCommand;

typedef struct DataCommand{
    char *name;
    int id;
}dataCommand;

typedef struct Command{
    char *name;
    int id;
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
    char lineInString[MAX_LINE+1];
    commandLinePtr next;
}commandLine;

void addCommandLine(commandLinePtr *head, commandLinePtr new);
commandLinePtr setNewCommandLine(int lineNumber, int commandType, int hasLabel, int sourceOperandAddressingMode, int destinyOperandAddressingMode, tokenPtr tokenListHead);
void freeCommandLineList(commandLinePtr head);


/*extern reference*/
typedef struct ExternReference *externReferencePtr;
typedef struct ExternReference{
    char name[30];
    int address;
    externReferencePtr next;
}externReference;

void addExternReference(externReferencePtr *externReferenceHead, char *name, int address);
void freeExternReferenceList(externReferencePtr head);

#endif
