#ifndef COWVM_FUNCTION_H
#define COWVM_FUNCTION_H

#include <stdint.h>
#include "builder.h"
#include "type.h"
#include "../misc/data/byte_writer.h"

typedef struct {
    ByteWriter code;
    void *generated_func;
} _CowJitFunc;


typedef struct {
    char *name;
    CowType *args;
    CowType return_type;
    size_t args_count;
    _CowBuilder builder;
    _CowJitFunc jit_func;
} _CowFunc;

typedef _CowFunc *CowFunc;

void cow_func_free(CowFunc func);

CowBuilder cow_func_get_builder(CowFunc func);

CowValue cow_builder_call_func(CowBuilder builder, CowFunc func_to_call, CowValue *args, size_t args_count);

#endif //COWVM_FUNCTION_H
