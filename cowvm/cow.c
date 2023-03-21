#include "cow.h"
#include "misc/data/memory.h"
#include "gen/x86_64/gen.h"

CowModule cow_module_create() {
    CowModule module = cow_calloc(sizeof(_CowModule));

    return module;
}

void cow_module_free(CowModule module) {

    for (size_t i = 0; i < module->funcs.size; ++i) {
        CowFunc func = module->funcs.data[i];
        cow_func_free(func);
    }

    array_free(&module->funcs);
}

void cow_module_jit(CowModule module) {
    cow_x86_64_jit(module);

    // Allocate executable memory
    for (size_t i = 0; i < module->funcs.size; ++i) {
        CowFunc func = module->funcs.data[i];
        char *code = func->jit_func.code.data;
        const size_t code_size = func->jit_func.code.current_index + 1;
        char *exec = cow_alloc_exec_mem(code_size);
        memcpy(exec, code, code_size);
        func->jit_func.generated_func = exec;
    }
}

CowFunc cow_create_func(CowModule module, char *name, CowType *args, size_t args_count, CowType return_type) {
    CowFunc new_func = cow_calloc(sizeof(_CowFunc));
    new_func->name = name;
    CowType *func_args = cow_alloc(sizeof(CowType) * args_count);
    memcpy(func_args, args, sizeof(CowType) * args_count);
    new_func->args = func_args;
    new_func->args_count = args_count;
    new_func->return_type = return_type;
    array_add(&module->funcs, new_func);

    CowBlock first_block = cow_builder_create_block(&new_func->builder);
    new_func->builder.current_block = first_block;
    for (size_t i = 0; i < args_count; ++i) {
        CowValue arg_value = cow_alloc(sizeof(_CowValue));
        arg_value->type = args[i];
        arg_value->id = i;
        array_add(&new_func->builder.values, arg_value);
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