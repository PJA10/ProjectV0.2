/*
 * Created by PJA on 3/5/2018.
*/

#include "prepareFiles.h"
#include "assistantFunctions.h"
#include "globalVars.h"

void makeObjFile(FILE *objFile);

void prepareFiles(char *inputFileName){
    FILE *objFile;
    FILE *externFile;
    FILE *entryFile;
    if(createOutputFiles(&objFile, &externFile, &entryFile, inputFileName) == FAIL) {
        return;
    }
    makeObjFile(objFile);
    return;
}


void makeObjFile(FILE *objFile) {
    char *curr;
    int i;
    curr = intToBase32(IC-MEMORY_START_POS);
    fprintf(objFile, "\t %s ", curr);
    free(curr);
    curr = intToBase32(DC);
    fprintf(objFile, "%s\n", curr);
    free(curr);
    for(i = 0; i < IC-MEMORY_START_POS; i++) {
        curr = intToBase32(i + MEMORY_START_POS);
        fprintf(objFile, "%s\t", curr);
        free(curr);
        curr = intToBase32(actionMemoryBase[i]);
        fprintf(objFile, "%s\n", curr);
        free(curr);
    }
    for(i = 0; i < DC; i++) {
        curr = intToBase32(i+IC);
        fprintf(objFile, "%s\t", curr);
        free(curr);
        curr = intToBase32(dataMemoryBase[i]);
        fprintf(objFile, "%s\n", curr);
        free(curr);
    }
}


int createOutputFiles(FILE **objFIle, FILE **externFile, FILE **entryFile, char *inputFileName) {
    char *fileName = concat(inputFileName, ".ob");
    (*objFIle) = getFile(fileName, "w");
    free(fileName);
    if(*objFIle == NULL) {
        return FAIL;
    }
    fileName = concat(inputFileName, ".ext");
    (*externFile) = getFile(fileName, "w");
    free(fileName);
    if(*externFile == NULL) {
        fclose(*objFIle);
        return FAIL;
    }
    fileName = concat(inputFileName, ".ent");
    (*entryFile) = getFile(fileName, "w");
    free(fileName);
    if(*entryFile == NULL) {
        fclose(*objFIle);
        fclose(*entryFile);
        return FAIL;
    }
}


/**
 * getFile
 *
 * The function open a file with a given name in a given mode
 * The function prints if there was a problem
 *
 * params
 * fileName - a sting with the file name
 * mode - the mode to open the file with, for example "r* for read or "w" for write
 *
 * return:
 * FILE * - file pointer to the fail, and NULL if there was a problem
 *
 * */
FILE *getFile(char *fileName, char *mode) {
    FILE *file = fopen(fileName, mode); /*using fopen*/
    if(file == NULL) {
        printf("Error, with getting file %s with mode %s\n", fileName, mode); /*print that there was a problem*/
        return NULL;
    }
    return  file;
}