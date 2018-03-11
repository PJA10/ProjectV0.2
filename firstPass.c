/*
 * Created by PJA on 3/5/2018.
*/

#include "firstPass.h"
#include "assistantFunctions.h"
#include "globalVars.h"

/**
 * */
int firstPass(FILE *file, commandLinePtr *secondPassCommandsHead) {
    int isErrorsFlag = FALSE; /*a flag to point if there has been a error in the users code*/
    char buff[MAX_LINE] = {0}; /*a buffer for the file line*/
    tokenPtr head;
    tokenPtr curr;
    int hasLabel;
    int commandType;
    int lineNumber = 0;
    int success = SUCCESS;

    fgets(buff, MAX_LINE, file); /*get the next line*/
    /*run for every line in the file until EOF*/
    while(!feof(file)) {
        commandLinePtr currLineCommandLine;
        int needToFree = TRUE;
        lineNumber++;
        head = splitToTokens(buff);
        curr = head;
        printLog("\nline:\n");
        printLog(buff);
        printLog("the line token list is:\n");
        while(curr) {
            printLog(curr->string);
            printLog(" -> ");
            curr = curr->next;
        }
        printLog("\n");

        if(head == NULL){ /*if there is no text in the line*/
            printLog("the line is empty\n");
        }
        else if(head->string[FIRST_ELEMENT] == ';') { /*if the line starts with a ';'*/
            freeTokenList(head);
            printLog("the line is a comment\n");
        }
        else if((hasLabel = checkIfHasValidLabel(head)) == FAIL) {
            fprintf(stderr, "in line number: %d\n%s\n", lineNumber, buff);
            freeTokenList(head);
            isErrorsFlag = TRUE;
        }
        else {
            curr = head;
            if (hasLabel == TRUE) { /*if there is a label, the command will be in the second token*/
                curr = curr->next;
                printLog("this line contain a label\n");
            }
            commandType = getCommandType(curr);

            printLog("the line is of the type: ");
            printLog(commands[commandType].name);
            printLog("\n");

            switch (commandType) {
                case DATA:
                case STRING:
                case STRUCT:
                    printLog("This is a data command\n");
                    success = handleDataCommands(head, hasLabel, commandType);
                    break;

                case ENTRY:
                    currLineCommandLine = setNewCommandLine(lineNumber, commandType, hasLabel, FAIL, FAIL, head);
                    addCommandLine(secondPassCommandsHead, currLineCommandLine);
                    needToFree = FALSE;
                    break;

                case EXTERN:
                    success = handleExternCommand(head, hasLabel, commandType);
                    break;

                case UNKNOWN:
                    fprintf(stderr, "ERROR: undefined instruction %s\n", curr->string);
                    break;

                default:
                    currLineCommandLine = setNewCommandLine(lineNumber, commandType, hasLabel, FAIL, FAIL, head);
                    success = handleActionCommands(currLineCommandLine);
                    if(success == FAIL) {
                        free(currLineCommandLine);
                    }
                    else {
                        addCommandLine(secondPassCommandsHead, currLineCommandLine);
                        needToFree = FALSE;
                    }
                    break;
            }

            if (success == FAIL) { /*if there was an error*/
                fprintf(stderr, "in line number: %d\n%s\n", lineNumber, buff);
                isErrorsFlag = TRUE;
            }
            if(needToFree) {
                freeTokenList(head);
            }
        }
        fgets(buff, MAX_LINE, file); /*get the next line*/
    }
    if(isErrorsFlag == TRUE) {
        return FAIL;
    }
    return SUCCESS;
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
    int success = SUCCESS, i;
    if(hasLabel) {
        int labelType = DATA_LABEL;
        labelPtr newLabel = (labelPtr) calloc(sizeof(label), 1);
        char *name = (char *) calloc(sizeof(char), MAX_LENGTH_OF_LABEL_NAME + 2); /*1 for ':' and 1 for '\0' at the end*/
        checkFail(newLabel);
        checkFail(name);
        commandToken = head->next; /*if there is a label then the second token should be the command*/

        /*set new and add it to the list*/
        strcpy(name, head->string);
        name[strlen(name)-1] = '\0'; /*delete the ':' at the end of the name*/

        if(commandType == STRUCT) {
            labelType = STRUCT_LABEL;
        }
        setLabel(newLabel, name, DC, labelType, FALSE);
        if(addLabel(&labelTabale, newLabel) == FAIL)
            return FAIL;
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
    for(i = 0; i <MAX_LINE; i++) {
        fprintf(stderr, "%d ", dataMemoryBase[i]);
    }
    fprintf(stderr, "\n");
    if(success == FAIL && hasLabel) {
        freeLastLabel(labelTabale);
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
int handleExternCommand(tokenPtr head, int hasLabel, int commandType) {
    /*we use else for the new block to declare vars*/
    tokenPtr commandToken = head;
    tokenPtr curr;
    labelPtr newLabel;
    char *name;
    if (hasLabel) {
        commandToken = commandToken->next; /*if there are a label then the command token is second in the line*/
        /*we do nothing more with the lable, we ignore labels in .extern and .entry*/
    }
    curr = commandToken->next; /*where there is suppose to be a name for the label*/
    if(curr == NULL) {
        fprintf(stderr, "ERROR: expected after .extern a label name\n");
        return FAIL;
    }
    if(checkIfValidLabelName(curr->string) == FAIL) {
        return FAIL;
    }
    if(curr->next != NULL) {
        fprintf(stderr, "ERROR: expected end of line after %s\n", curr->string);
        return FAIL;
    }
    newLabel = (labelPtr) calloc(sizeof(label), 1);
    name = (char *) calloc(sizeof(char), MAX_LENGTH_OF_LABEL_NAME + 1); /*1 for '\0' at the end*/
    checkFail(newLabel);
    checkFail(name);
    /*set new and add it to the list*/
    strcpy(name, curr->string);
    setLabel(newLabel, name, 0, EXTERN_LABEL, FALSE);
    if(addLabel(&labelTabale, newLabel) == FAIL)
        return FAIL;
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
        labelPtr newLabel = (labelPtr) calloc(1, sizeof(label));
        char *name = (char *) calloc( MAX_LENGTH_OF_LABEL_NAME + 2, sizeof(char)); /*1 for ':' and 1 for '\0' at the end*/
        checkFail(newLabel);
        checkFail(name);
        /*if there is a label then the second token should be the command*/
        commandToken = commandToken->next;
        /*set new and add it to the list*/
        strcpy(name, actionCommandLine->tokenListHead->string);
        name[strlen(name)-1] = '\0'; /*delete the ':' at the end of the name*/
        setLabel(newLabel, name, IC, ACTION_LABEL, FALSE);
        if(!addLabel(&labelTabale, newLabel))
            return FAIL;
    }

    if(commandType <= TWO_OPERANDS || commandType == LEA){ /*checks if the command requires two operand*/
        int sourceOperandAddressingMode;
        int destinyOperandAddressingMode;
        int isValid;
        tokenPtr firstOperandToken = commandToken->next; /*after the command suppose to be the first operand*/
        tokenPtr commaToken;
        tokenPtr secondOperandToken;

        if(firstOperandToken == NULL) { /*checks if the first operand is NULL*/
            fprintf(stderr, "ERROR: the Command %s requires two operands\n", commands[commandType].name);
            return FAIL;
        }

         commaToken = firstOperandToken->next; /*between the two operand suppose to be a comma*/

        if(commaToken == NULL) { /*checks if the there is nothing after the first operand*/
            fprintf(stderr, "ERROR: the Command %s requires two operands\n", commands[commandType].name);
            return FAIL;
        }
        if(strcmp(commaToken->string, ",")) { /*checks if there is a comma*/
            fprintf(stderr, "ERROR: missing comma\n");
            return FAIL;
        }

        secondOperandToken = commaToken->next; /*after the comma suppose to be the second operand*/

        if(secondOperandToken == NULL) { /*checks if there is a second operand*/
            fprintf(stderr, "ERROR: the second operand should come after the comma\n");
            return FAIL;
        }
        if(secondOperandToken->next != NULL) { /* checks if there are too many operands*/
            fprintf(stderr, "ERROR: too many operands\n");
            return FAIL;
        }

        sourceOperandAddressingMode = checkAddressingMode(firstOperandToken);
        isValid = checkIfValidAddressingMode(sourceOperandAddressingMode ,commands[commandType].sourceOperandValidAddressingModes);
        if(isValid == FAIL){ /* checks if the addressing mode of the first operand is valid*/
            fprintf(stderr, "ERROR: the command %s doesnt accept %s for the first operand\n", commands[commandType].name, addressingModes[sourceOperandAddressingMode]);
            return FAIL;
        }

        destinyOperandAddressingMode = checkAddressingMode(secondOperandToken);
        isValid = checkIfValidAddressingMode(destinyOperandAddressingMode, commands[commandType].destinyOperandValidAddressingModes);
        if(isValid == FAIL){ /*checks if the addressing mode of the second operand is valid*/
            fprintf(stderr, "ERROR: the command %s doesnt accept %s for the second operand\n", commands[commandType].name, addressingModes[destinyOperandAddressingMode]);
            return FAIL;
        }

        /*update actionCommandLine with the now known addressing modes*/
        actionCommandLine->sourceOperandAddressingMode = sourceOperandAddressingMode;
        actionCommandLine->destinyOperandAddressingMode = destinyOperandAddressingMode;
    }
    else if(commandType<=ONE_OPERAND && commandType!=LEA){ /*checks if the command requires one operand*/
        int destinyOperandAddressingMode;
        int isValid;
        tokenPtr firstOperandToken = commandToken->next; /*after the command suppose to be the first operand*/

        if(firstOperandToken == NULL) { /*checks if the first operand is NULL*/
            fprintf(stderr, "ERROR: the command %s requires one operand, got zero\n", commands[commandType].name);
            return FAIL;
        }
        if(firstOperandToken->next != NULL) { /*checks if there are too many operands*/
            fprintf(stderr, "ERROR: too many operands\n");
            return FAIL;
        }
        destinyOperandAddressingMode = checkAddressingMode(firstOperandToken);
        isValid = checkIfValidAddressingMode(destinyOperandAddressingMode ,commands[commandType].destinyOperandValidAddressingModes);
        if(isValid == FAIL){ /*checks if the addressing mode of the only operand is valid*/
            fprintf(stderr, "ERROR: the command %s doesnt accept %s for the operand\n", commands[commandType].name, addressingModes[destinyOperandAddressingMode]);
            return FAIL;
        }

        /*update actionCommandLine with the now known addressing mode*/
        actionCommandLine->destinyOperandAddressingMode = destinyOperandAddressingMode;
    }
    else if(commandToken -> next != NULL){ /*if we are here, the command requires zero operands so we check for extra operands*/
        fprintf(stderr, "ERROR: expected end of line after %s\n", commands[commandType].name);
        return FAIL;
    }
    codeActionCommand(actionCommandLine->destinyOperandAddressingMode, actionCommandLine->sourceOperandAddressingMode, actionCommandLine->commandType-FIRST_COMMAND_ID);
    return SUCCESS; /* if we are here, there are no errors*/
}

