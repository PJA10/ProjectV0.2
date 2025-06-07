

#ifndef CODE_SECONDPASS_H
#define CODE_SECONDPASS_H

#include <stdio.h>
#include "constants.h"
#include "dataStructures.h"

int secondPass(commandLinePtr, externReferencePtr *);
int handleSecondPassEntryCommand(commandLinePtr entryCommandLine);
int handleMatrixAddressing(tokenPtr operand, externReferencePtr *externReferencesHead);
int handleDirectAddressing(char *operand_str, externReferencePtr *externReferencesHead);
int handleImmediateAddressing(tokenPtr operand);
int codeOperand(int operandAddressingMode, tokenPtr operandToken, int whatOperand,  externReferencePtr *externReferencesHead);
int handleRegisterAddressing(tokenPtr operand, int whatOperand);
int handleSecondPassActionCommands(commandLinePtr actionCommandLine,  externReferencePtr *externReferencesHead);


#endif
