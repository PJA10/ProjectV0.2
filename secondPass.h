

#ifndef CODE_SECONDPASS_H
#define CODE_SECONDPASS_H

#include <stdio.h>
#include "constants.h"
#include "dataStructures.h"

int secondPass(commandLinePtr, externReferencePtr *);
int handleEntryCommand(commandLinePtr entryCommandLine);
int handleStructAddressing(tokenPtr operand, externReferencePtr *externReferenceHead);
int handleDirectAddressing(tokenPtr operand, externReferencePtr *externReferenceHead);
int handleImmediateAddressing(tokenPtr operand);
int codeOperand(int operandAddressingMode, tokenPtr operandToken, int whatOperand,  externReferencePtr *externReferenceHead);
int handleRegisterAddressing(tokenPtr operand, int whatOperand);
int handleSecondPassActionCommands(commandLinePtr actionCommandLine,  externReferencePtr *externReferenceHead);


#endif
