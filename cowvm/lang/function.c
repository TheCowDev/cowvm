#include "function.h"
#include "instruction.h"

void cow_func_free(CowFunc func) {
    cow_free(func->args);
    cow_builder_free(&func->builder);
}

CowBuilder cow_func_get_builder(CowFunc func) {
    return &func->builder;
}