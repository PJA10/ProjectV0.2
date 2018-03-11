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
    tokenPtr curr;
    int sourceOperandAddressingModeCode = 0;
    int destinyOperandAddressingModeCode = 0;
    int success = SUCCESS;

    IC++; /*for the memory word that discribe the command*/
    if(actionCommandLine->hasLabel) {
        commandToken = commandToken->next;
    }
    curr = commandToken;
    if(actionCommandLine->commandType <= TWO_OPERANDS) {
        curr = curr->next;
        switch (actionCommandLine->sourceOperandAddressingMode) {
            case IMMEDIATE_ADDRESSING:
                success = handleImmediateAddressing(curr);
                break;

            case DIRECT_ADDRESSING:
                success = handleDirectAddressing(curr);
                break;

            case STRUCT_ACCESS_ADRESSING:
                success = handleStructAddressing(curr);
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

            case STRUCT_ACCESS_ADRESSING:
                success = handleStructAddressing(curr);
                break;
        }
    }
    return success;
}


/**
 * handleStructAddressing
 *
 * The function handle operand that are in struct addressing method
 * The function will search for a struct label with the name of the operand string until the dot
 * And add to the memory the address of the label
 * Then the function will add to the memory the number of the field the operand use
 *
 * params:
 * operand - a pointer to the operand's token
 *
 * return:
 * int - if the function failed or succeeded
 *
 * */
int handleStructAddressing(tokenPtr operand) {
    int fieldNum;
    int i;
    labelPtr structLabel;
    char *labelName = (char *) calloc(sizeof(operand->string), sizeof(char)); /*a string fot the label's name*/
    checkFail(labelName);
    for(i = 0; i < strlen(operand->string); i++) { /*run over the operand string*/
        if(operand->string[i] == '.') { /*until we get to te dot*/
            labelName[i] = '\0'; /*mark end of string in the label name*/
            break;
        }
        labelName[i] = operand->string[i]; /*copy the current char from the operand string to the label name*/
    }
    /*i is the location of the dot*/
    fieldNum = operand->string[i+1] - '0'; /*convert to int, '1' to 1 and '2' to 2*/
    structLabel = checkLabelName(labelName);
    if(structLabel == NULL) { /*if there is no label's with this name*/
        fprintf(stderr, "ERROR: no struct label with the name of %s\n", labelName);
        free(labelName);
        return FAIL;
    }
    free(labelName); /*we don't need labelName anymore*/

    if(structLabel->type != STRUCT_LABEL) { /*if the label with the correct name isn't a struct label*/
        fprintf(stderr, "ERROR: the label %s isn't a struct label\n", structLabel->name);
        return FAIL;
    }
    addAddressToActionMemoryBase(structLabel); /*add the label address to the memory*/
    addNumberToActionMemoryBase(fieldNum); /*followed by the filed's number*/
    return SUCCESS;
}






/**
 * handleDirectAddressing
 *
 * This function handle operands that are in direct addressing method
 * The function will find the label with name of the operand string
 * And add to the memory a word with the address and code method
 * If there isn't a label with that name the function will return FAIL
 *
 * parameters:
 * operand - a pointer to the token of the operand
 *
 * return:
 * int - if the function failed or succeeded
 *
 * */
int handleDirectAddressing(tokenPtr operand) {
    int operandMemoryWord = 0;
    int codeMethodBits;
    /*check if there is a label with the name of the operand string*/
    labelPtr operandLabel = checkLabelName(operand->string);

    if(operandLabel == NULL) { /*if there isn't a label with the name of the operand string*/
        fprintf(stderr, "ERROR: no label with the name %s are defined\n", operand->string);
        return FAIL;
    }
    /*shift the address twice to the left for the code method bits*/
    addAddressToActionMemoryBase(operandLabel);
    return SUCCESS;
}


/**
 * handleImmediateAddressing
 *
 * This function handle operands that are in immediate addressing method
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
int handleImmediateAddressing(tokenPtr operand) {
    char *numberString = operand->string++;
    int realNumber;
    int operandMemoryWord;
    realNumber = atoi(numberString);
    /*if the number is to big or to small*/
    if(realNumber > MAX_VALID_IMMEDIATE_ADDRESSING_NUMBER || realNumber < MIN_VALID_IMMEDIATE_ADDRESSING_NUMBER) {
        fprintf(stderr, "ERROR: immediate addressing operators can only be between: %d to %d\n", MAX_VALID_IMMEDIATE_ADDRESSING_NUMBER, MIN_VALID_IMMEDIATE_ADDRESSING_NUMBER);
        return FAIL;
    }
    addNumberToActionMemoryBase(realNumber);
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
    if(label->type == EXTERN_LABEL) { /*if the given label is an extern label*/
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
