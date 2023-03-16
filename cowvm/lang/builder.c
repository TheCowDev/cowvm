#include "builder.h"
#include "block.h"
#include "instruction.h"
#include "opcode.h"

CowBlock cow_builder_create_block(_CowBuilder *builder) {
    CowBlock new_block = cow_calloc(sizeof(_CowBlock));
    array_add(&builder->blocks, new_block);
    return new_block;
}

static CowValue add_const_instr(_CowBuilder *builder, int opcode, CowType type, int64_t value) {
    CowValue result_value = cow_alloc(sizeof(_CowValue));
    result_value->id = builder->current_block->instructions.size;
    result_value->type = type;

    CowInstr *instr = cow_alloc(sizeof(CowInstr));
    instr->opcode = opcode;
    instr->const_value = value;
    array_add(&builder->current_block->instructions, instr);

    array_add(&builder->values, result_value);

    instr->gen_value = result_value;

    return result_value;
}

CowValue cow_builder_const_i64(_CowBuilder *builder, int64_t value) {
    return add_const_instr(builder, COW_OPCODE_CONST_I64, cow_type_i64(), value);
}

CowValue cow_builder_const_i32(_CowBuilder *builder, int32_t value) {
    return add_const_instr(builder, COW_OPCODE_CONST_I32, cow_type_i32(), (int32_t) value);
}

CowValue cow_builder_const_i16(_CowBuilder *builder, int16_t value) {
    return add_const_instr(builder, COW_OPCODE_CONST_I16, cow_type_i16(), (int16_t) value);
}

CowValue cow_builder_const_i8(_CowBuilder *builder, int8_t value) {
    return add_const_instr(builder, COW_OPCODE_CONST_I8, cow_type_i8(), (int8_t) value);
}

CowValue cow_builder_const_f64(_CowBuilder *builder, double value) {
    return add_const_instr(builder, COW_OPCODE_CONST_I64, cow_type_f64(), *((int64_t *) &value));
}

CowValue cow_builder_const_f32(_CowBuilder *builder, float value) {
    return add_const_instr(builder, COW_OPCODE_CONST_I32, cow_type_f64(), (int64_t) *((int32_t *) &value));
}


static CowValue add_op_instr(_CowBuilder *builder, int opcode, CowValue left, CowValue right) {
    CowValue result_value = cow_alloc(sizeof(_CowValue));
    result_value->id = builder->current_block->instructions.size;

    CowInstr *instr = cow_alloc(sizeof(CowInstr));
    instr->opcode = opcode;
    instr->op.left_value = left;
    instr->op.right_value = right;
    array_add(&builder->current_block->instructions, instr);


    result_value->type = left->type;
    array_add(&builder->values, result_value);

    instr->gen_value = result_value;
    return result_value;
}

CowValue cow_builder_add(_CowBuilder *builder, CowValue first, CowValue second) {
    return add_op_instr(builder, COW_OPCODE_ADD, first, second);
}

CowValue cow_builder_sub(_CowBuilder *builder, CowValue first, CowValue second) {
    return add_op_instr(builder, COW_OPCODE_SUB, first, second);
}

CowValue cow_builder_div(_CowBuilder *builder, CowValue first, CowValue second) {
    return add_op_instr(builder, COW_OPCODE_DIV, first, second);
}

CowValue cow_builder_idiv(_CowBuilder *builder, CowValue first, CowValue second) {
    return add_op_instr(builder, COW_OPCODE_IDIV, first, second);
}

CowValue cow_builder_mul(_CowBuilder *builder, CowValue first, CowValue second) {
    return add_op_instr(builder, COW_OPCODE_MUL, first, second);
}


CowValue cow_builder_eq(_CowBuilder *builder, CowValue first, CowValue second) {
    return add_op_instr(builder, COW_OPCODE_EQ, first, second);
}

CowValue cow_builder_diff(_CowBuilder *builder, CowValue first, CowValue second) {
    return add_op_instr(builder, COW_OPCODE_DIFF, first, second);
}

CowValue cow_builder_smaller(_CowBuilder *builder, CowValue first, CowValue second) {
    return add_op_instr(builder, COW_OPCODE_SMALLER, first, second);
}

CowValue cow_builder_smaller_eq(_CowBuilder *builder, CowValue first, CowValue second) {
    return add_op_instr(builder, COW_OPCODE_SMALLER_EQ, first, second);
}

CowValue cow_builder_larger(_CowBuilder *builder, CowValue first, CowValue second) {
    return add_op_instr(builder, COW_OPCODE_LARGER, first, second);
}

CowValue cow_builder_larger_eq(_CowBuilder *builder, CowValue first, CowValue second) {
    return add_op_instr(builder, COW_OPCODE_LARGER_EQ, first, second);
}

CowValue cow_builder_not(CowBuilder builder, CowValue value) {
    CowValue result_value = cow_alloc(sizeof(_CowValue));
    result_value->id = builder->current_block->instructions.size;

    CowInstr *instr = cow_alloc(sizeof(CowInstr));
    instr->opcode = COW_OPCODE_NOT;
    instr->not_value = value;
    array_add(&builder->current_block->instructions, instr);

    result_value->type = value->type;

    array_add(&builder->values, result_value);

    instr->gen_value = result_value;
    return result_value;
}

void cow_builder_br(_CowBuilder *builder, CowBlock block) {
    CowInstr *instr = cow_alloc(sizeof(CowInstr));
    instr->opcode = COW_OPCODE_BR;
    instr->br.block_to_br = block;
    array_add(&builder->current_block->instructions, instr);
}

void cow_builder_cond_br(_CowBuilder *builder, CowBlock block) {
    CowInstr *instr = cow_alloc(sizeof(CowInstr));
    instr->opcode = COW_OPCODE_BR;
    instr->br.block_to_br = block;
    array_add(&builder->current_block->instructions, instr);
}


CowValue cow_builder_load(_CowBuilder *builder, CowValue value, CowType type) {
    CowValue result_value = cow_alloc(sizeof(_CowValue));
    result_value->id = builder->current_block->instructions.size;

    CowInstr *instr = cow_alloc(sizeof(CowInstr));
    instr->opcode = COW_OPCODE_LOAD;
    instr->load.addr = value;
    instr->load.type = type;
    array_add(&builder->current_block->instructions, instr);

    result_value->type = type;

    array_add(&builder->values, result_value);

    instr->gen_value = result_value;
    return result_value;
}

void cow_builder_store(_CowBuilder *builder, CowValue addr, CowValue value_to_store) {
    CowInstr *instr = cow_alloc(sizeof(CowInstr));
    instr->opcode = COW_OPCODE_STORE;
    instr->store.addr = addr;
    instr->store.to_store = value_to_store;
    array_add(&builder->current_block->instructions, instr);
}

CowValue
cow_builder_call(CowBuilder builder, CowType call_return, CowValue func_to_call, CowValue *args, size_t args_count) {
    CowValue result_value = cow_alloc(sizeof(_CowValue));
    result_value->type = call_return;
    result_value->id = builder->current_block->instructions.size;

    CowInstr *instr = cow_alloc(sizeof(CowInstr));
    instr->opcode = COW_OPCODE_CALL;
    instr->call.func_value = func_to_call;
    instr->call.args = args;
    instr->call.args_count = args_count;
    array_add(&builder->current_block->instructions, instr);

    array_add(&builder->values, result_value);

    instr->gen_value = result_value;
    return result_value;
}


void cow_builder_ret(_CowBuilder *builder, CowValue value) {
    CowInstr *instr = cow_alloc(sizeof(CowInstr));
    instr->opcode = COW_OPCODE_RET;
    instr->return_value = value;
    array_add(&builder->current_block->instructions, instr);
}

void cow_builder_ret_void(_CowBuilder *builder) {
    CowInstr *instr = cow_alloc(sizeof(CowInstr));
    instr->opcode = COW_OPCODE_RET_VOID;
    array_add(&builder->current_block->instructions, instr);
}

