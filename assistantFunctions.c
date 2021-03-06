/*
 * mmn14
 * assistantFunctions.c
 * Alon Wartski - 209729193 and Roy Lenz - 322585944
 *
 * In this file we have a lot of functions that dont relate to a spesific action.
 * like connecting strings, getting the command type and more
 *
 * */


#include "assistantFunctions.h"
#include "globalVars.h"


/**
 * concat
 *
 * The function concatenates two string to one
 * The function will return a dynamic pointer to th new string
 *
 * params:
 * s1 - the first part of the contacted string
 * s2 - the second part of the contacted string
 *
 * return:
 * char* - a dynamic pointer to the new string
 *
 * */
char* concat(char *s1, char *s2) {
    char *result = (char *) calloc(strlen(s1)+strlen(s2)+1, sizeof(char)); /*+1 for the null-terminator */
    checkFail(result);
    strcpy(result, s1); /*copy s1 into result*/
    strcat(result, s2); /*copy s2 to the end of result(currently the end of s1) */
    return result;
}


/**
 * printLog
 *
 * This function is a log function. every debug or not specified in the assignment print we want to make,
 * will print from this function.
 * The printing is dependent if the flag LogIsOn, in that way we can turn the log on and off as we want
 * The project checker may change the flag as he wish
 *
 * params:
 * string - The string we want to log
 *
 * */
void printLog(char *string) {
    if(logIsOn) {
        fprintf(stderr, "%s", string);
    }
}


/**
 * splitToTokens
 *
 * The function split the line it get as a parameter to a liked list of tokens.
 * Every token in the linked list can be 2 things, a comma or just a string.
 * A line will be split by spaces and commas. please notice that even that a comma will separate the line, there will
 * be a node just for the comma alone.
 *
 * params:
 * char *line - the line that need to be split
 *
 * return:
 * tokenPtr - a pointer to the head token of the now created linked list
 *
 * */
tokenPtr splitToTokens(char *line) {
    int ch; /*a char to go over the line char by char*/
    int i = 0;
    int inQuotationMarks = FALSE;
    tokenPtr head = NULL;
    tokenPtr tail = head; /*a tail so we wo'nt run the hole list every time we want to add a node*/
    char currString[MAX_LINE] = {0}; /*the currant string, all the chars since the last space oe comma*/
	ch = line[i++];
    while (ch != '\0') { /*loop over the line*/
        if (ch == ',' && !inQuotationMarks) { /*if the current char is a comma*/
            splitIfNeeded(&head, &tail, currString); /*the comma will separate if needed*/
            /*even if there was a split and even if not, now we want to add a node with a comma*/
            addToken(&tail, ","); /*add a node with ","*/
            if (head == NULL) {
                head = tail; /*if this is thr first node*/
            } else {
                tail = tail->next;
            }
        } else if (isspace(ch) && !inQuotationMarks) { /*if the char is a white-char or ':'*/
            splitIfNeeded(&head, &tail, currString);
            /*we don't need to do nothing with a space after another space or a comma*/
        } else { /*when ch is everything else*/
            int len = strlen(currString);
            currString[len] = ch;
            currString[len + 1] = '\0';
            if(ch == '\"') {
                if(inQuotationMarks) {
                    inQuotationMarks = FALSE;
                }
                else {
                    inQuotationMarks = TRUE;
                }
            }
            if (ch == ':') {
                splitIfNeeded(&head, &tail, currString);
            }
        }
        ch = line[i++]; /*move to the next char*/
    }
    splitIfNeeded(&head, &tail, currString);
    return head;
}


/**
 * splitIfNeeded
 *
 * This function is a side function of splitToTokens
 * The function gets the linked list of tokens and the current string that is read,
 * if the current string is'nt empty the fcntion add a new node with that string to the list
 *
 * params:
 * head - a pointer to a tolenPtr to the list head
 * tail - a pointer to a tolenPtr to the list tail
 * currString - the current string read from the line
 *
 * */
void splitIfNeeded(tokenPtr *head, tokenPtr *tail, char *currString){
    if(currString[FIRST_ELEMENT]) { /*if tha last string hasn't been separated*/
        addToken(tail, currString); /*add a node with current string*/
        if((*head) == NULL) { /*if this is the first node*/
            (*head) = (*tail);
        }
        else{
            (*tail) = (*tail)->next;
        }
        strcpy(currString, "");
    }
}


/**
 * checkFail
 *
 * The function checks if a memory allocate failed.
 * The function gets the pointer to the allocated memory and will exit the program if the pointer is NULL
 * Of curse that the function will also print to the stderr that an memory problem has occurred
 *
 * */
void checkFail(void *pointer) {
    if(pointer == NULL) {
        fprintf(stderr, "error with system memory\n");
        exit(FAIL);
    }
}


/**
 * checkIfHasValidLabel
 *
 * The function checks if a command is a label.
 * The function gets the pointer to the first token in a line and returns if the token is a label
 * or if the command is an illegal label or if the command isn't a label
 *
 * params:
 * head - a pointer to the first token in a line
 *
 * return:
 * int - 1 for label, 0 for not a label and -1 for error
 *
 * */
int checkIfHasValidLabel(tokenPtr head) {
	if(head -> string[strlen(head -> string)-1] == ':'){
		char labelName[MAX_LENGTH_OF_LABEL_NAME + 2]; /*1 for ':', and 1 for '\0'*/
		if(strlen(head->string) > MAX_LENGTH_OF_LABEL_NAME+1) { /*if the label name is to long, +1 for ':'*/
                fprintf(stderr, "ERROR: Label must be less then %d characters\n",
                        MAX_LENGTH_OF_LABEL_NAME + 1); /*1 because we wright less*/
            return FAIL;
        }
        strcpy(labelName, head->string);
        labelName[strlen(labelName) - 1] = '\0'; /*delete the ':'*/
        if(checkIfValidLabelName(labelName, TRUE) == FAIL)
            return FAIL;
        else
		    return TRUE;
	}
	return FALSE;
}


/**
 * checkIfValidLabelName
 *
 * The function check if a string is a valid name for a label, is must be less the 30 chars, start with a alpha
 * and have only alphas and numbers
 *
 * params:
 * labelName - the string to check
 *
 * return:
 * int - If the string is a valid name or not
 *
 * */
int checkIfValidLabelName(char *labelName, int toPrint) {
    int i;
    if(strlen(labelName) > MAX_LENGTH_OF_LABEL_NAME) {
        if(toPrint) {
            fprintf(stderr, "ERROR: Label must be less then %d characters\n",
                    MAX_LENGTH_OF_LABEL_NAME + 1); /*1 because we wright less*/
        }
        return FAIL;
    }
    if(!isalpha(labelName[0])){
        if(toPrint) {
            fprintf(stderr, "ERROR: Label must start with an alpha character\n");
        }
        return FAIL;
    }
    for (i = 1; i < strlen(labelName); i++) {
        if (!isalnum(labelName[i])) { /*checks if there is a non alphanumeric character, which is illegal*/
            if(toPrint) {
                fprintf(stderr, "ERROR: Label must contain only alphanumeric characters\n");
            }
            return FAIL;
        }
    }
    for (i = 0; i < NUM_OF_ACTION_COMMANDS; i++) { /* checks that the name of the label isn't a command*/
        if (!strcmp(labelName, actionCommandsArray[i].name)) {
            if(toPrint) {
                fprintf(stderr, "ERROR: Label cant have the name of a command\n");
            }
            return FAIL;
        }
    }
    for (i = 0; i < NUM_OF_REGISTERS; i++) {/* checks that the name of the label isn't a register */
        if (!strcmp(labelName, registers[i])) {
            if(toPrint) {
                fprintf(stderr, "ERROR: Label cant have the name of a register\n");
            }
            return FAIL;
        }
    }
    return SUCCESS;
}


/**
 * isNumber
 *
 * This function check if a given string is a valid number
 * Valid number consists of only digits, except for the first index where there can be a '+' or '-'
 *
 * params:
 * toCheck - the string that need to be checked
 *
 * return:
 * int - if the function failed or succeeded
 *
 * */
int isNumber(char *toCheck) {
    int i, ch;

    for(i = 0; i < strlen(toCheck); i++) {
        ch = toCheck[i];
        if(i == 0 && (ch == '+' || ch == '-')) {
            continue; /*minus and plus can be in the first index*/
        }
        if(!isdigit(ch)) {
            return FALSE;
        }
    }
    return TRUE;
}


/**
 * checkLabelName
 *
 * The function gets a labels name and checks if there is an label with that name
 * If there is a label with that name the function will return that label
 * and if there isn't the function will return NULL
 *
 * params:
 * name - the name to search for
 *
 * return:
 * labelPtr - a pointer to the found label or NULL if not found
 *
 * */
labelPtr checkLabelName(char *name){
    labelPtr temp = labelTable;
    for(; temp != NULL; temp = temp->next){
        if(!strcmp(temp->name, name)){
            return temp;
        }
    }
    return NULL;
}


/**
 * getAddressingMode
 *
 * This function gets a token of an operand and returns which addressing mode the oprand is
 * The function will return the function returns fail if the operand isnt any adressing mode and the addressing mode if the token is an operand
 *
 * params:
 * operand - the tokenPtr of the operand
 *
 * return:
 * int - the addressing mode id
 *
 **/
int getAddressingMode(tokenPtr operand) {
    if(operand -> string[0] == '#') {/* checks if the operand uses immediate addresing mode*/
        char *number = operand->string;
        number++;
        if(isNumber(number)) {
            return IMMEDIATE_ADDRESSING;
        }
        return UNKNOWN_ADDRESSING_MODE;
    }
    else{
        int i;
        for(i = 0; i < strlen(operand->string)-1; i++) {/*looks for a dot*/
            if (operand->string[i] == '.') {
                break;
            }
        }
        if(operand->string[i] == '.'){ /*if we found a dot*/
            if((operand->string[i+1] != '1' && operand->string[i+1] != '2') || operand->string[i+2] != '\0') {/*checks for struct access addressing mode*/
                return UNKNOWN_ADDRESSING_MODE;
            }
            return STRUCT_ACCESS_ADRESSING;
        }
        else {
            for(i = 0;i<NUM_OF_REGISTERS;i++){ /* checks if the operand uses registers addresing mode*/
                if(!strcmp(operand -> string,registers[i])){
                    return REGISTERS_ADDRESSING;
                }
            }
            if(checkIfValidLabelName(operand->string, FALSE) == SUCCESS) {
                return DIRECT_ADDRESSING; /*otherwise the function uses direct addressing*/
            }
            else {
                return UNKNOWN_ADDRESSING_MODE;
            }
        }
    }
}


/**
 * checkIfValidAddressingMode
 *
 * This function gets an addressing mode type and an array of valid addressing modes and checks if our addressing mode type is valid
 * The function will return fail if the addressing mode isnt valid and valid if the addressing mode is valid
 *
 * params:
 * addressingMode - the addressing mode type
 * validAddressingModes - the array of valid addressing modes
 *
 * return:
 * int - if the addressing mode is valid or ileagal
 *
 **/
int checkIfValidAddressingMode(int addressingMode, const int *validAddressingModes) {
    int i;
    for (i = 0; i < NUMBER_OF_ADRESSING_MODES; i++) { /* we check all of the leagal addressing modes to sea if our addressing mode is leagal*/
        if (validAddressingModes[i] == addressingMode) {/* if we see our addressing mode in the list, we return valid*/
            return VALID
        }
        if (validAddressingModes[i] == -1) { /*otherwise we return fail*/
            return FAIL;
        }
    }
    return FAIL;
}


/**
 * analyzeGetArray
 *
 * This function gets an token list and an array and fill the array with the numbers given in the linked list as a .data
 * The function will check syntax errors and will point if found and return FAIL
 * The function will check that all the tokens are numbers and separated with a comma token
 *
 * params:
 * head - the head of the token linked list
 * numbersArray - the array to put the numbers in
 *
 * return:
 * int - if the function failed or succeeded
 *
 * */
int analyzeGetArray(tokenPtr commandToken, int *numbersArray) {
    int paramIndex = 0;
    int counter = 0;
    tokenPtr curr = commandToken->next; /*the commandToken is the .data token*/

    while(curr) { /*run oven the linked list*/
        /*every even index if a number index and between them in the odd indexes are the commas*/
        if(paramIndex%2 == 0) { /*if this is a number index*/
            if(!strcmp(curr->string, ",")) { /*if the string is ","*/
                fprintf(stderr, "ERROR: multiple consecutive commas\n"); /*then there is two ',' together*/
                return FAIL;
            }
            if(!isNumber(curr->string)) {
                fprintf(stderr, "ERROR: .data parameters must be numbers\n"); /*then there is two ',' together*/
                return FAIL;
            }
            /*if everything is all right then add the number to the array*/
            numbersArray[counter] = atoi(curr->string);
            /*if the number is to big or to small*/
            if(numbersArray[counter] > MAX_VALID_DATA_NUMBER || numbersArray[counter] < MIN_VALID_DATA_NUMBER) {
                fprintf(stderr, "ERROR: .data parameters must be between %d to %d\n", MIN_VALID_DATA_NUMBER, MAX_VALID_DATA_NUMBER);
                return FAIL;
            }
            counter++;
        }
        else { /*a odd index, then this is a comma index*/
            if(strcmp(curr->string, ",")) { /*if its not a comma*/
                fprintf(stderr, "ERROR: missing comma\n");
                return FAIL;
            }
            if(!strcmp(curr->string, ",") && curr->next == NULL) { /*if this is a comma and the last token in the list*/
                fprintf(stderr, "ERROR: extraneous comma at end of line\n"); /*then its not needed*/
                return FAIL;
            }

        }
        curr = curr->next;
        paramIndex++;
    }
    if(!counter) { /*if there was no numbers*/
        fprintf(stderr, "ERROR: .data must get at lest one number\n");
        return FAIL;
    }
    return SUCCESS;
}


/**
 * getCommandType
 *
 * This function identify a token's string command
 * The function check if the token's string is one if the posibale commands name and return the command id
 *
 * params:
 * token - the string's token
 *
 * return:
 * int - the id of the command
 *
*/
int getCommandType(tokenPtr token) {
    int i;
    for(i = 0; i<NUM_OF_ACTION_COMMANDS; i++) { /*run over commands*/
        if(!strcmp(token->string, actionCommandsArray[i].name)) { /*if the token string equals the command name*/
            return actionCommandsArray[i].id;
        }
    }
    if(!strcmp(token->string, ".data")) {
        return DATA;
    }
    if(!strcmp(token->string, ".string")) {
        return STRING;
    }
    if(!strcmp(token->string, ".struct")) {
        return STRUCT;
    }
    if(!strcmp(token->string, ".extern")) {
        return EXTERN;
    }
    if(!strcmp(token->string, ".entry")) {
        return ENTRY;
    }
    return UNKNOWN;
}


/**
 * checkStringToken
 *
 * The function gets a token and check if its a valid string parameter
 * A string parameter must start and end with " and must be the last token in the line
 *
 * params:
 * token - the token to check
 *
 * return:
 * int - if the token is valid or not
 *
 * */
int checkStringToken(tokenPtr token) {
    if(!token) {
        fprintf(stderr, "ERROR: expected string\n");
        return FAIL;
    }
    if(token->string[0] != '\"' || token->string[strlen(token->string)-1] != '\"') {
        fprintf(stderr, "ERROR: string parameter is a string that starts and end with \", |%s| is not a valid string\n", token->string);
        return FAIL;
    }
    if(token->next != NULL) { /*if the string isn't the end of the line*/
        fprintf(stderr, "ERROR: expected end of line after: |%s|\n", token->string);
        return FAIL;
    }
    return SUCCESS;
}


/**
 * storeString
 *
 * The function stores a string in the data memory base
 *
 * params:
 * string - the string to store in the memory
 *
 * */
void storeString(char *string) {
    int ch,i = 0;
    ch = string[i];
    while(ch != '\0') { /*run on the string*/
        if(i == FIRST_ELEMENT || i == (strlen(string)-1)) { /*if this is the first or last index that with |"|*/
            ch = string[++i]; /*update the char o the next one*/
            continue;
        }
        dataMemoryBase[DC] = ch; /*the data memory base in the counter place is the current char*/
        DC++; /*update the counter*/
        ch = string[++i]; /*update the char o the next one*/
    }
    /*after the string chars we need a memory word with 0's to mark end of string*/
    dataMemoryBase[DC] = '\0';
    DC++; /*update the counter*/
}


/**
 * addNumberToActionMemoryBase
 *
 * The function add a given number to the action memory base
 * The function will add the absolute code method bits to the memory to
 * The function will correct negatives numbers to the Two's complement system with 10 bits
 *
 * params:
 * number - the number that we want to add to the memory*/
void addNumberToActionMemoryBase(int number) {
    int operandMemoryWord;
    if(number < 0) { /*two's complement system*/
        number += pow(2, NUM_OF_BITS_IN_MEMORY_WORD-2); /*-2 because of the E R A bits*/
    }
    operandMemoryWord = (number << NUM_OF_CODE_METHOD_BITS) | ABSOLUTE_CODE_METHOD_MASK;
    actionMemoryBase[IC-MEMORY_START_POS] = operandMemoryWord;
    IC++;
}


/**
 * addNumberToDataMemoryBase
 *
 * The function add a number to the data memory base
 * The function will correct negatives numbers to the Two's complement system with 10 bits
 *
 * params:
 * newNumber - the new number the we want to add to the data memory base
 *
 * */
void addNumberToDataMemoryBase(int newNumber) {
    dataMemoryBase[DC] = newNumber;
    if(newNumber < 0) {
        dataMemoryBase[DC] += pow(2, NUM_OF_BITS_IN_MEMORY_WORD);
    }
    DC++;
}


/**
 * numberOfLinesToSkip
 *
 * The function checks how many lines we need to skip, based of the operands addressing modes
 *
 * params:
 * sourceAddressingMode - the id of the source operand's addressing mode, -1 for no operand
 * destinyAddressingMode - the id of the destiny operand's addressing mode, -1 for no operand
 *
 * */
int NumberOfLinesToSkip(int sourceAddressingMode, int destinyAddressingMode) {
    int sum = 1;
    if(sourceAddressingMode == STRUCT_ACCESS_ADRESSING){ /*if the operand is a struct we need to save two lines*/
        sum += 2;
    }
    else if(sourceAddressingMode != NO_OPERAND){/*otherwise, if we have an operand, we need to save one line*/
        sum += 1;
    }
    if(destinyAddressingMode == STRUCT_ACCESS_ADRESSING){/*if the operand is a struct we need to save two lines*/
        sum += 2;
    }
    else if(destinyAddressingMode != NO_OPERAND){/*otherwise, if we have an operand, we need to save one line*/
        sum += 1;
    }
    if(destinyAddressingMode == REGISTERS_ADDRESSING && sourceAddressingMode == REGISTERS_ADDRESSING){ /* if both of the operands are register, we need to save only one line*/
        sum -= 1;
    }
    return sum;
}


/**
 * codeActionCommand
 *
 * The function codes into actionMemoryBase the command and saves the other lines that we need to code for the second pass
 *
 * params:
 * sourceAddressingMode - the id of the source operand's addressing mode, -1 for no operand
 * destinyAddressingMode - the id of the destiny operand's addressing mode, -1 for no operand
 *
 * */
void codeActionCommand(int destinyOperandAddressingMode, int sourceOperandAddressingMode, int commandType) {
    int codedActionCommand;
    int opCode = actionCommandsArray[commandType].opCode;
    if(destinyOperandAddressingMode != NO_OPERAND && sourceOperandAddressingMode == NO_OPERAND) {
        codedActionCommand = opCode << 6 | destinyOperandAddressingMode << 2;
    }
    else if (destinyOperandAddressingMode == NO_OPERAND && sourceOperandAddressingMode == NO_OPERAND){
        codedActionCommand = opCode << 6;
    }
    else{
        codedActionCommand = opCode << 6 | sourceOperandAddressingMode << 4 | destinyOperandAddressingMode << 2;
    }
    actionMemoryBase[IC - MEMORY_START_POS] = codedActionCommand;
    IC += NumberOfLinesToSkip(sourceOperandAddressingMode, destinyOperandAddressingMode);
}


/**
 * printInBinary
 *
 * A debug function
 * The function prints a number in binary
 *
 * params:
 * num - the number that need to be printed in binary
 *
 * */
void printInBinary(int num) {
    int mask;
    for(mask = 512; mask!= 0; mask = mask>>1) {
        if(num&mask) {
            fprintf(stderr , "%d", 1);
        }
        else{
            fprintf(stderr, "%d",0);
        }
    }
    fprintf(stderr, "\n");
}


/**
 * printMemoryBase
 *
 * A debug function
 * The function prints a memory base until there are five zeros in one sequence
 *
 * params:
 * memoryBase - witch memory base to print
 *
 * */
void printMemoryBase(int *memoryBase) {
    int i,j;
    for(i = 0;; i++){
        printInBinary(memoryBase[i]);
        if(memoryBase[i] == 0){
            j++;
        }
        else{
            j=0;
        }
        if(j == 5){
            break;
        }

    }
}


/**
 * addAddressToActionMemoryBase
 *
 * The function add a new word to the action memory base with the address of a given lable
 * The function handle extern to
 *
 * paramsL
 * label - the label the we want to add the address of
 *
 * */
void addAddressToActionMemoryBase(labelPtr label, externReferencePtr *externReferenceHead) {
    int operandMemoryWord;
    int codeMethodBits;
    operandMemoryWord = label->address << NUM_OF_CODE_METHOD_BITS;
    if(label->type == EXTERN_LABEL) { /*if the label is an external label*/
        codeMethodBits = EXTERNAL_CODE_METHOD_MASK; /*then the code method is external*/
        addExternReference(externReferenceHead, label->name, IC);
    }
    else {
        codeMethodBits = RELOCATABLE_CODE_METHOD_MASK; /*then the code method is relocatable*/
    }
    operandMemoryWord |= codeMethodBits; /*the memory bits is the connection of the address and the code method*/
    actionMemoryBase[IC-MEMORY_START_POS] = operandMemoryWord;
    IC++;
}


/**
 * intToBase32
 *
 * The function convert a given int into the strange 32 base
 * The function will store the result in a given string
 * The function assume that the given length is at lest 3
 *
 * params:
 * output - the string where to store the converted number
 * num - the number that need to be converted
 *
 * */
void intToBase32(char *output, int num) {
    int firstFiveBits = ((int) pow(2, 5)) - 1; /*-1 for getting every bit on*/
    int lastFiveBits = (((int) pow(2, 10)) -1) - firstFiveBits; /*-1 for getting every bit on, - firstFiveBits because we want only the lasy five bits*/
    char base32String[3]; /*two 32 base chars represents a memory word and 1 char for end of string sign*/
    base32String[0] = base32[(num & lastFiveBits) >> 5];
    base32String[1] = base32[num & firstFiveBits];
    base32String[2] = '\0';
    strcpy(output, base32String);
}



/**
 * handleSourceOperand
 *
 * This function handle an operand in an action command
 * The function check that there is an operand and that the operand's addressing mode fits the line command type
 * The function will update the commandLine operand addressing mode if there were no problems with the operand
 * The function will return FAIL else
 *
 * params:
 * actionCommandLine - a pointer to the commandLine of the operand
 * operandToken - the operand's toke
 *
 * return:
 * int - if the operand is valid or not
 *
 * */
int handleOperand(commandLinePtr actionCommandLine, tokenPtr operandToken, int whatOperand) {
    int *operandAddressingMode;
    int commandType = actionCommandLine->commandType;
    char operandName[12];

    if(whatOperand == SOURCE) { /*if this is an source operand*/
        /*then the operand addressint mode should be stored in the sourceOperandAddressingMode*/
        operandAddressingMode = &(actionCommandLine->sourceOperandAddressingMode);
        strcpy(operandName, "source");
    }
    else { /*if this is an destiny operand*/
        /*then the operand addressint mode should be stored in the destinyOperandAddressingMode*/
        operandAddressingMode = &(actionCommandLine->destinyOperandAddressingMode);
        strcpy(operandName, "destination");
    }

    if(operandToken == NULL) { /*checks if there is an operand at all*/
        fprintf(stderr, "ERROR: the Command %s requires a %s operand\n", actionCommandsArray[commandType].name, operandName);
        return FAIL;
    }
    if(whatOperand == DESTINY) { /*if this is a destiny operand*/
        if(operandToken->next != NULL) { /*then if it's not the end of the line*/
            fprintf(stderr, "ERROR: expected end of line after |%s|\n", operandToken->string);
            return FAIL;
        }
    }

    /*check the operand addressing mode*/
    (*operandAddressingMode) = checkAddressingMode(operandToken, commandType, whatOperand);
    if((*operandAddressingMode) == FAIL) { /*if the operand's addressing mode doest fit the cmmand type*/
        return FAIL;
    }
    return SUCCESS;
}


/**
 * handleTwoOperands
 *
 * This function handle action command with two operands
 * The function check's the validity of both operand and that there is a comma between them
 * The function will print if there is a problem and return FAIL
 *
 * params:
 * actionCommandLine - the action command commandLine
 * firstOperandToken - the token of the first operand, the source operand
 *
 * return:
 * int - if the command operands are valid or not
 *
 * */
int handleTwoOperands(commandLinePtr actionCommandLine, tokenPtr firstOperandToken) {
    tokenPtr commaToken;
    tokenPtr secondOperandToken;
    int commandType = actionCommandLine->commandType;

    if(handleOperand(actionCommandLine, firstOperandToken, SOURCE) == FAIL) { /*if there is a error in the first operand*/
        return FAIL;
    }

    commaToken = firstOperandToken->next; /*between the two operand suppose to be a comma*/
    if(commaToken == NULL) { /*checks if the there is nothing after the first operand*/
        fprintf(stderr, "ERROR: the Command %s requires two operands\n", actionCommandsArray[commandType].name);
        return FAIL;
    }
    if(strcmp(commaToken->string, ",")) { /*checks if there is a comma*/
        fprintf(stderr, "ERROR: missing comma\n");
        return FAIL;
    }

    secondOperandToken = commaToken->next; /*after the comma suppose to be the second operand*/
    if(handleOperand(actionCommandLine, secondOperandToken, DESTINY) == FAIL) { /*if there is a error in the second operand*/
        return FAIL;
    }

    return SUCCESS;
}


/**
 * checkAddressingMode
 *
 * This function gets a operand addressing mode and check if it is a addressing mode that fits the command type
 * The function will print if there is a problem and return FAIL
 * The function get a parameter to indicate the operand type, source or destiny
 *
 * params:
 * operandToken - the operand's token
 * commandType - the operand's command type
 * whatOperand - the operand type, source or destiny
 *
 * return:
 * int - the operand addressing mode or FAIL if the addressing mode doest fit the action type
 *
 * */
int checkAddressingMode(tokenPtr operandToken, int commandType, int whatOperand) {
    int operandAddressingMode;
    int isValid;
    const int *validAddressingModes;
    char operandNumberString[12];

    if (whatOperand == SOURCE) { /*if this is an source operand*/
        validAddressingModes = actionCommandsArray[commandType].sourceOperandValidAddressingModes;
        strcpy(operandNumberString, "source");
    } else { /*if this is an destiny operand*/
        validAddressingModes = actionCommandsArray[commandType].destinyOperandValidAddressingModes;
        strcpy(operandNumberString, "destination");
    }

    operandAddressingMode = getAddressingMode(operandToken); /*get the operand's addressing mode*/

    /*check if the operan's addressing mode fits the command type*/
    isValid = checkIfValidAddressingMode(operandAddressingMode, validAddressingModes);
    if (isValid == FAIL) { /*if the addressing mode is'nt valid*/
        fprintf(stderr, "ERROR: the command %s doesnt accept %s for the %s operand\n", actionCommandsArray[commandType].name,
                addressingModes[operandAddressingMode], operandNumberString);
        return FAIL;
    }
    return operandAddressingMode;
}


/**
 * handleLabel
 *
 * This function handle label disclaimer at the start of a line
 * The function add the label to the label table
 * If there is an error like that there is alabel with the same name all ready
 * then the function will return FAIL
 *
 * params:
 * labelName - the name of the new label, the ':' will be omitted
 * address - the address of the label
 * labelType - the label type, action, data or external label
 *
 * return:
 * inr - if the label is valid or not
 *
 * */
int handleLabel(char *labelName, int address, int labelType) {
    labelPtr newLabel = (labelPtr) calloc(1, sizeof(label));
    char *name = (char *) calloc(MAX_LENGTH_OF_LABEL_NAME + 2, sizeof(char)); /*1 for ':' and 1 for '\0' at the end*/
    checkFail(newLabel);
    checkFail(name);
    /*if there is a label then the second token should be the command*/
    /*set new and add it to the list*/
    strcpy(name, labelName);
	if(name[strlen(name) - 1] == ':') /*if the last char of the name is : then*/
    	name[strlen(name) - 1] = '\0'; /*delete the ':' at the end of the name*/
    setLabel(newLabel, name, address, labelType, FALSE);
    if (addLabel(&labelTable, newLabel) == FAIL) {
        free(name);
        free(newLabel);
        return FAIL;
    }
    return SUCCESS;
}

/**
 * handleNotCommandLines
 *
 * This function recognize comment and empty lines
 * The function will return TRUE if the line is a not command line
 * and FALSE else
 *
 * params:
 * head - a pointer to the head of the token linked
 *
 * return:
 * int - if this line is not a command line
 *
 * */
int handleNotCommandLines(tokenPtr head) {
    if(head == NULL) {
        printLog("the line is empty\n");
        return TRUE;
    }
    if(head->string[FIRST_ELEMENT] == ';') {
        printLog("the line is a comment\n");
        return TRUE;
    }
    return FALSE;
}


/**
 * printLineError
 *
 * This function prints the part that comes after every error
 * The function also free the token linked list if the line
 *
 * params:
 * lineString - the string of the line
 * lineNumber - the number of the line in the input file
 * head - the head of the token linked list
 *
 * */
void printLineError(char *lineString, int lineNumber, tokenPtr head) {
    fprintf(stderr, "in line number: %d\n%s\n\n", lineNumber, lineString);
    freeTokenList(head);
}
