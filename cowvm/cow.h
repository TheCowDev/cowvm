#ifndef COWVM_COW_H
#define COWVM_COW_H

#include "lang/function.h"
#include "lang/type.h"
#include "misc/data/array.h"

typedef struct {
    Array funcs;
} _CowModule;

typedef _CowModule *CowModule;

CowModule cow_module_create();

void cow_module_jit(CowModule module);

CowFunc cow_create_func(CowModule module, char *name, CowType *args, size_t args_count, CowType return_type);

CowFunc cow_find_function(CowModule module, char *name);

#endif //COWVM_COW_H
