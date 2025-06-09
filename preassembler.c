
/*
 * mmn14
 * preassembler.c
 *
 * This file is in charge of the preassembler phase.
 * in preassembler phase we go over each line in the initial source file (.as) and handle it's macros.
 * the extended source file after resolving the macros is saved as a .am file.
 *
 * */

 #include "preassembler.h"
 #include "globalVars.h"
 #include "assistantFunctions.h"
 #include "dataStructures.h"
 #include "prepareFiles.h"

 
/**
 * preassembler
 *
 * This is the main function of the preassembler phase.
 * This function reads the source file line by line and processes macro definitions and calls.
 * It builds an extended source file (.am) with the macros expanded in place.
 * If any error is encountered while reading a line or processing macros, the function will return FAIL.
 * The function handles macro definitions, macro closures, and macro calls
 *
 * params:
 * fileBasename - the base name of the file (without extension)
 *
 * return:
 * int - SUCCESS if preprocessing completed without errors, otherwise FAIL
 *
 * */
int preassembler(char *fileBasename) {
    int success = SUCCESS;
    FILE *initialSourceFile = NULL, *extendedSourceFile = NULL;
    int isErrorsFlag = FALSE, lineNumber = 0;
    char lineString[MAX_LINE + 1] = {0};
    macroPtr activeMacro = NULL;

    success = openPreassemblerFiles(fileBasename, &initialSourceFile, &extendedSourceFile);
    if(success != SUCCESS) {
        goto cleanup;
    }
    
    while(!feof(initialSourceFile)) {
        success = readLineFromFile(initialSourceFile, lineString);
        lineNumber++;
        if(success != SUCCESS) {
            printLineError(lineString, lineNumber, NULL);
            isErrorsFlag = TRUE;
            continue;
        }
        success = preassembleLine(extendedSourceFile, lineString, lineNumber, &activeMacro);
        if(success != SUCCESS) {
            isErrorsFlag = TRUE;
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
 * cleanupExtendedSourceFile
 *
 * This function deletes the extended source file (.am) if it exists.
 * It is used to clean up leftover files.
 * If the file cannot be deleted, the function will return FAIL.
 *
 * params:
 * fileBasename - the base name of the file (without extension)
 *
 * return:
 * int - SUCCESS if the file was deleted or didn't exist, otherwise FAIL
 *
 * */
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


/**
 * openPreassemblerFiles
 *
 * This function opens the initial source file (.as) for reading
 * and the extended source file (.am) for writing.
 * If either file fails to open, the function will return FAIL.
 *
 * params:
 * fileBasename - the base name of the file (without extension)
 * initialSourceFilePtr - pointer to the FILE* that will be set to the .as file
 * extendedSourceFilePtr - pointer to the FILE* that will be set to the .am file
 *
 * return:
 * int - SUCCESS if both files were opened successfully, otherwise FAIL
 *
 * */
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


/**
 * handleMacroDef
 *
 * This function handles a macro definition line.
 * It checks for macro syntax validity, ensures the name is legal and unique,
 * and creates a new macro structure in the global list.
 * If there are any syntax or naming issues, it returns FAIL.
 *
 * params:
 * lineTokenList - the list of tokens parsed from the line
 * lineString - the original line string
 * lineNumber - the line number in the file
 * activeMacroPtr - pointer to the current active macro, to be set if macro is valid
 *
 * return:
 * int - SUCCESS if macro is defined correctly, otherwise FAIL
 *
 * */
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


/**
 * preassembleLine
 *
 * This function processes a single line from the source file during the preassembler phase.
 * It handles macro calls, macro definitions, macro closures, and regular lines.
 * Macro lines are stored and expanded as needed, while regular lines are written directly to output.
 * Any syntax errors or logic issues in macros will cause the function to return FAIL.
 *
 * params:
 * extendedSourceFile - the file pointer to the output (.am) file
 * lineString - the line string read from the source file
 * lineNumber - the current line number in the file
 * activeMacroPtr - pointer to the current active macro definition, if any
 *
 * return:
 * int - SUCCESS if line handled correctly, otherwise FAIL
 *
 * */
int preassembleLine(FILE *extendedSourceFile, char *lineString, int lineNumber, macroPtr *activeMacroPtr) {
    int i = 0, success = SUCCESS;
    tokenPtr lineTokenList = splitToTokens(lineString);

    if(lineTokenList) {
        macroPtr calledMacro = findMacro(macroList, lineTokenList->string);
        if(calledMacro != NULL) {
            if(lineTokenList->next != NULL) {
                fprintf(stderr, "ERROR: excess characters in macro call\n");
                printLineError(lineString, lineNumber, lineTokenList);
                return FAIL;
            }
            for(i = 0; i < calledMacro->lineCount; i++) {
                fprintf(extendedSourceFile, "%s\n", calledMacro->lines[i]);
            }
            return SUCCESS;
        }
    }

    if(lineTokenList && !strcmp(lineTokenList->string, MACRO_DEF_STRING)) {
        success = handleMacroDef(lineTokenList, lineString, lineNumber, activeMacroPtr);
        if(success != SUCCESS) {
            return FAIL;
        }
        return SUCCESS;;
    }
    if(*activeMacroPtr != NULL) {
        if(lineTokenList && strcmp(lineTokenList->string, MACRO_END_STRING) == 0) {
            if(lineTokenList->next != NULL) {
                fprintf(stderr, "ERROR: excess characters in macro closer\n");
                printLineError(lineString, lineNumber, lineTokenList);
                return FAIL;
            }
            *activeMacroPtr = NULL;
        }
        else {
            if(addLineToMacro(*activeMacroPtr, lineString) != SUCCESS) {
                printLineError(lineString, lineNumber, lineTokenList);
                return FAIL;
            }
        }
    }
    else {
        fprintf(extendedSourceFile, "%s\n", lineString);
    }
    return success;
}
