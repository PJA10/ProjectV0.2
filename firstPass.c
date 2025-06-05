/*
 * mmn14
 * firstPass.c
 *
 * this file is in charge of the first pass.
 * in the first pass we go over each line and start to code it from assembly to the memmory base in the wierd 32 bit
 * this file handles action, commands, data commands lables and more.
 *
 * */

#include "firstPass.h"
#include "assistantFunctions.h"
#include "globalVars.h"
#include "prepareFiles.h"


/**
 * firstPass
 *
 * This if the main function of the first pass
 * This function is going over the file and handle every line
 * If there is a error in one of the file lines the function will return FAIL
 * The function check a line is valid, not to long
 * if it is then the function handle the line(read more in handleLine)
 *
 * params:
 * file - the input file to read from
 * secondPassCommandsHead - the empty second pass commandLine linked list to be filled
 *
 * return:
 * int - if there are errors in the user's code or not
 *
 * */
int firstPass(FILE *file, commandLinePtr *secondPassCommandsHead) {
    int isErrorsFlag = FALSE; /*a flag to point if there has been a error in the users code*/
    char buff[MAX_LINE + 1] = {0}; /*a buffer for the file line, 1 for the end of string sign*/
    int lineNumber = 0;
    int success = readLineFromFile(file, buff); /*get the next line*/

    /*run for every line in the file until EOF*/
    while(!feof(file)) {
        lineNumber++;
        if(success == FAIL){ /*if the line is to long, is does'nt matter if its a comment line*/
            printLineError(buff, lineNumber, NULL);
            isErrorsFlag = TRUE;
        }
        else { /*if the line is valid*/
            success = handleLine(buff, lineNumber, secondPassCommandsHead);
            if(success == FAIL) {
                isErrorsFlag = TRUE;
            }
        }
        success = readLineFromFile(file, buff); /*get the next line*/
    }
    if(isErrorsFlag == TRUE) {
        return FAIL;
    }
    return SUCCESS;
}


/**
 * handleLine
 *
 * This function handle a line from the input file
 * The function split the line to a linked list of tokens
 * If the line is empty or a comment line the function will do nothing
 * The function check if the line has a label
 * The function makes a commandLine struct of the line and handle it(read more, handleCommandLine)
 * The function return if there was an error in the line or not
 *
 * params:
 * lineString - the line in string
 * lineNumber - the number of the line
 * secondPassCommandsHead - a pointer to the head of the second pass commandLine linked list
 *
 * return:
 * int - if there was an error in the line or not
 *
 * */
int handleLine(char *lineString, int lineNumber, commandLinePtr *secondPassCommandsHead) {
    tokenPtr head = splitToTokens(lineString);
    tokenPtr curr = head;
    int success;
    commandLinePtr currLineCommandLine;
    int hasLabel;
    int commandType;

    printLog("\nline:\n");
    printLog(lineString);
    printLog("\nthe line token list is:\n");
    while(curr != NULL) {
        printLog(curr->string);
        printLog(" -> ");
        curr = curr->next;
    }
    curr = head;
    printLog("\n");
    if(handleNotCommandLines(head)) {
        freeTokenList(head);
        return SUCCESS;
    }
    hasLabel = checkIfHasValidLabel(head); /*check if there is a label at the start of the line*/
    if(hasLabel == FAIL) { /*if there is an unvalid label*/
        printLineError(lineString, lineNumber, head);
        return FAIL;
    }
    if (hasLabel == TRUE) { /*if there is a label, the command will be in the second token*/
        curr = curr->next;
        printLog("this line contain a label\n");
        if(curr == NULL) { /*if there isn't anything after the label(there is no command)*/
            fprintf(stderr, "ERROR: no command after label");
            printLineError(lineString, lineNumber, head);
            return FAIL;
        }
    }
    commandType = getCommandType(curr);
    currLineCommandLine = setNewCommandLine(lineNumber, commandType, hasLabel, FAIL, FAIL, head, lineString);

    success = handleCommandLine(currLineCommandLine, secondPassCommandsHead);
    return success;
}


/**
 * handleCommandLine
 *
 * This function handle command lines
 * If the line is a data command the line will code the data into the data memory base
 * If the line is an extern command the function will add the label as extern label
 * If the line is an action command the function will code the first memory word of the command
 * after checking the number of operands and their addressing mode the function will leave memory space for
 * the operands code
 * Additionally the function will add a the commandLine struct to the linked list of the action and entry commands
 * used by the second pass
 *
 * params:
 * commandLine - the commandLine struct of the current line
 * secondPassCommandsHead - a pointer to the head of the second pass commandLine linked list
 *
 * return:
 * int - if there is an error with the command parameters or not
 *
 * */
int handleCommandLine(commandLinePtr commandLine, commandLinePtr *secondPassCommandsHead) {
    int commandType = commandLine->commandType;
    int success = SUCCESS;
    tokenPtr commandToken = commandLine->tokenListHead;
    int gotAdded = FALSE;

    if(commandLine->hasLabel) /*if there is a lable*/
        commandToken = commandToken->next; /*then the command will be in the second token*/

    switch (commandType) {
        case DATA:
        case STRING:
        case STRUCT: /*if the line is a .data, .string or .struct command*/
            printLog("This is a data command\n");
            success = handleDataCommands(commandLine->tokenListHead, commandLine->hasLabel, commandType);
            break;
        case ENTRY: /*if the ine is a .entry command*/
            success = handleEntryCommand(commandLine); 
            if (success == FALSE) { /*if there are no errors and the entry label isn't all ready set*/
                addCommandLine(secondPassCommandsHead, commandLine); /*we will set flag the label in the second pass*/
                gotAdded = TRUE;
            }
            break;
        case EXTERN: /*if the line is a .extern command*/
            success = handleExternCommand(commandLine->tokenListHead, commandLine->hasLabel);
            break;
        case UNKNOWN: /*if the line isn't a valid command*/
            success = FAIL;
            fprintf(stderr, "ERROR: undefined instruction %s\n", commandToken->string);
            break;
        default: /*if the line is one of the acctions commands*/
            success = handleActionCommands(commandLine);
            if (success != FAIL) { /*if there are no errors in the operands*/
                addCommandLine(secondPassCommandsHead, commandLine); /*then we will code them in the second pass*/
                gotAdded = TRUE;
            }
            break;
    }

    if (success == FAIL) { /*if there was an error*/
        printLineError(commandLine->lineInString, commandLine->lineNumber, NULL);
        /*NULL because we will free head if needed in the next if*/
    }
    if(gotAdded == FALSE) {
        freeTokenList(commandLine->tokenListHead);
        free(commandLine);
    }
    return success;
}


/**
 * handleDataCommand
 *
 * This function handle the data command, .data .string and .struct
 * The function will add a new label if needed and will code the data in the data memory base
 *
 * params:
 * head - the pointer to the head of the token linked list
 * hasLabel - if the line has a label or not
 * commandType - the type of the command, suppose to be DATA or STRING or STRUCT
 *
 * */
int handleDataCommands(tokenPtr head, int hasLabel, int commandType) {
    tokenPtr commandToken = head;
    int success = SUCCESS;
    if(hasLabel) {
        commandToken = head->next; /*if there is a label then the second token should be the command*/
        if(handleLabel(head->string, DC, DATA_LABEL) == FAIL) {
            return FAIL;
        }
    }
    switch (commandType) {
        case DATA:
            success = handleDotData(commandToken);
            break;

        case STRING:
            success = handleDotString(commandToken);
            break;

        case STRUCT:
            success = handleDotStruct(commandToken);
            break;

        default:
            printLog("!!!!ERROR: in handleDataCommands when commandType isn't data, string or struck\n");
    }
    if(success == FAIL && hasLabel) {
        freeLastLabel(labelTable);
    }
    return success;
}


/**
 * handleDotData
 *
 * This function handle ".data" command
 * The function read the line into an array and update the data memory base
 * If there is a error with the command parameters the function will return FAIL
 *
 * params:
 * head - the token ptr to the command token
 *
 * return:
 * int - if the function failed or succeeded
 *
 * */
int handleDotData(tokenPtr commandToken) {
    int numbersArray[MAX_LINE];
    int i;

    for(i = 0; i < MAX_LINE; i++) { /*initialize numbersArray to not valid numbers*/
        numbersArray[i] = NOT_VALID_DATA_NUMBER;
    }
    /*get the numbers from the user*/
    if(analyzeGetArray(commandToken, numbersArray) == FAIL) {  /*and if there was an error*/
        return FAIL;
    }

    for(i = 0; numbersArray[i] != NOT_VALID_DATA_NUMBER; i++) { /*run over the numbers analyzeGetArray got to numbersArray*/
        addNumberToDataMemoryBase(numbersArray[i]);
    }
    return SUCCESS;
}


/**
 * handleDotString
 *
 * This function handle ".string" command
 * The function check if the command parameter is correct and if it is the function update the data memory base
 * If there is an error in the line the function will return FAIL
 *
 * params:
 * commandToken - the token of the command ".string" in the line
 *
 * return:
 * int - if the function failed or succeeded
 *
 * */
int handleDotString(tokenPtr commandToken) {
    char *input; /*a pointer to char that will point to the token's string*/
    tokenPtr curr = commandToken->next;

    if(checkStringToken(curr) == FAIL) {
        return FAIL;
    }
    input = (curr)->string;
    storeString(input);
    return SUCCESS;
}


/**
 * handleDotStruct
 *
 * This function handle ".struct" command
 * The function check if the command parameters are correct and if they are, it updates the data memory base
 * If there is an error in the line the function will return FAIL
 *
 * params:
 * commandToken - the token of the command ".struct"
 *
 * return:
 * int - if the function failed or succeeded
 *
 * */
int handleDotStruct(tokenPtr commandToken) {
    tokenPtr curr = commandToken->next;
    int structNumber;
    char *strucrString;
    if(!curr) {
        fprintf(stderr, "ERROR: expected after .struct a number then , then a string\n");
        return FAIL;
    }
    if(!isNumber(curr->string)) {
        fprintf(stderr, "ERROR: expected after .struct command a real number\n");
        return FAIL;
    }
    structNumber = atoi(curr->string);
    if(structNumber > MAX_VALID_DATA_NUMBER || structNumber < MIN_VALID_DATA_NUMBER) { /*if the number is to big or to small*/
        fprintf(stderr, "ERROR: struct number must be between: %d to %d\n", MAX_VALID_DATA_NUMBER, MIN_VALID_DATA_NUMBER);
        return FAIL;
    }
    curr = curr->next; /*now this is suppose to be a comma token*/
    if(!curr) {
        fprintf(stderr, "ERROR: expected after the number a , then a string\n");
        return FAIL;
    }
    if(strcmp(curr->string, ",") != 0) { /*if the curr string isn't ","*/
        fprintf(stderr, "ERROR: expected after struct number a comma\n");
        return FAIL;
    }
    curr = curr->next;
    if(checkStringToken(curr) == FAIL) {
        return FAIL;
    }
    strucrString = curr->string;
    addNumberToDataMemoryBase(structNumber);
    storeString(strucrString);
    return SUCCESS;
}


/**
 * handleExternCommand
 *
 * This function handle .extern command while in the first pass
 * The funtion will check if there is 1 parameter that is a valid label name
 * If it is the fucntion will add the label to the label table as external
 *
 * params:
 * head - the head of the line token linked list
 * hasLabel - If there is a label in the start of the line or not
 * commandType - The command Type, can be ENTRY or EXTERN
 *
 * return:
 * int - if the function failed or succeeded
 *
 * */
int handleExternCommand(tokenPtr head, int hasLabel ) {
    /*we use else for the new block to declare vars*/
    tokenPtr commandToken = head;
    tokenPtr curr;
    labelPtr existsLabel;

    if (hasLabel) {
        commandToken = commandToken->next; /*if there are a label then the command token is second in the line*/
        /*we do nothing more with the lable, we ignore labels in .extern and .entry*/
    }
    curr = commandToken->next; /*where there is suppose to be a name for the label*/
    if(curr == NULL) {
        fprintf(stderr, "ERROR: expected after .extern a label name\n");
        return FAIL;
    }
    if(checkIfValidLabelName(curr->string, TRUE) == FAIL) {
        return FAIL;
    }
    if(curr->next != NULL) {
        fprintf(stderr, "ERROR: expected end of line after %s\n", curr->string);
        return FAIL;
    }
    existsLabel = checkLabelName(curr->string);
    if(existsLabel!= NULL && existsLabel->type == EXTERN) { /*if the label had all ready been extern*/
        return SUCCESS; /*it is ok to declare the same external more than once*/
    }

	if(handleLabel(curr->string, 0, EXTERN_LABEL) == FAIL) {
		return FAIL;
	}
    return SUCCESS;
}


/**
 * handleActionCommand
 *
 * This function handle action command while in the first pass
 * The funtion will check if there are enough parameters, commas and valid addressing modes
 * the function returns fail if the command is not valid and success if the command is valid
 *
 * params:
 * head - the head of the line token linked list
 * isLabel - If there is a label in the start of the line or not
 * commandType - The command Type, can be all of the 16 action commands
 *
 * return:
 * int - if the function failed or succeeded
 *
 * */
int handleActionCommands(commandLinePtr actionCommandLine){
    tokenPtr commandToken = actionCommandLine->tokenListHead;
    int commandType = actionCommandLine->commandType;
    if(actionCommandLine->hasLabel){
        /*if there is a label then the second token should be the command*/
        commandToken = commandToken->next;

        if(handleLabel(actionCommandLine->tokenListHead->string, IC, ACTION_LABEL) == FAIL) {
            return FAIL;
        }

    }
    if(commandType <= TWO_OPERANDS){
        if(handleTwoOperands(actionCommandLine, commandToken->next) == FAIL) { /*if there is an error with operands*/
            return FAIL;
        }
    }
    else if(commandType<=ONE_OPERAND){ /*checks if the command requires one operand*/
        if(handleOperand(actionCommandLine, commandToken->next, DESTINY) == FAIL) {/*if there is an error with operand*/
            return FAIL;
        }
    }
    else if(commandToken -> next != NULL){ /*if we are here, the command requires zero operands so we check for extra operands*/
        fprintf(stderr, "ERROR: expected end of line after %s\n", actionCommandsArray[commandType].name);
        return FAIL;
    }

    /*destinyOperandAddressingMode, sourceOperandAddressingMode are updated in handleOperand*/
    codeActionCommand(actionCommandLine->destinyOperandAddressingMode, actionCommandLine->sourceOperandAddressingMode, actionCommandLine->commandType);
    return SUCCESS; /* if we are here, there are no errors*/
}


/**
 * handleEntryCommand
 *
 * The function handle .entry command while in the first pass
 * The function will check that the command have one parameter
 * The function will check that there haven't been an external label with the same name
 * If the label have been all ready has been set the function will flag it
 * The function will return if it found the label or nor, and FAIL for error
 *
 * params:
 * entryCommandLine - The commandLine struct of the .entry line
 *
 * return:
 * int - if the function failed, found the label or not
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
    if(currToken->next != NULL) { /*if the parameter isn't the end of the line*/
        fprintf(stderr, "ERROR: expected end of line after first parameter of .entry\n");
        return FAIL;
    }
    label = checkLabelName(currToken->string); /*find a label with this name*/
    if(label != NULL ) { /*if there is a label with this name*/
        if (label->type == EXTERN_LABEL) { /*if the label is an extern label*/
            fprintf(stderr, "ERROR: .entry label cant be an extern label\n");
            return FAIL;
        }
        label->hasEntry = TRUE; /*if the label is'nt extern then we can flag it now and not pass it to the second pass*/
        return TRUE; /*return that we found and flag the label*/
    }
    return FALSE;
}
