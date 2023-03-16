#include "function.h"
#include "instruction.h"

CowBuilder cow_func_get_builder(CowFunc func) {
    return &func->builder;
}