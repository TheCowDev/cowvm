#ifndef COWVM_INSTRUCTION_H
#define COWVM_INSTRUCTION_H

#include <stdint.h>
#include "value.h"
#include "opcode.h"
#include "block.h"

typedef struct {
    int opcode;

    union {

        int64_t const_value;

        struct {
            CowValue left_value;
            CowValue right_value;
        } op;

        struct {
            CowValue addr;
            CowValue to_store;
        } store;

        struct {
            CowValue addr;
            CowType type;
        } load;

        struct {
            CowValue func_value;
            CowValue *args;
            size_t args_count;
        } call;

        struct {
            CowBlock block_to_br;
        } br;

        struct {
            CowValue cond_value;
            CowBlock block_to_br_true;
            CowBlock block_to_br_false;
        } cond_br;

        CowValue return_value;
        CowValue not_value;

    };

    CowValue gen_value;

} CowInstr;

#endif //COWVM_INSTRUCTION_H
