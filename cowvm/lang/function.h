#ifndef COWVM_FUNCTION_H
#define COWVM_FUNCTION_H

#include <stdint.h>
#include "builder.h"
#include "type.h"


typedef struct {
    char *name;
    CowType *args;
    CowType return_type;
    size_t args_count;
    _CowBuilder builder;
} _CowFunc;

typedef _CowFunc *CowFunc;

CowBuilder cow_func_get_builder(CowFunc func);

CowValue cow_builder_call_func(CowBuilder builder, CowFunc func_to_call, CowValue *args, size_t args_count);

#endif //COWVM_FUNCTION_H
