#ifndef COWVM_BUILDER_H
#define COWVM_BUILDER_H

#include <stdint.h>
#include "type.h"
#include "value.h"
#include "block.h"
#include "../misc/data/array.h"

typedef struct {
    Array blocks;
    CowBlock current_block;
    Array values;
} _CowBuilder;

typedef _CowBuilder *CowBuilder;

CowBlock cow_builder_create_block(_CowBuilder *builder);

CowValue cow_builder_get_arg(CowBuilder builder, size_t index);

CowBlock cow_builder_set_current_block(CowBuilder builder, CowBlock block);

CowValue cow_builder_const_i64(_CowBuilder *builder, int64_t value);

CowValue cow_builder_const_i32(_CowBuilder *builder, int32_t value);

CowValue cow_builder_const_i16(_CowBuilder *builder, int16_t value);

CowValue cow_builder_const_i8(_CowBuilder *builder, int8_t value);

CowValue cow_builder_const_f64(_CowBuilder *builder, double value);

CowValue cow_builder_const_f32(_CowBuilder *builder, float value);

CowValue cow_builder_const_ptr(CowBuilder builder, void *ptr);


CowValue cow_builder_add(_CowBuilder *builder, CowValue first, CowValue second);

CowValue cow_builder_sub(_CowBuilder *builder, CowValue first, CowValue second);

CowValue cow_builder_div(_CowBuilder *builder, CowValue first, CowValue second);

CowValue cow_builder_idiv(_CowBuilder *builder, CowValue first, CowValue second);

CowValue cow_builder_mul(_CowBuilder *builder, CowValue first, CowValue second);


CowValue cow_builder_eq(_CowBuilder *builder, CowValue first, CowValue second);

CowValue cow_builder_diff(_CowBuilder *builder, CowValue first, CowValue second);

CowValue cow_builder_smaller(_CowBuilder *builder, CowValue first, CowValue second);

CowValue cow_builder_smaller_eq(_CowBuilder *builder, CowValue first, CowValue second);

CowValue cow_builder_larger(_CowBuilder *builder, CowValue first, CowValue second);

CowValue cow_builder_larger_eq(_CowBuilder *builder, CowValue first, CowValue second);

CowValue cow_builder_not(CowBuilder builder, CowValue value);


void cow_builder_br(_CowBuilder *builder, CowBlock block);

void cow_builder_cond_br(_CowBuilder *builder, CowValue cond, CowBlock block_true, CowBlock block_false);


CowValue cow_builder_load(_CowBuilder *builder, CowValue value, CowType type);

void cow_builder_store(_CowBuilder *builder, CowValue addr, CowValue value_to_store);


CowValue
cow_builder_call_ptr(CowBuilder builder, CowType call_return, CowValue func_to_call, CowValue *args, size_t args_count);

void cow_builder_ret(_CowBuilder *builder, CowValue value);

void cow_builder_ret_void(_CowBuilder *builder);


#endif //COWVM_BUILDER_H
