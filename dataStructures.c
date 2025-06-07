/*
 * mmn14
 * dataStructures.c
 *
 * In this file we have all the functions that relate to data structurs and their memmory.
 * like freeing the memmory, adding to the linked list and more
 * there are 5 data structures: externReference, commandLine, commandInfo, label and token
 *
 * token: a linked list of strings
 * lable: a linked list of lables, with the name of the lable, the type of the lable and if the lable has entry
 * commandInfo: contains all the info of a command - the name, the id, the opcode and the source and destination addressing modes
 * commandLine: a lined list with all the commands with a token for the original line (splited to tokens) source and destination addressing modes, command type, line 
 * number and if the command has a lable in it
 * externReference - a linked list of names and addresses
 *
 * */

#include "dataStructures.h"
#include "assistantFunctions.h"


/**
 * addToken
 *
 * This function add at the end of a token list a new node with a given string
 *
 * params:
 * head - a pointer to tokenPtr of the head of the list
 * string - the string we want the new token to contain
 *
 * */
void addToken(tokenPtr *head, char *string) {
    char *newString;
    tokenPtr curr;
    tokenPtr new;

    new = (tokenPtr) calloc(1, sizeof(token)); /*calloc memory for the new node*/
    checkFail(new);
    newString = (char *) calloc(strlen(string) + 1, sizeof(char));
    /*we want to calloc memory for the string because we dont know how long it is*/
    checkFail(newString);
    strcpy(newString, string); /*copy the string from the parameter string t our new string*/
    new->string = newString; /*the string of the new token is newString*/
    new->next = NULL; /*the new token is foing to be the last token to it pointes to NULL*/

    if(*head == NULL) { /*if we got an empty list*/
        *head= new;
        return;
    }
    /*run to the end of the ist*/
    curr = *head;
    while(curr->next) {
        curr = curr->next;
    }

    curr->next = new; /*connect the current last token to point to te new one*/
    return;
}


/**
 * freeTokenList
 *
 * This function gets a token list head and frees the list
 *
 * params:
 * head - a pointer to the head of the token list
 *
 * */
void freeTokenList(tokenPtr head) {
    tokenPtr curr = head;

    if(head == NULL){ /*if we got an empty list*/
        /*so we have no memory to free*/
        return;
    }
    /*run over the list*/
    while(curr) {
        curr = head->next;
        free(head->string); /*we want to free the string that is in the token also*/
        free(head);
        head = curr;
    }
}


/**
 * addLabel
 *
 * The function add a new label to a label linked list
 * If the name of the new name is all ready assined the function will print error
 * The function return if there was an error or not
 *
 * params:
 * head - the head of the linked list
 * new - the new label node we want to add
 *
 * return:
 * int - if there was an error or not
 *
 * */
int addLabel(labelPtr *head, labelPtr new) {
    labelPtr curr = (*head);
    new->next = NULL;
    if(*head == NULL) { /*if we got en empty list*/
        *head = new;
        return SUCCESS;
    }
    while(curr->next){ /*go over the list*/
        if(!strcmp(curr->name, new->name)) { /*if the curr label name equals the new label name*/
            fprintf(stderr, "ERROR: can't define two labels with the same name\n");
            return FAIL;
        }
        curr = curr->next;
    }
    if(!strcmp(curr->name, new->name)) { /*check for te last one to*/
        fprintf(stderr, "ERROR: can't define two labels with the same name: %s\n", curr->name);
        return FAIL;
    }
    curr->next = new; /*the last node point to the new one*/
    return SUCCESS;
}


/**
 * freeLabelList
 *
 * This function gets a label list head and frees the list
 *
 * params:
 * head - a pointer to the head of the label list
 *
 * */
void freeLabelList(labelPtr head) {
    labelPtr curr = head;

    if(head == NULL){ /*if we got an empty list*/
        /*so we have no memory to free*/
        return;
    }
    /*run over the list*/
    while(curr) {
        curr = head->next;
        free(head->name);
        free(head);
        head = curr;
    }
}


/**
 * freeLastLabel
 *
 * This function gets a label list head and frees the last label in the list
 *
 * params:
 * head - a pointer to the head of the label list
 *
 * */
void freeLastLabel(labelPtr head) {
    labelPtr curr = head;
    labelPtr prev = head;

    if(head == NULL){ /*if we got an empty list*/
        /*so we have no memory to free*/
        return;
    }
    /*run over the list*/
    while(curr->next) {
        prev = curr;
        curr = curr->next;
    }

    free(curr->name);
    free(curr);
    prev->next = NULL;
}


/**
 * setLabel
 *
 * The function set the fields of a label to given parameters
 *
 * params:
 * label - the label to set
 * name - the name to set to
 * address - the address to set to
 * isAction - the isAvtion to set to
 * isExtarnal - the isExtarnal to set to
 *
 * */
void setLabel(labelPtr label, char *name, int address, int type, int hasEntry) {
    label->name = name;
    label->address = address;
    label->type = type;
    label->hasEntry = hasEntry;
}


/**
 * addCommandLine
 *
 * The function add a new node to a linked list of commandLines
 *
 * params:
 * head - the head of the commandLines linked list
 * lineNumber - the line number of the new commandLine
 * commandType - The command type of the new commandLine
 * tokenListHead - the head of the linked list of tokens of the new commandLine
 *
 * */
void addCommandLine(commandLinePtr *head, commandLinePtr new) {
    commandLinePtr curr = *head;

    if(*head == NULL) {
        *head = new;
        return;
    }
    while(curr->next) {
        curr = curr->next;
    }
    curr->next = new;
}


/**
 * setCommandLine
 *
 * The function sets a new commandLine with given parameters and return a pointer to the commandLine
 *
 * params:
 * lineNumber - the new commandLine lineNumber
 * commandType - the new commandLine commandType
 * hasLabel - if new commandLine has a label
 * sourceOperandAddressingMode - the new commandLine sourceOperandAddressingMode
 * destenationOperandAddressingMode - the new commandLine destenationOperandAddressingMode
 * tokenListHead - the new tokenListHead pointer
 *
 * return:
 * commandLinePtr - the new commandLine pointer
 *
 * */
commandLinePtr setNewCommandLine(int lineNumber, int commandType, int hasLabel, int sourceOperandAddressingMode,
                                 int destenationOperandAddressingMode, tokenPtr tokenListHead, char *lineString) {
    commandLinePtr new = (commandLinePtr) calloc(1, sizeof(commandLine));
    checkFail(new);
    new->next = NULL;
    new->lineNumber = lineNumber;
    new->commandType = commandType;
    new->hasLabel = hasLabel;
    new->sourceOperandAddressingMode = sourceOperandAddressingMode;
    new->destenationOperandAddressingMode = destenationOperandAddressingMode;
    new->tokenListHead = tokenListHead;
    strcpy(new->lineInString, lineString);
    return new;
}



/**
 * freeCommandLineList
 *
 * This function gets a commandLine list head and frees it
 *
 * params:
 * head - a pointer to the head of the commandLine list
 *
 * */
void freeCommandLineList(commandLinePtr head) {
    commandLinePtr curr = head;

    if(head == NULL){ /*if we got an empty list*/
        /*so we have no memory to free*/
        return;
    }
    /*run over the list*/
    while(curr) {
        curr = head->next;
        freeTokenList(head->tokenListHead); /*free the line token list*/
        free(head);
        head = curr;
    }
}


/**
 * addExternReference
 * 
 * The function add a new node to a linked list of ExternReference
 * 
 * params:
 * externReferencesHead - the head of the ExternReference linked list
 * name - the name of the new ExternalReference node
 * address - the address of the new ExternReference  node
 *
 * */
void addExternReference(externReferencePtr *externReferencesHead, char *name, int address) {
    externReferencePtr curr = *externReferencesHead;
    externReferencePtr new = (externReferencePtr) calloc(1, sizeof(externReference));
    checkFail(new);
    strcpy(new->name, name);
    new->address = address;
    new->next = NULL;

    if(curr == NULL) {
        *externReferencesHead = new;
        return;
    }
    while(curr->next) {
        curr = curr->next;
    }
    curr->next = new;
}


/**
 * freeExternReferenceList
 *
 * This function gets a externReference list head and frees it
 *
 * params:
 * head - a pointer to the head of the externReference list
 *
 * */
void freeExternReferenceList(externReferencePtr head) {
    externReferencePtr curr = head;

    if(head == NULL){ /*if we got an empty list*/
        /*so we have no memory to free*/
        return;
    }
    /*run over the list*/
    while(curr) {
        curr = head->next;
        free(head);
        head = curr;
    }
}
