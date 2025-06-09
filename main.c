/*
 * mmn14
 * main.c
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
int logIsOn = TRUE; /*turn on the log print*/
labelPtr labelTable = NULL;
macroPtr macroList = NULL;
const commandInfo actionCommandsArray[NUM_OF_ACTION_COMMANDS] = {
        /*a array to store all the functions names and id for identification*/
        {"mov", MOV, MOV_OP_CODE, {IMMEDIATE_ADDRESSING,DIRECT_ADDRESSING,MATRIX_ACCESS_ADRESSING,REGISTERS_ADDRESSING},
                {DIRECT_ADDRESSING,MATRIX_ACCESS_ADRESSING,REGISTERS_ADDRESSING,END_OF_ADDRESSING_LIST}},

        {"cmp", CMP, CMP_OP_CODE, {IMMEDIATE_ADDRESSING,DIRECT_ADDRESSING,MATRIX_ACCESS_ADRESSING,REGISTERS_ADDRESSING},
                {IMMEDIATE_ADDRESSING,DIRECT_ADDRESSING,MATRIX_ACCESS_ADRESSING,REGISTERS_ADDRESSING}},

        {"add", ADD, ADD_OP_CODE, {IMMEDIATE_ADDRESSING,DIRECT_ADDRESSING,MATRIX_ACCESS_ADRESSING,REGISTERS_ADDRESSING},
                {DIRECT_ADDRESSING,MATRIX_ACCESS_ADRESSING,REGISTERS_ADDRESSING,END_OF_ADDRESSING_LIST}},

        {"sub", SUB, SUB_OP_CODE, {IMMEDIATE_ADDRESSING,DIRECT_ADDRESSING,MATRIX_ACCESS_ADRESSING,REGISTERS_ADDRESSING},
                {DIRECT_ADDRESSING,MATRIX_ACCESS_ADRESSING,REGISTERS_ADDRESSING,END_OF_ADDRESSING_LIST}},

        {"lea", LEA, LEA_OP_CODE, {DIRECT_ADDRESSING,MATRIX_ACCESS_ADRESSING, END_OF_ADDRESSING_LIST},
                {DIRECT_ADDRESSING,MATRIX_ACCESS_ADRESSING,REGISTERS_ADDRESSING,END_OF_ADDRESSING_LIST}},

        {"not", NOT, NOT_OP_CODE, {END_OF_ADDRESSING_LIST}
                ,{DIRECT_ADDRESSING,MATRIX_ACCESS_ADRESSING,REGISTERS_ADDRESSING,END_OF_ADDRESSING_LIST}},

        {"clr", CLR, CLR_OP_CODE, {END_OF_ADDRESSING_LIST}
                ,{DIRECT_ADDRESSING,MATRIX_ACCESS_ADRESSING,REGISTERS_ADDRESSING,END_OF_ADDRESSING_LIST}},

        {"inc", INC, INC_OP_CODE, {END_OF_ADDRESSING_LIST}
                ,{DIRECT_ADDRESSING,MATRIX_ACCESS_ADRESSING,REGISTERS_ADDRESSING,END_OF_ADDRESSING_LIST}},

        {"dec", DEC, DEC_OP_CODE, {END_OF_ADDRESSING_LIST}
                ,{DIRECT_ADDRESSING,MATRIX_ACCESS_ADRESSING,REGISTERS_ADDRESSING,END_OF_ADDRESSING_LIST}},

        {"jmp", JMP, JMP_OP_CODE, {END_OF_ADDRESSING_LIST}
                ,{DIRECT_ADDRESSING,MATRIX_ACCESS_ADRESSING,REGISTERS_ADDRESSING,END_OF_ADDRESSING_LIST}},

        {"bne", BNE, BNE_OP_CODE, {END_OF_ADDRESSING_LIST}
                ,{DIRECT_ADDRESSING,MATRIX_ACCESS_ADRESSING,REGISTERS_ADDRESSING,END_OF_ADDRESSING_LIST}},

        {"red", RED, RED_OP_CODE, {END_OF_ADDRESSING_LIST}
                ,{DIRECT_ADDRESSING,MATRIX_ACCESS_ADRESSING,REGISTERS_ADDRESSING,END_OF_ADDRESSING_LIST}},

        {"prn", PRN, PRN_OP_CODE, {END_OF_ADDRESSING_LIST}
                ,{IMMEDIATE_ADDRESSING,DIRECT_ADDRESSING,MATRIX_ACCESS_ADRESSING,REGISTERS_ADDRESSING}},

        {"jsr", JSR, JSR_OP_CODE, {END_OF_ADDRESSING_LIST}
                ,{DIRECT_ADDRESSING,MATRIX_ACCESS_ADRESSING,REGISTERS_ADDRESSING,END_OF_ADDRESSING_LIST}},

        {"rts", RTS, RTS_OP_CODE, {END_OF_ADDRESSING_LIST}, {END_OF_ADDRESSING_LIST}},

        {"stop", STOP, STOP_OP_CODE, {END_OF_ADDRESSING_LIST} ,{END_OF_ADDRESSING_LIST}}
};
const char *registers[] = {"r0","r1","r2","r3","r4","r5","r6","r7"};

/*for printing*/
const char *addressingModes[] = {"immediate addressing", "direct addressing", "struct access addressing",
                                 "registers addressing", "unknown addressing mode"};
const char base4[] = {'a', 'b', 'c', 'd'};
int actionMemoryBase[MAX_MEMORY_LENGTH];
int dataMemoryBase[MAX_MEMORY_LENGTH];


int main(int args, char* argv[]) {
    int fileNum; /*the file number(first file, second file...)*/
    int success = SUCCESS;

    if(args < MIN_VAL_OF_ARGS) {
        printf("Error, have to get at lest one file name\n");
        return FAIL;
    }
    for(fileNum = FIRST_REAL_ARG; fileNum < args; fileNum++) { /*for every file name in argv*/
        commandLinePtr secondPassCommandsHead = NULL;
        externReferencePtr externReferencesHead = NULL;
        FILE *extendedSourceFile = NULL;
        char *extendedSourceFileName = NULL;;
        
        /*initialization of the counters*/
        resetProgram();

        success = preassembler(argv[fileNum]);
        if(success != SUCCESS) {
            goto iteration_cleanup;
        }
        
        extendedSourceFileName = concatToNew(argv[fileNum], ".am");
        extendedSourceFile = getFile(extendedSourceFileName, "r");
        free(extendedSourceFileName);
        if(extendedSourceFile == NULL) {
            success = FAIL;
            goto iteration_cleanup;
        }

        success = firstPass(extendedSourceFile, &secondPassCommandsHead);
        if(success != SUCCESS) {
            goto iteration_cleanup;
        }
        updateDataLabelsAddress();
        success = secondPass(secondPassCommandsHead, &externReferencesHead);
        if(success != SUCCESS) {
            goto iteration_cleanup;
        }
        prepareFiles(argv[fileNum], externReferencesHead);
        printf("file: %s succeeded\n\n", argv[fileNum]);

iteration_cleanup:
        if(success != SUCCESS) {
            printf("file: %s filed\n\n", argv[fileNum]);
            cleanupExtendedSourceFile(argv[fileNum]);
        }
        if(extendedSourceFile != NULL) {
                fclose(extendedSourceFile);
        }
        freeExternReferenceList(externReferencesHead);
        freeCommandLineList(secondPassCommandsHead);
        freeGlobal();
    }
    /*printMemoryBase(actionMemoryBase);
    printMemoryBase(dataMemoryBase);*/
    return EXIT_SUCCESS;
}

int cleanupExtendedSourceFile(char *fileBasename) {
    int success = SUCCESS;
    FILE *extendedSourceFile = NULL;
    char *extendedSourceFileName = NULL;;

    extendedSourceFileName = concatToNew(fileBasename, ".am");

    extendedSourceFile = fopen(extendedSourceFileName, "r");
    if (extendedSourceFile) { /* only if file exists */
        fclose(extendedSourceFile);
        if (remove(extendedSourceFileName) != 0) {
            fprintf(stderr, "ERROR: can't delete file %s\n", extendedSourceFileName);
            success = FAIL;
            goto cleanup;;
        }
    }

cleanup:
    if(extendedSourceFileName) {
        free(extendedSourceFileName);
    }
    return success;
}


int openPreassemblerFiles(char *fileBasename, FILE **initialSourceFilePtr, FILE **extendedSourceFilePtr) {
    int success = FAIL;
    FILE *initialSourceFile = NULL;
    FILE *extendedSourceFile = NULL;
    char *initialSourceFileName = NULL;
    char *extendedSourceFileName = NULL;;
    initialSourceFileName = concatToNew(fileBasename, ".as");
    initialSourceFile = getFile(initialSourceFileName, "r");
    if(initialSourceFile == NULL) {
        success = FAIL;
        goto cleanup;
    }
    extendedSourceFileName = concatToNew(fileBasename, ".am");
    extendedSourceFile = getFile(extendedSourceFileName, "w");
    if(extendedSourceFile == NULL) {
        success = FAIL;
        goto cleanup;
    }

    (*initialSourceFilePtr) = initialSourceFile;
    (*extendedSourceFilePtr) = extendedSourceFile;
    success = SUCCESS;

cleanup:
    if(initialSourceFileName) {
        free(initialSourceFileName);
    }
    if(extendedSourceFileName) {
        free(extendedSourceFileName);
    }
    if(success != SUCCESS && initialSourceFile) {
        fclose(initialSourceFile);
    }
    if(success != SUCCESS && extendedSourceFile) {
        fclose(extendedSourceFile);
    }

    return success;
}


int handleMacroDef(tokenPtr lineTokenList,char *lineString, int lineNumber, macroPtr *activeMacroPtr) {
    tokenPtr currToken = lineTokenList->next;

    if(!currToken) {
        printLineError(lineString, lineNumber, lineTokenList);
        return FAIL;
    }
    if(checkIfValidMacroName(currToken->string, TRUE) == FAIL) {
        printLineError(lineString, lineNumber, lineTokenList);
        return FAIL;
    }
    if(findMacro(macroList, currToken->string) != NULL) {
        fprintf(stderr, "ERROR: macro name already exists\n");
        printLineError(lineString, lineNumber, lineTokenList);
        return FAIL;
    }
    if(currToken->next != NULL) {
        fprintf(stderr, "ERROR: excess characters in macro definition\n");
        printLineError(lineString, lineNumber, lineTokenList);
        return FAIL;
    }

    *activeMacroPtr = addmacro(&macroList, currToken->string);
    if(*activeMacroPtr == NULL) {
        printLineError(lineString, lineNumber, lineTokenList);
        return FAIL;
    }
    return SUCCESS;
}


int preassembler(char *fileBasename) {
    int success = SUCCESS;
    FILE *initialSourceFile = NULL;
    FILE *extendedSourceFile = NULL;
    int isErrorsFlag = FALSE;
    char lineString[MAX_LINE + 1] = {0};
    int lineNumber = 0;
    success = openPreassemblerFiles(fileBasename, &initialSourceFile, &extendedSourceFile);
    if(success != SUCCESS) {
        goto cleanup;
    }
    macroPtr activeMacro = NULL;
    while(!feof(initialSourceFile)) {
        success = readLineFromFile(initialSourceFile, lineString);
        lineNumber++;
        if(success == FAIL) {
            printLineError(lineString, lineNumber, NULL);
            isErrorsFlag = TRUE;
            continue;
        }
        tokenPtr lineTokenList = splitToTokens(lineString);
        tokenPtr currToken = lineTokenList;
        int i = 0;

        if(lineTokenList) {
            macroPtr calledMacro = findMacro(macroList, lineTokenList->string);
            if(calledMacro != NULL) {
                if(lineTokenList->next != NULL) {
                    fprintf(stderr, "ERROR: excess characters in macro call\n");
                    printLineError(lineString, lineNumber, lineTokenList);
                    isErrorsFlag = TRUE;
                    continue;
                }
                for(i = 0; i < calledMacro->lineCount; i++) {
                    fprintf(extendedSourceFile, "%s\n", calledMacro->lines[i]);
                }
                continue;
            }
        }

        if(lineTokenList && !strcmp(lineTokenList->string, MACRO_DEF_STRING)) {
            success = handleMacroDef(lineTokenList, lineString, lineNumber, &activeMacro);
            if(success != SUCCESS) {
                isErrorsFlag = TRUE;
            }
            continue;
        }
        if(activeMacro != NULL) {
            if(lineTokenList && strcmp(lineTokenList->string, MACRO_END_STRING) == 0) {
                if(lineTokenList->next != NULL) {
                    fprintf(stderr, "ERROR: excess characters in macro closer\n");
                    printLineError(lineString, lineNumber, lineTokenList);
                    isErrorsFlag = TRUE;
                    continue;
                }
                activeMacro = NULL;
            }
            else {
                if(addLineToMacro(activeMacro, lineString) != SUCCESS) {
                    printLineError(lineString, lineNumber, lineTokenList);
                    isErrorsFlag = TRUE;
                    continue;
                }
            }
        }
        else {
            fprintf(extendedSourceFile, "%s\n", lineString);
        }
    }

cleanup:
    if(isErrorsFlag == TRUE) {
        success = FAIL;
    }
    if(extendedSourceFile != NULL) {
        fclose(extendedSourceFile);
    }
    if(initialSourceFile != NULL) {
        fclose(initialSourceFile);
    }
    return success;
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
    freemacroList(macroList);
    macroList = NULL;
}
