#ifndef COWVM_FUNCTION_H
#define COWVM_FUNCTION_H

#include "builder.h"

typedef struct {
    char *name;
    CowType *args;
    CowType return_type;
    size_t args_count;
    _CowBuilder builder;
} _CowFunc;

typedef _CowFunc *CowFunc;

CowBuilder cow_func_get_builder(CowFunc func);

void cow_func_generate(CowFunc func);

#endif //COWVM_FUNCTION_H
