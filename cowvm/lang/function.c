#include "function.h"
#include "instruction.h"

void cow_func_free(CowFunc func) {
    cow_free(func->args);
    cow_builder_free(&func->builder);

    for (size_t i = 0; i < func->jit_func.direct_call_offset.size; ++i) {
        cow_free(func->jit_func.direct_call_offset.data[i]);
    }

    array_free(&func->jit_func.direct_call_offset);
}

CowBuilder cow_func_get_builder(CowFunc func) {
    return &func->builder;
}