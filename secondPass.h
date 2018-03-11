

#ifndef CODE_SECONDPASS_H
#define CODE_SECONDPASS_H

#include <stdio.h>
#include "constants.h"
#include "dataStructures.h"

int secondPass(commandLinePtr);
int handleEntryCommand(commandLinePtr entryCommandLine);
int handleStructAddressing(tokenPtr operand);
int handleDirectAddressing(tokenPtr operand);
int handleImmediateAddressing(tokenPtr operand);
int codeOperand(int operandAddressingMode, tokenPtr operandToken, int whatOperand);
int handleRegisterAddressing(tokenPtr operand, int whatOperand);


#endif
