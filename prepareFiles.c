/*
 * Created by PJA on 3/5/2018.
*/

#include "prepareFiles.h"

void prepareFiles(){

    return;
}

/*
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
        printf("Error, with geting file %s with mode %s\n", fileName, mode); /*print that there was a problem*/
        return NULL;
    }
    return  file;
}