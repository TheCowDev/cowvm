#ifndef COWVM_OPCODE_H
#define COWVM_OPCODE_H

/*
 * Cow vm instruction set
 */

#define COW_OPCODE_CONST_I128 1
#define COW_OPCODE_CONST_I64 2
#define COW_OPCODE_CONST_I32 3
#define COW_OPCODE_CONST_I16 4
#define COW_OPCODE_CONST_I8 5

#define COW_OPCODE_ADD 20
#define COW_OPCODE_SUB 21
#define COW_OPCODE_MUL 22
#define COW_OPCODE_DIV 23
#define COW_OPCODE_IDIV 24

#define COW_OPCODE_EQ 25
#define COW_OPCODE_DIFF 26
#define COW_OPCODE_LARGER 27
#define COW_OPCODE_LARGER_EQ 28
#define COW_OPCODE_SMALLER 29
#define COW_OPCODE_SMALLER_EQ 30

#define COW_OPCODE_LOAD 40
#define COW_OPCODE_STORE 41

#define COW_OPCODE_BR 50
#define COW_OPCODE_COND_BR 51

#endif //COWVM_OPCODE_H
