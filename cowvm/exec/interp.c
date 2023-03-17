#include "interp.h"
#include "../lang/instruction.h"

CowInterpValue eval_func(CowFunc func, CowInterpValue *values) {

    typedef struct {
        CowBlock block;
        int32_t instr_index;
        size_t gen_value_id;
        CowInterpValue *registers;
    } CallFrame;

    CallFrame frames[1200];
    size_t current_frame = 0;

    // Var to handle registers
    CowInterpValue pool_of_registers[5000];
    CowInterpValue *registers = (CowInterpValue *) &pool_of_registers;

    // Var to handle instructions
    int32_t instr_index = 0;
    CowBlock current_block = (CowBlock) func->builder.blocks.data[0];
    CowInstr *current_instr = (CowInstr *) current_block->instructions.data[0];

    for (size_t i = 0; i < func->args_count; ++i) {
        registers[i] = values[i];
    }

    while (1) {

        switch (current_instr->opcode) {

            case COW_OPCODE_CONST_I64: {
                CowInterpValue value;
                value.value_i64 = current_instr->const_value;
                registers[current_instr->gen_value->id] = value;
            }
                break;

            case COW_OPCODE_CONST_I32: {
                CowInterpValue value;
                value.value_i32 = (int32_t) current_instr->const_value;
                registers[current_instr->gen_value->id] = value;
            }
                break;

            case COW_OPCODE_CONST_I16: {
                CowInterpValue value;
                value.value_i16 = (int16_t) current_instr->const_value;
                registers[current_instr->gen_value->id] = value;
            }
                break;

            case COW_OPCODE_CONST_I8: {
                CowInterpValue value;
                value.value_i8 = (int8_t) current_instr->const_value;
                registers[current_instr->gen_value->id] = value;
            }
                break;

            case COW_OPCODE_CONST_PTR: {
                CowInterpValue value;
                value.value_ptr = (void *) current_instr->const_value;
                registers[current_instr->gen_value->id] = value;
            }
                break;

            case COW_OPCODE_ADD: {
                CowInterpValue add_value;
                add_value.value_i64 = registers[current_instr->op.left_value->id].value_i64 +
                                      registers[current_instr->op.right_value->id].value_i64;
                registers[current_instr->gen_value->id] = add_value;
            }
                break;

            case COW_OPCODE_SUB: {
                CowInterpValue add_value;
                add_value.value_i64 = registers[current_instr->op.left_value->id].value_i64 -
                                      registers[current_instr->op.right_value->id].value_i64;
                registers[current_instr->gen_value->id] = add_value;
            }
                break;

            case COW_OPCODE_MUL: {
                CowInterpValue add_value;
                add_value.value_i64 = registers[current_instr->op.left_value->id].value_i64 *
                                      registers[current_instr->op.right_value->id].value_i64;
                registers[current_instr->gen_value->id] = add_value;
            }
                break;

            case COW_OPCODE_DIV: {
                CowInterpValue add_value;
                add_value.value_i64 = registers[current_instr->op.left_value->id].value_i64 /
                                      registers[current_instr->op.right_value->id].value_i64;
                registers[current_instr->gen_value->id] = add_value;
            }
                break;

            case COW_OPCODE_IDIV: {
                CowInterpValue add_value;
                add_value.value_i64 = registers[current_instr->op.left_value->id].value_i64 /
                                      registers[current_instr->op.right_value->id].value_i64;
                registers[current_instr->gen_value->id] = add_value;
            }
                break;

            case COW_OPCODE_EQ: {
                CowInterpValue add_value;
                add_value.value_i1 = registers[current_instr->op.left_value->id].value_i64 ==
                                     registers[current_instr->op.right_value->id].value_i64;
                registers[current_instr->gen_value->id] = add_value;
            }
                break;

            case COW_OPCODE_DIFF: {
                CowInterpValue add_value;
                add_value.value_i1 = registers[current_instr->op.left_value->id].value_i64 !=
                                     registers[current_instr->op.right_value->id].value_i64;
                registers[current_instr->gen_value->id] = add_value;
            }
                break;

            case COW_OPCODE_SMALLER: {
                CowInterpValue smaller_value;
                smaller_value.value_i1 = registers[current_instr->op.left_value->id].value_i64 <
                                         registers[current_instr->op.right_value->id].value_i64;
                registers[current_instr->gen_value->id] = smaller_value;
            }
                break;

            case COW_OPCODE_SMALLER_EQ: {
                CowInterpValue smaller_eq_value;
                smaller_eq_value.value_i1 = registers[current_instr->op.left_value->id].value_i64 <=
                                            registers[current_instr->op.right_value->id].value_i64;
                registers[current_instr->gen_value->id] = smaller_eq_value;
            }
                break;

            case COW_OPCODE_LARGER: {
                CowInterpValue larger_value;
                larger_value.value_i1 = registers[current_instr->op.left_value->id].value_i64 >
                                        registers[current_instr->op.right_value->id].value_i64;
                registers[current_instr->gen_value->id] = larger_value;
            }
                break;

            case COW_OPCODE_LARGER_EQ: {
                CowInterpValue larger_eq_value;
                larger_eq_value.value_i1 = registers[current_instr->op.left_value->id].value_i64 >=
                                           registers[current_instr->op.right_value->id].value_i64;
                registers[current_instr->gen_value->id] = larger_eq_value;
            }
                break;

            case COW_OPCODE_NOT: {
                CowInterpValue not_value;
                not_value.value_i64 = !registers[current_instr->not_value->id].value_i64;
                registers[current_instr->gen_value->id] = not_value;
            }
                break;

            case COW_OPCODE_LOAD: {

                void *addr_to_load = registers[current_instr->load.addr->id].value_ptr;

                switch (current_instr->load.type.data_type) {

                    case COW_DATA_TYPE_PTR:
                    case COW_DATA_TYPE_F64:
                    case COW_DATA_TYPE_I64:
                        registers[current_instr->gen_value->id].value_i64 = *((int64_t *) addr_to_load);
                        break;

                    case COW_DATA_TYPE_F32:
                    case COW_DATA_TYPE_I32:
                        registers[current_instr->gen_value->id].value_i32 = *((int32_t *) addr_to_load);
                        break;

                    case COW_DATA_TYPE_I16:
                        registers[current_instr->gen_value->id].value_i32 = *((int16_t *) addr_to_load);
                        break;

                    case COW_DATA_TYPE_I8:
                        registers[current_instr->gen_value->id].value_i8 = *((int8_t *) addr_to_load);
                        break;

                    default:
                        break;
                }

            }
                break;

            case COW_OPCODE_STORE: {
                void *addr_to_store = registers[current_instr->store.addr->id].value_ptr;

                switch (current_instr->store.to_store->type.data_type) {

                    case COW_DATA_TYPE_PTR:
                    case COW_DATA_TYPE_F64:
                    case COW_DATA_TYPE_I64:
                        *((int64_t *) addr_to_store) = registers[current_instr->store.to_store->id].value_i64;
                        break;

                    case COW_DATA_TYPE_F32:
                    case COW_DATA_TYPE_I32:
                        *((int32_t *) addr_to_store) = registers[current_instr->store.to_store->id].value_i32;
                        break;

                    case COW_DATA_TYPE_I16:
                        *((int16_t *) addr_to_store) = registers[current_instr->store.to_store->id].value_i16;
                        break;

                    case COW_DATA_TYPE_I8:
                        *((int8_t *) addr_to_store) = registers[current_instr->store.to_store->id].value_i8;
                        break;

                    default:
                        break;
                }
            }
                break;

            case COW_OPCODE_BR: {
                current_block = current_instr->br.block_to_br;
                instr_index = -1;
            }
                break;

            case COW_OPCODE_COND_BR: {
                instr_index = -1;
                if (registers[current_instr->cond_br.cond_value->id].value_i1) {
                    current_block = current_instr->cond_br.block_to_br_true;
                } else {
                    current_block = current_instr->cond_br.block_to_br_false;
                }
            }
                break;

            case COW_OPCODE_CALL_PTR: {
                printf("Call to ptr not supported for interpreted");
            }
                break;

            case COW_OPCODE_CALL_FUNC: {
                CowInstr *old_instr = current_instr;
                CowInterpValue *old_registers = registers;

                CowFunc func_to_call = current_instr->call_func.func_value;

                CallFrame *new_frame = (CallFrame *) &frames[current_frame];
                new_frame->instr_index = instr_index;
                new_frame->registers = old_registers;
                new_frame->gen_value_id = old_instr->gen_value->id;
                new_frame->block = current_block;
                ++current_frame;

                instr_index = -1;
                current_block = (CowBlock) func_to_call->builder.blocks.data[0];
                registers += func_to_call->builder.values.size;

                for (size_t i = 0; i < func_to_call->args_count; ++i) {
                    registers[i] = old_registers[old_instr->call_func.args[i]->id];
                }
            }
                break;

            case COW_OPCODE_RET: {
                if (current_frame > 0) {
                    --current_frame;
                    CowInterpValue *value_returned = &registers[current_instr->return_value->id];

                    CallFrame *calling_frame = &frames[current_frame];
                    instr_index = calling_frame->instr_index;
                    registers = (CowInterpValue *) calling_frame->registers;
                    current_block = calling_frame->block;
                    registers[calling_frame->gen_value_id] = *value_returned;
                } else {
                    CowInterpValue return_value;
                    return_value = registers[current_instr->return_value->id];
                    return return_value;
                }
            }
                break;

            case COW_OPCODE_RET_VOID: {
                if (current_frame > 0) {
                    --current_frame;
                    CowInterpValue *value_returned = &registers[current_instr->gen_value->id];

                    CallFrame *calling_frame = &frames[current_frame];
                    instr_index = calling_frame->instr_index;
                    registers = (CowInterpValue *) calling_frame->registers;
                    current_block = calling_frame->block;
                    registers[calling_frame->gen_value_id] = *value_returned;
                } else {
                    return (CowInterpValue) {};
                }
            }
                break;
        }

        ++instr_index;
        current_instr = current_block->instructions.data[instr_index];
    }

}

CowInterpValue cow_interpret(char *name, CowModule module, CowInterpValue *values) {
    CowFunc func_to_call = cow_find_function(module, name);
    if (func_to_call != NULL) {
        return eval_func(func_to_call, values);
    }

    return (CowInterpValue) {};
}