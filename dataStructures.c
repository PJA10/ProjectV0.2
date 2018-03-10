/*
 * Created by PJA on 3/5/2018.
*/


#include "dataStructures.h"
#include "assistantFunctions.h"


/**********TOKEN:**********/


/**
 * addToToken
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

    new = (tokenPtr) calloc(sizeof(token), 1); /*calloc memory for the new node*/
    checkFail(new);
    newString = (char *) calloc(sizeof(char), strlen(string) + 1);
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


/**********END TOKEN**********/
/**********LABEL:**********/


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
void setLabel(labelPtr label, char *name, int address, int isAction, int isExtarnal, int hasEntry) {
    label->name = name;
    label->address = address;
    label->isAction = isAction;
    label->isExtarnal = isExtarnal;
    label->hasEntry = hasEntry;
}


/**********END LABEL**********/
/**********COMMAND_LINE:**********/


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
void addCommandLine(commandLinePtr *head, int lineNumber, int commandType, int hasLabel, int sourceOperandAddressingMode, int destinyOperandAddressingMode, tokenPtr tokenListHead) {
    commandLinePtr new = (commandLinePtr) calloc(sizeof(commandLine), 1);
    commandLinePtr curr = *head;
    checkFail(new);
    new->next = NULL;
    new->lineNumber = lineNumber;
    new->commandType = commandType;
    new->hasLabel = hasLabel;
    new->sourceOperandAddressingMode = sourceOperandAddressingMode;
    new->destinyOperandAddressingMode = destinyOperandAddressingMode;
    new->tokenListHead = tokenListHead;
    if(*head == NULL) {
        *head = new;
        return;
    }
    while(curr->next) {
        curr = curr->next;
    }
    curr->next = new;
}