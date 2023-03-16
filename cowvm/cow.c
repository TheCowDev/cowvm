#include "cow.h"
#include "misc/data/memory.h"

CowModule cow_module_create() {
    CowModule module = cow_calloc(sizeof(_CowModule));

    return module;
}

CowFunc cow_create_func(CowModule module, char *name, CowType *args, size_t args_count, CowType return_type) {
    CowFunc new_func = cow_alloc(sizeof(_CowFunc));
    new_func->builder = (_CowBuilder) {0};
    new_func->name = name;
    new_func->args = args;
    new_func->args_count = args_count;
    new_func->return_type = return_type;
    array_add(&module->funcs, new_func);


    CowBlock first_block = cow_builder_create_block(&new_func->builder);
    new_func->builder.current_block = first_block;
    for (size_t i = 0; i < args_count; ++i) {
        array_add(&new_func->builder.values, NULL);
    }

    return new_func;
}

CowFunc cow_find_function(CowModule module, char *name) {
    for (size_t i = 0; i < module->funcs.size; ++i) {
        CowFunc func = (CowFunc) module->funcs.data[i];
        if (strcmp(func->name, name) == 0) {
            return func;
        }
    }

    return NULL;
}