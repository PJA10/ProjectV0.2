/*
 * mmn14
 * prepareFiles.c
 *
 * this file is in charge of everything that is related to files.
 * it prepares the files, gets the files and prints to the files
 *
 * */

#include "prepareFiles.h"
#include "assistantFunctions.h"
#include "globalVars.h"


/**
 * prepareFiles
 *
 * The function prepare the output files for the program
 * Object file with the machine code
 * Extern File with every reference to an external label in the machine code
 * And a entry file with declaration of every entry label
 *
 * params:
 * inputFileName - the input file name, the output files names is the same as the input name
 * externReferencesHead - a linked list of every external label reference
 *
 * */
void prepareFiles(char *inputFileName, externReferencePtr externReferencesHead){
    FILE *objFile;
    FILE *externFile;
    FILE *entryFile;

    if(createOutputFiles(&objFile, &externFile, &entryFile, inputFileName) == FAIL) { /*create the files*/
        return;
    }

    makeObjFile(objFile);
    fclose(objFile);

    if(makeEntryFile(entryFile) == FALSE) {
        char *fileName = concat(inputFileName, ".ent");
        remove(fileName);
        free(fileName);
    }
	fclose(entryFile);

    if(makeExternFile(externFile, externReferencesHead) == FALSE) {
        char *fileName = concat(inputFileName, ".ext");
        remove(fileName);
        free(fileName);
    }
    fclose(externFile);
}


/**
 * makeExternFile
 *
 * This function write into the extern file
 * The function will write every external label reference in the machine code
 * Every row is one reference with the label name followed by the code address
 *
 * params:
 * externFile - the file to write to
 * externReferencesHead - a linked list of every external label reference
 *
 * */
int makeExternFile(FILE *externFile, externReferencePtr externReferencesHead) {
    externReferencePtr curr = externReferencesHead;
    if(externReferencesHead == NULL) {
        return FALSE;
    }
    while(curr) { /*run over the list*/
        /*and print the extern label name followed by the an address with this label*/
        fprintf(externFile, "%s\t", curr->name);
        printNumberToFile(externFile, curr->address, "\n");
        curr = curr->next;
    }
    return TRUE;
}



/**
 * makeObjFile
 *
 * This function write into the object file
 * The function will write the machine code
 * The first line in the file will be the length of the action memory and the data memory
 * After that every row will be an address followed by the code of the address
 *
 * params:
 * objFile - the file we want to print to
 *
 * */
void makeObjFile(FILE *objFile) {
    int i;
    printNumberToFile(objFile, IC-MEMORY_START_POS, "\t"); /*print the length of the action memory base*/
    printNumberToFile(objFile, DC, "\n"); /*print the length of the data memory base*/
    for(i = 0; i < IC-MEMORY_START_POS; i++) { /*for every action memory word we have written*/
        printNumberToFile(objFile, i + MEMORY_START_POS, "\t"); /*print the address and tab*/
        /*i + MEMORY_START_POS because i doesn't include the 100 memory start position*/
        printNumberToFile(objFile, actionMemoryBase[i], "\n"); /*print the machine code in this address*/
    }
    for(i = 0; i < DC; i++) { /*for every data memory word we have written*/
        printNumberToFile(objFile, i + IC, "\t"); /*print the address and tab*/
        /*i + IC because we want the data memory to be right after the action memory*/
        printNumberToFile(objFile, dataMemoryBase[i], "\n"); /*print the machine code in this address*/
    }
}


/**
 * makeEntryFile
 *
 * This function write the entry file
 * The function will write every label that have a .entry command in the next format:
 * the label name followed by the label address in the strange 4 base
 *
 * params:
 * entryFile - the file we want to print to
 *
 * */
int makeEntryFile(FILE *entryFile) {
    labelPtr curr = labelTable;
    int wroteSomething = FALSE;
    while(curr) { /*for every label*/
        if(curr->hasEntry) { /*if the label have an .entry command*/
            fprintf(entryFile, "%s\t", curr->name); /*print the label name*/
            printNumberToFile(entryFile, curr->address, "\n"); /*print the label address*/
            wroteSomething = TRUE;
        }
        curr = curr->next;
    }
    return wroteSomething;
}


/**
 * createOutputFiles
 *
 * This function open the output files
 * If there is an error with one of the files the function will close the other files and return FAIL
 *
 * params:
 * objFIle - a pointer to the object file location
 * externFile - a pointer to the extern file location
 * entryFile - a pointer to the entry file location
 * inputFileName - the input file name, the output files names are the same as the input file name
 *
 * return:
 * int - if the function failed or succeeded
 *
 * */
int createOutputFiles(FILE **objFIle, FILE **externFile, FILE **entryFile, char *inputFileName) {
    char *fileName = concat(inputFileName, ".ob");

    (*objFIle) = getFile(fileName, "w");
    free(fileName);
    if(*objFIle == NULL) { /*if the been an error*/
        return FAIL;
    }
    fileName = concat(inputFileName, ".ext");
    (*externFile) = getFile(fileName, "w");
    free(fileName);
    if(*externFile == NULL) { /*if the been an error*/
        fclose(*objFIle);
        return FAIL;
    }
    fileName = concat(inputFileName, ".ent");
    (*entryFile) = getFile(fileName, "w");
    free(fileName);
    if(*entryFile == NULL) { /*if the been an error*/
        fclose(*objFIle);
        fclose(*entryFile);
        return FAIL;
    }

    return SUCCESS;
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


/**
 * printNumberToFile
 *
 * This function print to a file a given number in the strange 4 base followed by a given string
 *
 * params:
 * toPrintFile - the file we want to print to
 * numberTocConvert - the number we want to print in the strange 4 base
 * stringToPrint - the string we want that will be after the 4 base string
 *
 * */
void printNumberToFile(FILE *toPrintFile, int numberTocConvert, char *stringToPrint) {
    char base4String[LENGTH_OF_BASE4_WORD + 1] = {0};
    intToBase4(base4String, numberTocConvert);
    fprintf(toPrintFile, "%s%s", base4String, stringToPrint);
}


/**
 * readLineFromFile
 *
 * This function read from a file a line
 * The function will insert the line into a given string
 * If the file line is too long the function will return FAIL
 *
 * params:
 * file - the file to read from
 * buff - the string to insert the line to
 *
 * return:
 * int - if the line is valid or to long
 * 
 * */
int readLineFromFile(FILE *file, char buff[MAX_LINE+1]) {
    int ch;
    int counter = 0;
    ch = fgetc(file);
    while(ch != '\n' && !feof(file)) {
        if(counter < MAX_LINE) {
            buff[counter] = ch;
        }
        ch = fgetc(file);
        counter++;
    }
    if(counter > MAX_LINE + 1) {
        fprintf(stderr, "ERROR: line is to long, the maximum of a line is:%d\n", MAX_LINE);
        return FAIL;
    }
    buff[counter] = '\0';
    return SUCCESS;
}
