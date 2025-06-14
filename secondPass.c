/*
 * mmn14
 * secondPass.c
 *
 * this file runs after the first pass.
 * in the second pass the functions doesnt run on the original input file, they run on a linked list that was made in the first pass
 * The file code actions operands and flag the labels with .entry command
 *
 * */

#include "secondPass.h"
#include "globalVars.h"
#include "assistantFunctions.h"


/**
 * secondPass
 *
 * This function run over a linked list with every action and entry command
 * The function code actions operands and flag the labels with .entry command
 * The function runs after first pass and and because of that our label table are now completed
 * and we can do the thing that were mentioned
 *
 * params:
 * secondPassCommandsHead - the linked list of commandLine with the action and entry commands
 * externReferencesHead - a empty linked list of the external labels references to be filled
 *
 * return:
 * int - if there is a error in the user's code
 *
 * */
int secondPass(commandLinePtr secondPassCommandsHead, externReferencePtr *externReferencesHead) {
    commandLinePtr curr = secondPassCommandsHead;
    int isErrorsFlag = FALSE;
    IC = MEMORY_START_POS; /*we run over all the memory again*/
    while(curr) { /*run over te commandLine linked list*/
        int success = SUCCESS;
        switch (curr->commandType) {
            case ENTRY:
                success = handleSecondPassEntryCommand(curr); /*if the current command is .entry*/
                break;

            default: /*if the current command is an action*/
                success = handleSecondPassActionCommands(curr, externReferencesHead);
                break;
        }
        if(success == FAIL) { /*if there was an error in the command*/
            fprintf(stderr, "in line number: %d\n%s\n\n", curr->lineNumber, curr->lineInString);
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
 * handleSecondPassActionCommands
 *
 * This function handle command while in the second pass
 * The function code the command operands as needed
 * If there is an error with the command operands the function will return FAIL
 *
 * params:
 * actionCommandLine - the commandLine of the action command
 * externReferencesHead - a linked list of every external label reference
 *
 * return:
 * int - if there is a error in the user's operands
 *
 * */
int handleSecondPassActionCommands(commandLinePtr actionCommandLine, externReferencePtr *externReferencesHead) {
    tokenPtr  commandToken = actionCommandLine->tokenListHead;
    tokenPtr curr;
    int sourceOperandAddressingModeCode = actionCommandLine->sourceOperandAddressingMode;
    int destinationOperandAddressingModeCode = actionCommandLine->destenationOperandAddressingMode;
    int success = SUCCESS;
    int commandType = actionCommandLine->commandType;

    IC++; /*for the memory word that discribe the command*/
    if(actionCommandLine->hasLabel) {
        commandToken = commandToken->next;
    }
    curr = commandToken;
    if(commandType <= TWO_OPERANDS) {
        curr = curr->next;
        success = codeOperand(sourceOperandAddressingModeCode, curr, SOURCE, externReferencesHead);
        if(success == FAIL) {
            return FAIL;
        }

        if(sourceOperandAddressingModeCode == REGISTERS_ADDRESSING &&
           destinationOperandAddressingModeCode == REGISTERS_ADDRESSING) {
            IC--;
        }
        curr = curr->next; /*go to the comma*/
    }
    if(commandType <= ONE_OPERAND) {
        curr = curr->next; /*from the comma or the command move to the destination operand*/
        success = codeOperand(destinationOperandAddressingModeCode, curr, DESTINATION, externReferencesHead);
        if(success == FAIL) {
            return FAIL;
        }
    }
    return success;
}


/**
 * codeOperand
 *
 * The function code a given operand to the action memory base
 *
 * params:
 * operandAddressingMode - the operand addressing mode
 * operandToken - the token of the operand
 * whatOperand - which operand is it, source or destination
 * externReferencesHead - a linked list of every external label reference
 *
 * return:
 * int - if there is an error in the operand or not
 *
 * */
int codeOperand(int operandAddressingMode, tokenPtr operandToken, int whatOperand, externReferencePtr *externReferencesHead) {
    int success = SUCCESS;

    switch (operandAddressingMode) {
        case IMMEDIATE_ADDRESSING:
            success = handleImmediateAddressing(operandToken);
            break;

        case DIRECT_ADDRESSING:
            success = handleDirectAddressing(operandToken->string, externReferencesHead);
            break;

        case MATRIX_ACCESS_ADRESSING:
            success = handleMatrixAddressing(operandToken, externReferencesHead);
            break;

        case REGISTERS_ADDRESSING:
            success = handleRegisterAddressing(operandToken, whatOperand);
            break;
        
        default:
            return FAIL;
    }

    return success;
}


/**
 * handleRegisterAddressing
 *
 * This function handle operand that in register addressing method
 * The function will add the register number to the memory in the right bits in the memory word with dependence
 * with which operand it is
 *
 * params:
 * operand - a pointer to the operand token
 * whatOperand - which operand is it, source or destination
 *
 * return:
 * int - if the function failed or succeeded
 *
 * */
int handleRegisterAddressing(tokenPtr operand, int whatOperand) {
    int registerNumber;
    int binaryRegisterNum;

    registerNumber = operand->string[1] - '0'; /*the second char in the string will be the register number, right after the 'r'*/
    /*minus '0' to convert to int, '1' - 1, '2' - 2 and so on*/
    if(whatOperand == DESTINATION) { /*if this is the destination operand*/
        binaryRegisterNum = registerNumber << NUM_OF_CODE_METHOD_BITS;
    }
    else{ /*if this is the source operand*/
        binaryRegisterNum = (registerNumber << NUM_OF_CODE_METHOD_BITS) << NUM_OF_REGISTER_NUMBER_BITS;
    }
    binaryRegisterNum = binaryRegisterNum | ABSOLUTE_CODE_METHOD_MASK; /*just in case the mask will change from 0*/
    actionMemoryBase[IC-MEMORY_START_POS] += binaryRegisterNum; /*add to the memory*/
    IC++; /*update the counter*/
    return SUCCESS;
}


/**
 * handleMatrixAddressing
 *
 * The function handle operand that are in matrix access addressing method
 * The function will handle the label of the matrix just like a direct addressing and add it's address to the memory
 * After the label's address the function will add a word to memory with the row and column registers
 * The row register will be in bits 6-9 and the col register will be in bits 2-5, bits 0-1 will represent absolute coding method
 *
 * params:
 * operand - a pointer to the operand's token
 * externReferencesHead - a linked list of every external label reference
 *
 * return:
 * int - if the function failed or succeeded
 *
 * */
int handleMatrixAddressing(tokenPtr operand, externReferencePtr *externReferencesHead) {
    int success = SUCCESS;
    int rowRegisterNumber = 0, binaryRowRegister = 0;
    int binaryColRegister = 0, colRegisterNumber = 0;
    int operand_string_length = 0;
    char matrixLabelSubstring[MAX_LENGTH_OF_LABEL_NAME + 1] = {0};
    operand_string_length = strlen(operand->string);

    /* we know that (operand_string_length - MATRIX_ADDRESSING_SUFFIX_LENGTH) is smaller then MAX_LENGTH_OF_LABEL_NAME
    because this operator validated as matrix addressing operand so it has a valid label with a matrix suffix */
    strncpy(matrixLabelSubstring, operand->string, operand_string_length - MATRIX_ADDRESSING_SUFFIX_LENGTH);

    success = handleDirectAddressing(matrixLabelSubstring, externReferencesHead);
    if(success == FAIL) {
        return FAIL;
    }

    /*minus '0' to convert to int, '1' -> 1, '2' -> 2 and so on*/
    rowRegisterNumber = operand->string[operand_string_length-6] - '0';
    colRegisterNumber = operand->string[operand_string_length-2] - '0';

    binaryRowRegister = (rowRegisterNumber << NUM_OF_CODE_METHOD_BITS) << NUM_OF_REGISTER_NUMBER_BITS;
    binaryColRegister = colRegisterNumber << NUM_OF_CODE_METHOD_BITS;

    actionMemoryBase[IC-MEMORY_START_POS] = binaryRowRegister | binaryColRegister | ABSOLUTE_CODE_METHOD_MASK;
    IC++; /*update the counter*/

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
 * operand_str - the operan string
 * externReferencesHead - a linked list of every external label reference
 *
 * return:
 * int - if the function failed or succeeded
 *
 * */
int handleDirectAddressing(char *operand_str, externReferencePtr *externReferencesHead) {
    /*check if there is a label with the name of the operand string*/
    labelPtr operandLabel = findLabel(operand_str);

    if(operandLabel == NULL) { /*if there isn't a label with the name of the operand string*/
        fprintf(stderr, "ERROR: no label with the name %s are defined\n", operand_str);
        return FAIL;
    }

    addAddressToActionMemoryBase(operandLabel, externReferencesHead);

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
    int realNumber;
    char *numberString = operand->string;
    numberString++;
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
 * handleSecondPassEntryCommand
 *
 * The function handle .entry command while in the second pass
 * The function will search for the entry label
 * If it is the function will flag the entry label parameter as entry
 *
 * params:
 * entryCommandLine - The commandLine struct of the .entry line
 *
 * return:
 * int - if the function failed or succeeded
 *
 * */
int handleSecondPassEntryCommand(commandLinePtr entryCommandLine) {
    labelPtr label;
    tokenPtr currToken;
    tokenPtr commandToken = entryCommandLine->tokenListHead;
    if(entryCommandLine->hasLabel == TRUE) {
        commandToken = commandToken->next; /*if there is a label then the command is the second token and not the first*/
    }
    currToken = commandToken;
    currToken = currToken->next;
    label = findLabel(currToken->string);
    if(label == NULL) { /*if there is no label with the given name*/
        fprintf(stderr, "ERROR: .entry parameter must be a existing label\n");
        return FAIL;
    }
    if(label->type == EXTERN_LABEL) { /*if the given label is an extern label*/
        fprintf(stderr, "ERROR: .entry label cant be an extern label\n");
        return FAIL;
    }
    label->hasEntry = TRUE;
    return SUCCESS;
}
