/*
 * mmn14
 * main.c
 * Alon Wartski - 209729193 and Roy Lenz - 322585944
 *
 * this is the main file of this project.
 * it has all the global variables.
 * it calls each stage (like firstPass and secondPass) in its turn.
 * it prepares the files and free them at the end.
 *
 * */


#include "main.h"
#include "globalVars.h"
#include "firstPass.h"
#include "prepareFiles.h"
#include "assistantFunctions.h"
#include "secondPass.h"

int IC = MEMORY_START_POS;
int DC = 0;
int logIsOn;
labelPtr labelTable;
const commandInfo actionCommandsArray[NUM_OF_ACTION_COMMANDS] = {
        /*a array to store all the functions names and id for identification*/
        {"mov", MOV, MOV_OP_CODE, {IMMEDIATE_ADDRESSING,DIRECT_ADDRESSING,STRUCT_ACCESS_ADRESSING,REGISTERS_ADDRESSING},
                {DIRECT_ADDRESSING,STRUCT_ACCESS_ADRESSING,REGISTERS_ADDRESSING,END_OF_ADDRESSING_LIST}},

        {"cmp", CMP, CMP_OP_CODE, {IMMEDIATE_ADDRESSING,DIRECT_ADDRESSING,STRUCT_ACCESS_ADRESSING,REGISTERS_ADDRESSING},
                {IMMEDIATE_ADDRESSING,DIRECT_ADDRESSING,STRUCT_ACCESS_ADRESSING,REGISTERS_ADDRESSING}},

        {"add", ADD, ADD_OP_CODE, {IMMEDIATE_ADDRESSING,DIRECT_ADDRESSING,STRUCT_ACCESS_ADRESSING,REGISTERS_ADDRESSING},
                {DIRECT_ADDRESSING,STRUCT_ACCESS_ADRESSING,REGISTERS_ADDRESSING,END_OF_ADDRESSING_LIST}},

        {"sub", SUB, SUB_OP_CODE, {IMMEDIATE_ADDRESSING,DIRECT_ADDRESSING,STRUCT_ACCESS_ADRESSING,REGISTERS_ADDRESSING},
                {DIRECT_ADDRESSING,STRUCT_ACCESS_ADRESSING,REGISTERS_ADDRESSING,END_OF_ADDRESSING_LIST}},

        {"lea", LEA, LEA_OP_CODE, {DIRECT_ADDRESSING,STRUCT_ACCESS_ADRESSING, END_OF_ADDRESSING_LIST},
                {DIRECT_ADDRESSING,STRUCT_ACCESS_ADRESSING,REGISTERS_ADDRESSING,END_OF_ADDRESSING_LIST}},

        {"not", NOT, NOT_OP_CODE, {END_OF_ADDRESSING_LIST}
                ,{DIRECT_ADDRESSING,STRUCT_ACCESS_ADRESSING,REGISTERS_ADDRESSING,END_OF_ADDRESSING_LIST}},

        {"clr", CLR, CLR_OP_CODE, {END_OF_ADDRESSING_LIST}
                ,{DIRECT_ADDRESSING,STRUCT_ACCESS_ADRESSING,REGISTERS_ADDRESSING,END_OF_ADDRESSING_LIST}},

        {"inc", INC, INC_OP_CODE, {END_OF_ADDRESSING_LIST}
                ,{DIRECT_ADDRESSING,STRUCT_ACCESS_ADRESSING,REGISTERS_ADDRESSING,END_OF_ADDRESSING_LIST}},

        {"dec", DEC, DEC_OP_CODE, {END_OF_ADDRESSING_LIST}
                ,{DIRECT_ADDRESSING,STRUCT_ACCESS_ADRESSING,REGISTERS_ADDRESSING,END_OF_ADDRESSING_LIST}},

        {"jmp", JMP, JMP_OP_CODE, {END_OF_ADDRESSING_LIST}
                ,{DIRECT_ADDRESSING,STRUCT_ACCESS_ADRESSING,REGISTERS_ADDRESSING,END_OF_ADDRESSING_LIST}},

        {"bne", BNE, BNE_OP_CODE, {END_OF_ADDRESSING_LIST}
                ,{DIRECT_ADDRESSING,STRUCT_ACCESS_ADRESSING,REGISTERS_ADDRESSING,END_OF_ADDRESSING_LIST}},

        {"red", RED, RED_OP_CODE, {END_OF_ADDRESSING_LIST}
                ,{DIRECT_ADDRESSING,STRUCT_ACCESS_ADRESSING,REGISTERS_ADDRESSING,END_OF_ADDRESSING_LIST}},

        {"prn", PRN, PRN_OP_CODE, {END_OF_ADDRESSING_LIST}
                ,{IMMEDIATE_ADDRESSING,DIRECT_ADDRESSING,STRUCT_ACCESS_ADRESSING,REGISTERS_ADDRESSING}},

        {"jsr", JSR, JSR_OP_CODE, {END_OF_ADDRESSING_LIST}
                ,{DIRECT_ADDRESSING,STRUCT_ACCESS_ADRESSING,REGISTERS_ADDRESSING,END_OF_ADDRESSING_LIST}},

        {"rts", RTS, RTS_OP_CODE, {END_OF_ADDRESSING_LIST}, {END_OF_ADDRESSING_LIST}},

        {"stop", STOP, STOP_OP_CODE, {END_OF_ADDRESSING_LIST} ,{END_OF_ADDRESSING_LIST}}
};
const char *registers[] = {"r0","r1","r2","r3","r4","r5","r6","r7"};

/*for printing*/
const char *addressingModes[] = {"immediate addressing", "direct addressing", "struct access addressing",
                                 "registers addressing", "unknown addressing mode"};
const char base32[] = {'!','@','#','$','%','^','&','*','<','>','a','b','c','d','e','f','g','h','i','j','k','l','m','n',
                       'o','p','q','r','s','t','u','v'};
int actionMemoryBase[MAX_MEMORY_LENGTH];
int dataMemoryBase[MAX_MEMORY_LENGTH];

int main(int args, char* argv[]) {
    int fileNum; /*the file number(first file, second file...)*/
    logIsOn = FALSE; /*turn on the log print*/

    if(args < MIN_VAL_OF_ARGS) {
        printf("Error, have to get at lest one file name\n");
        return FAIL;
    }
    for(fileNum = FIRST_REAL_ARG; fileNum < args; fileNum++) { /*for every file name in argv*/
        commandLinePtr secondPassCommandsHead = NULL;
        externReferencePtr externReferencesHead = NULL;
        /*adding .as to the end of the file name */
        char *fileName = concat(argv[fileNum], ".as");
        FILE *file = getFile(fileName, "r");
        free(fileName);
        if(file == NULL) /*if we couldn't open the file*/
            continue; /*move to the next file name, we all ready printed the error in getFile*/

        /*initialization of the counters*/
        resetProgram();

        if(firstPass(file, &secondPassCommandsHead) == SUCCESS) {
            updateDataLabelsAddress();
            if(secondPass(secondPassCommandsHead, &externReferencesHead) == SUCCESS) {
                prepareFiles(argv[fileNum], externReferencesHead);
                printf("file: %s succeeded\n\n", argv[fileNum]);
            }
            else {
                printf("file: %s filed\n\n", argv[fileNum]);
            }
        }
        else {
            printf("file: %s filed\n\n", argv[fileNum]);
        }
		fclose(file);
        freeExternReferenceList(externReferencesHead);
        freeCommandLineList(secondPassCommandsHead);
        freeGlobal();
    }
    /*printMemoryBase(actionMemoryBase);
    printMemoryBase(dataMemoryBase);*/
    return EXIT_SUCCESS;
}


/**
 * updateDataLabelsAddress
 *
 * The function update the address in data labels
 *
 * */
void updateDataLabelsAddress() {
    labelPtr curr = labelTable;
    while(curr) {
        if(curr->type != ACTION_LABEL && curr->type != EXTERN_LABEL) {
            curr->address += IC;
        }
        curr = curr->next;
    }
}


/**
 * resetProgram
 *
 * The function resets the program global vars
 *
 * */
void resetProgram() {
    int i;
    IC = MEMORY_START_POS;
    DC = 0;
    for(i = 0; i < MAX_MEMORY_LENGTH; i++) {
        dataMemoryBase[i] = 0;
        actionMemoryBase[i] = 0;
    }
}

/**
 * freeGlobal
 *
 * The function free the list that are held by global pointers
 *
 * */
void freeGlobal() {
    freeLabelList(labelTable);
    labelTable = NULL;
}
