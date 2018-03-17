/*
 * Created by PJA on 3/2/2018.
*/


#include "main.h"
#include "globalVars.h"
#include "firstPass.h"
#include "prepareFiles.h"
#include "assistantFunctions.h"
#include "secondPass.h"
int IC = MEMORY_START_POS;
int DC = 0;
int logIsOn;
labelPtr labelTabale;
const dataCommand dataCommands[NUM_OF_DATA_COMMAND_TYPES] = { /*a array to store all the functions names and id for identification*/
        {".data", DATA,},
        {".string", STRING},
        {".struct", STRUCT},
        {".extern", EXTERN},
        {".entry", ENTRY}
};
const actionCommand actionCommands[NUM_OF_ACTION_COMMAND_TYPES+1] = {
        {"mov",     MOV, MOV_CODE, {IMMEDIATE_ADDRESSING,DIRECT_ADDRESSING,STRUCT_ACCESS_ADRESSING,REGISTERS_ADDRESSING}, {DIRECT_ADDRESSING,STRUCT_ACCESS_ADRESSING,REGISTERS_ADDRESSING,END_OF_ADDRESSING_LIST}},
        {"cmp",     CMP, CMP_CODE, {IMMEDIATE_ADDRESSING,DIRECT_ADDRESSING,STRUCT_ACCESS_ADRESSING,REGISTERS_ADDRESSING}, {IMMEDIATE_ADDRESSING,DIRECT_ADDRESSING,STRUCT_ACCESS_ADRESSING,REGISTERS_ADDRESSING}},
        {"add",     ADD, ADD_CODE, {IMMEDIATE_ADDRESSING,DIRECT_ADDRESSING,STRUCT_ACCESS_ADRESSING,REGISTERS_ADDRESSING},{DIRECT_ADDRESSING,STRUCT_ACCESS_ADRESSING,REGISTERS_ADDRESSING,END_OF_ADDRESSING_LIST}},
        {"sub",     SUB, SUB_CODE, {IMMEDIATE_ADDRESSING,DIRECT_ADDRESSING,STRUCT_ACCESS_ADRESSING,REGISTERS_ADDRESSING} ,{DIRECT_ADDRESSING,STRUCT_ACCESS_ADRESSING,REGISTERS_ADDRESSING,END_OF_ADDRESSING_LIST}},
        {"lea",     LEA, LEA_CODE, {DIRECT_ADDRESSING,STRUCT_ACCESS_ADRESSING, END_OF_ADDRESSING_LIST} ,{DIRECT_ADDRESSING,STRUCT_ACCESS_ADRESSING,REGISTERS_ADDRESSING,END_OF_ADDRESSING_LIST}},
        {"not",     NOT, NOT_CODE, {END_OF_ADDRESSING_LIST} ,{DIRECT_ADDRESSING,STRUCT_ACCESS_ADRESSING,REGISTERS_ADDRESSING,END_OF_ADDRESSING_LIST}},
        {"clr",     CLR, CLR_CODE, {END_OF_ADDRESSING_LIST} ,{DIRECT_ADDRESSING,STRUCT_ACCESS_ADRESSING,REGISTERS_ADDRESSING,END_OF_ADDRESSING_LIST}},
        {"inc",     INC, INC_CODE, {END_OF_ADDRESSING_LIST} ,{DIRECT_ADDRESSING,STRUCT_ACCESS_ADRESSING,REGISTERS_ADDRESSING,END_OF_ADDRESSING_LIST}},
        {"dec",     DEC, DEC_CODE, {END_OF_ADDRESSING_LIST} ,{DIRECT_ADDRESSING,STRUCT_ACCESS_ADRESSING,REGISTERS_ADDRESSING,END_OF_ADDRESSING_LIST}},
        {"jmp",     JMP, JMP_CODE, {END_OF_ADDRESSING_LIST} ,{DIRECT_ADDRESSING,STRUCT_ACCESS_ADRESSING,REGISTERS_ADDRESSING,END_OF_ADDRESSING_LIST}},
        {"bne",     BNE, BNE_CODE, {END_OF_ADDRESSING_LIST} ,{DIRECT_ADDRESSING,STRUCT_ACCESS_ADRESSING,REGISTERS_ADDRESSING,END_OF_ADDRESSING_LIST}},
        {"red",     RED, RED_CODE, {END_OF_ADDRESSING_LIST} ,{DIRECT_ADDRESSING,STRUCT_ACCESS_ADRESSING,REGISTERS_ADDRESSING,END_OF_ADDRESSING_LIST}},
        {"prn",     PRN, PRN_CODE, {END_OF_ADDRESSING_LIST} ,{IMMEDIATE_ADDRESSING,DIRECT_ADDRESSING,STRUCT_ACCESS_ADRESSING,REGISTERS_ADDRESSING}},
        {"jsr",     JSR, JSR_CODE, {END_OF_ADDRESSING_LIST} ,{DIRECT_ADDRESSING,STRUCT_ACCESS_ADRESSING,REGISTERS_ADDRESSING,END_OF_ADDRESSING_LIST}},
        {"rts",     RTS, RTS_CODE, {END_OF_ADDRESSING_LIST} ,{END_OF_ADDRESSING_LIST}},
        {"stop",    STOP, STOP_CODE, {END_OF_ADDRESSING_LIST} ,{END_OF_ADDRESSING_LIST}},
        {"unknown", UNKNOWN, UNKNOWN_CODE, {-1},{-1}}
};
const command commands[NUM_OF_COMMAND_TYPES] = { /*a array to store all the functions names and id for identification*/
        {".data", DATA,},
        {".string", STRING},
        {".struct", STRUCT},
        {".extern", EXTERN},
        {".entry", ENTRY},
        {"mov",     MOV},
        {"cmp",     CMP},
        {"add",     ADD},
        {"sub",     SUB},
        {"lea",     LEA},
        {"not",     NOT},
        {"clr",     CLR},
        {"inc",     INC},
        {"dec",     DEC},
        {"jmp",     JMP},
        {"bne",     BNE},
        {"red",     RED},
        {"prn",     PRN},
        {"jsr",     JSR},
        {"rts",     RTS},
        {"stop",    STOP},
        {"unknown", UNKNOWN}
};

const char *registers[] = {"r0","r1","r2","r3","r4","r5","r6","r7"};
const char *addressingModes[] = {"immediate addressing", "direct addressing", "struct access addressing", "registers addressing", "unknown addressing mode"};
const char base32[] = {'!','@','#','$','%','^','&','*','<','>','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v'};
int actionMemoryBase[MAX_MEMORY_LENGTH];
int dataMemoryBase[MAX_MEMORY_LENGTH];

int main(int args, char* argv[]) {
    int fileNum; /*the file number(first file, second file...)*/
    commandLinePtr secondPassCommandsHead = NULL;
    externReferencePtr externReferencesHead = NULL;
    logIsOn = TRUE; /*turn on the log print*/

    if(args < MIN_VAL_OF_ARGS) {
        printf("Error, have to get at lest one file name\n");
        return FAIL;
    }
    for(fileNum = FIRST_REAL_ARG; fileNum < args; fileNum++) { /*for every file name in argv*/
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
            }
        }
        freeExternReferenceList(externReferencesHead);
        freeCommandLineList(secondPassCommandsHead);
        freeGlobal();
    }
    printMemoryBase(actionMemoryBase);
    printMemoryBase(dataMemoryBase);
    return EXIT_SUCCESS;
}


/**
 * updateDataLabelsAddress
 *
 * The function update the address in data labels
 *
 * */
void updateDataLabelsAddress() {
    labelPtr curr = labelTabale;
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
    freeLabelList(labelTabale);
}