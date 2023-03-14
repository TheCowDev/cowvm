#ifndef COWVM_BUILDER_H
#define COWVM_BUILDER_H

#include <stdint.h>
#include "type.h"
#include "value.h"
#include "block.h"
#include "../misc/data/array.h"

typedef struct {
    Array blocks;
} InstrBuilder;

CowValue cow_builder_add(InstrBuilder *builder, CowValue first, CowValue second);

CowValue cow_builder_sub(InstrBuilder *builder, CowValue first, CowValue second);

CowValue cow_builder_div(InstrBuilder *builder, CowValue first, CowValue second);

CowValue cow_builder_idiv(InstrBuilder *builder, CowValue first, CowValue second);

CowValue cow_builder_mul(InstrBuilder *builder, CowValue first, CowValue second);


CowValue cow_builder_eq(InstrBuilder *builder, CowValue first, CowValue second);

CowValue cow_builder_diff(InstrBuilder *builder, CowValue first, CowValue second);

CowValue cow_builder_smaller(InstrBuilder *builder, CowValue first, CowValue second);

CowValue cow_builder_smaller_eq(InstrBuilder *builder, CowValue first, CowValue second);

CowValue cow_builder_larger(InstrBuilder *builder, CowValue first, CowValue second);

CowValue cow_builder_larger_eq(InstrBuilder *builder, CowValue first, CowValue second);


void cow_builder_br(InstrBuilder *builder, CowBlock block);

void cow_builder_cond_br(InstrBuilder *builder, CowBlock block);


CowValue cow_builder_load(InstrBuilder *builder, CowValue value, CowType type);

CowValue cow_builder_store(InstrBuilder *builder, CowValue addr, CowValue value_to_store);


#endif //COWVM_BUILDER_H
