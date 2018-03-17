

#ifndef CODE_CONSTANTS_H
#define CODE_CONSTANTS_H


#define MIN_VAL_OF_ARGS 2 /*we have to get at lest 1 name of a file + the call to the program of course*/
#define FAIL -1
#define FIRST_REAL_ARG 1 /*the first arg is the program exe name*/
#define MAX_LINE 257 /*256 +1 for '\n'*/
#define MEMORY_START_POS 100 /*the assignment instructions are listed as such*/
#define FIRST_ELEMENT 0
#define SUCCESS 1
#define TRUE 1
#define FALSE 0
#define NUM_OF_REGISTERS 8
#define MAX_LENGTH_OF_LABEL_NAME 30
#define MAX_MEMORY_LENGTH 256
#define TWO_OPERANDS LEA
#define ONE_OPERAND JSR
#define IMMEDIATE_ADDRESSING 0
#define DIRECT_ADDRESSING 1
#define STRUCT_ACCESS_ADRESSING 2
#define REGISTERS_ADDRESSING 3
#define UNKNOWN_ADDRESSING_MODE 4
#define END_OF_ADDRESSING_LIST -1
#define NUMBER_OF_ADRESSING_MODES 4
#define NO_OPERAND -1
#define VALID 1;
#define MAX_VALID_DATA_NUMBER 511
#define MIN_VALID_DATA_NUMBER -512
#define NOT_VALID_DATA_NUMBER MAX_VALID_DATA_NUMBER*2
#define NUM_OF_BITS_IN_MEMORY_WORD 10
#define MAX_VALID_IMMEDIATE_ADDRESSING_NUMBER 127
#define MIN_VALID_IMMEDIATE_ADDRESSING_NUMBER -128
#define NUM_OF_CODE_METHOD_BITS 2
#define RELOCATABLE_CODE_METHOD_MASK 2 /*10*/
#define ABSOLUTE_CODE_METHOD_MASK 0 /*00*/
#define EXTERNAL_CODE_METHOD_MASK 1 /*01*/
#define SOURCE 0
#define DESTINY 1
#define NUM_OF_REGISTER_NUMBER_BITS 4
#define NUM_OF_ACTION_COMMANDS 16

enum {ACTION_LABEL, EXTERN_LABEL, DATA_LABEL};

enum {MOV, CMP, ADD, SUB, LEA, NOT, CLR, INC, DEC, JMP, BNE, RED, PRN, JSR, RTS, STOP, /*action commands*/
    DATA, STRING, STRUCT, EXTERN, ENTRY, /*data commands*/
    UNKNOWN}; /*others*/

/*we split up the op code from the command type is self to improve maintenance, this way our actionCommandList
 * can be arranged by number of operands and not by the op code*/
enum {MOV_OP_CODE = 0, CMP_OP_CODE = 1, ADD_OP_CODE = 2, SUB_OP_CODE = 3, NOT_OP_CODE = 4, CLR_OP_CODE = 5,
    LEA_OP_CODE = 6, INC_OP_CODE = 7, DEC_OP_CODE = 8, JMP_OP_CODE = 9, BNE_OP_CODE = 10, RED_OP_CODE = 11,
    PRN_OP_CODE = 12, JSR_OP_CODE = 13, RTS_OP_CODE = 14, STOP_OP_CODE = 15};

#endif
