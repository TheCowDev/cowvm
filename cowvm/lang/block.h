#ifndef COWVM_BLOCK_H
#define COWVM_BLOCK_H

#include "../misc/data/array.h"


typedef struct {
    Array instructions;
} _CowBlock;

typedef _CowBlock *CowBlock;

#endif //COWVM_BLOCK_H
