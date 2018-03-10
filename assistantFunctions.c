/*
 * Created by PJA on 3/5/2018.
*/


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
    char *result = (char *) calloc(sizeof(char), strlen(s1)+strlen(s2)+1); /*+1 for the null-terminator */
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
    tokenPtr head = NULL;
    tokenPtr tail = head; /*a tail so we wo'nt run the hole list every time we want to add a node*/
    char currString[MAX_LINE] = {0}; /*the currant string, all the chars since the last space oe comma*/
    ch = line[i++];
    while (ch != '\0') { /*loop over the line*/
        if (ch == ',') { /*if the current char is a comma*/
            splitIfNeeded(&head, &tail, currString); /*the comma will separate if needed*/
            /*even if there was a split and even if not, now we want to add a node with a comma*/
            addToken(&tail, ","); /*add a node with ","*/
            if (head == NULL) {
                head = tail; /*if this is thr first node*/
            } else {
                tail = tail->next;
            }
        } else if (isspace(ch)) { /*if the char is a white-char or ':'*/
            splitIfNeeded(&head, &tail, currString);
            /*we don't need to do nothing with a space after another space or a comma*/
        } else { /*when ch is everything else*/
            int len = strlen(currString);
            currString[len] = ch;
            currString[len + 1] = '\0';
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
        if((*head) == NULL) { /*if this is thr first node*/
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


/*
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
        char labelName[MAX_LENGTH_OF_LABEL_NAME + 2]; /*1 for ':' and 1 for the '\0' at the end*/
        strcpy(labelName, head->string);
        labelName[strlen(labelName) - 1] = '\0'; /*delete the ':'*/
        if(checkIfValidLabelName(labelName) == FAIL)
            return FAIL;
        else
		    return TRUE;
	}
	return FALSE;
}


/**
 * checkIfValidLabelName
 *
 * The function check if a string is a valid name for a label, is must be less the 31 chars, start with a alpha
 * and have only alphas and numbers
 *
 * params:
 * labelName - the string to check
 *
 * return:
 * int - If the string is a valid name or not
 *
 * */
int checkIfValidLabelName(char *labelName) {
    int i;
    if(strlen(labelName) > MAX_LENGTH_OF_LABEL_NAME) {
        fprintf(stderr, "ERROR: Label must be less then %d characters\n", MAX_LENGTH_OF_LABEL_NAME + 1); /*1 because we wright less*/
        return FAIL;
    }
    if(!isalpha(labelName[0])){
        fprintf(stderr, "ERROR: Label must start with an alpha character\n");
        return FAIL;
    }
    for (i = 1; i < strlen(labelName); i++) {
        if (!isalnum(labelName[i])) { /*checks if there is a non alphanumeric character, which is illegal*/
            fprintf(stderr, "ERROR: Label must contain only alphanumeric characters\n");
            return FAIL;
        }
    }
    for (i = 0; i < NUM_OF_COMMAND_TYPES; i++) { /* checks that the name of the label isn't a command*/
        if (!strcmp(labelName, commands[i].name)) {
            fprintf(stderr, "ERROR: Label cant have the name of a command\n");
            return FAIL;
        }
    }
    for (i = 0; i < NUM_OF_REGISTERS; i++) {/* checks that the name of the label isn't a register */
        if (!strcmp(labelName, registers[i])) {
            fprintf(stderr, "ERROR: Label cant have the name of a register\n");
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
    labelPtr temp = labelTabale;
    for(; temp != NULL; temp = temp->next){
        if(!strcmp(temp->name, name)){
            return temp;
        }
    }
    return NULL;
}


/**
 * checkAddressingMode
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
int checkAddressingMode(tokenPtr operand) {
    char *number = operand->string;
    number++;
    if(operand -> string[0] == '#' && isNumber(number)){/* checks if the operand uses immediate addresing mode*/
        return IMMEDIATE_ADDRESSING;
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
                return FAIL;
            }
            return STRUCT_ACCESS_ADRESSING;
        }
        else {
            for(i = 0;i<NUM_OF_REGISTERS;i++){ /* checks if the operand uses registers addresing mode*/
                if(!strcmp(operand -> string,registers[i])){
                    return REGISTERS_ADDRESSING;
                }
            }
            return DIRECT_ADDRESSING; /*otherwise the function uses direct addressing*/
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
 * int - if the adddressing mode is valid or ileagal
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
    return VALID;
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
                fprintf(stderr, "ERROR: extraneous comma at end of commandToken\n"); /*then its not needed*/
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
    for(i = 0; i<NUM_OF_COMMAND_TYPES; i++) { /*run over commands*/
        if(!strcmp(token->string, commands[i].name)) { /*if the token string equals the command name*/
            return commands[i].id;
        }
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
    DC++;
    if(newNumber < 0) {
        dataMemoryBase[DC] += pow(2, NUM_OF_BITS_IN_MEMORY_WORD);
    }
}
