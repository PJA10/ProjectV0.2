/*
 * Created by PJA on 3/5/2018.
*/

#include "secondPass.h"
#include "globalVars.h"
#include "assistantFunctions.h"


/**
 * */
int secondPass(commandLinePtr secondPassCommandsHead) {
    commandLinePtr curr = secondPassCommandsHead;
    int isErrorsFlag = FALSE;
    while(curr) {
        int success = SUCCESS;
        switch (curr->commandType) {
            case ENTRY:
                success = handleEntryCommand(curr);
                break;

            default:

                break;
        }
        if(success == FAIL) {
            fprintf(stderr, "in line number: %d\n", curr->lineNumber);
            isErrorsFlag = TRUE;
        }
        curr = curr->next;
    }
    if(isErrorsFlag) {
        return FAIL;
    }
    return SUCCESS;
}


/**
 * handleEntryCommand
 *
 * The function handle .entry command while in the second pass
 * The function will check that the command parameter is correct
 * If it is the function will flag the entry label parameter as entry
 *
 * params:
 * entryCommandLine - The commandLine struct of the .entry line
 *
 * return:
 * int - if the function failed or succeeded
 *
 * */
int handleEntryCommand(commandLinePtr entryCommandLine) {
    labelPtr label;
    tokenPtr currToken;
    tokenPtr commandToken = entryCommandLine->tokenListHead;
    if(entryCommandLine->hasLabel == TRUE) {
        commandToken = commandToken->next; /*if there is a label then the command is the second token and not the first*/
    }
    currToken = commandToken;
    currToken = currToken->next;
    if(currToken == NULL) { /*if te command is the end of the line*/
        fprintf(stderr, "ERROR: expected label after .entry\n");
        return FAIL;
    }
    label = checkLabelName(currToken->string);
    if(label == NULL) { /*if there is no label with the given name*/
        fprintf(stderr, "ERROR: .entry parameter must be a existing label\n");
        return FAIL;
    }
    if(label->isExtarnal == TRUE) { /*if the given label is an extern label*/
        fprintf(stderr, "ERROR: .entry label cant be an extern label\n");
        return FAIL;
    }
    if(currToken->next != NULL) { /*if the parameter isn't the end of the line*/
        fprintf(stderr, "ERROR: expected end of line after first parameter of .entry\n");
        return FAIL;
    }
    label->hasEntry = TRUE;
    return SUCCESS;
}
