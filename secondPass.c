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
                success = handleSecondPassActionCommands(curr);
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


int handleSecondPassActionCommands(commandLinePtr actionCommandLine) {
    tokenPtr  commandToken = actionCommandLine->tokenListHead;
    tokenPtr curr = commandToken;
    int sourceOperandAddressingModeCode = 0;
    int destinyOperandAddressingModeCode = 0;
    int success = SUCCESS;

    IC++; /*for the memory word that discribe the command*/
    if(actionCommandLine->hasLabel) {
        commandToken = commandToken->next;
    }
    if(actionCommandLine->commandType <= TWO_OPERANDS) {
        curr = curr->next;
        switch (actionCommandLine->sourceOperandAddressingMode) {
            case IMMEDIATE_ADDRESSING:
                success = handleImmediateAddressing(curr);
                break;

            case DIRECT_ADDRESSING:
                success = handleDirectAddressing(curr);
                break;

        }
        curr = curr->next; /*go to the comma*/
    }
    if(actionCommandLine->commandType <= ONE_OPERAND) {
        curr = curr->next; /*from the comma or the command move to the destiny operand*/
        switch (actionCommandLine->destinyOperandAddressingMode) {
            case IMMEDIATE_ADDRESSING:
                success = handleImmediateAddressing(curr);
                break;

            case DIRECT_ADDRESSING:
                success = handleDirectAddressing(curr);
                break;

        }
    }
    return success;
}


int handleDirectAddressing(tokenPtr operand) {
    labelPtr operandLabel = checkLabelName(operand->string);
    return SUCCESS;
}


/**
 * handleImmediateAddressing
 *
 * This function handle operators that in immediate addressing method
 * The function will convert the number after the '#' to a real number
 * The function will check if the number is valid and then store the number in the action memory base
 * while using the two's complement method
 *
 * params:
 * operator - a pointer to the token of the operator
 *
 * return:
 * int - if the function failed or succeeded
 *
 * */
int handleImmediateAddressing(tokenPtr operator) {
    char *numberString = operator->string++;
    int realNumber;
    realNumber = atoi(numberString);
    /*if the number is to big or to small*/
    if(realNumber > MAX_VALID_IMMEDIATE_ADDRESSING_NUMBER || realNumber < MIN_VALID_IMMEDIATE_ADDRESSING_NUMBER) {
        fprintf(stderr, "ERROR: immediate addressing operators can only be between: %d to %d\n", MAX_VALID_IMMEDIATE_ADDRESSING_NUMBER, MIN_VALID_IMMEDIATE_ADDRESSING_NUMBER);
        return FAIL;
    }
    if(realNumber < 0) { /*two's complement system*/
        realNumber += pow(2,NUM_OF_BITS_IN_MEMORY_WORD-2); /*-2 because of the E R A bits*/
    }
    actionMemoryBase[IC-MEMORY_START_POS] = realNumber;
    IC++;
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
