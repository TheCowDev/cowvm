#include "interp.h"
#include <stdbool.h>
#include "../lang/function.h"
#include "../misc/data/memory.h"
#include "../lang/instruction.h"

CowInterpValue eval_func(CowFunc func, CowInterpValue *values) {


    int32_t instr_index = 0;

    CowBlock current_block = (CowBlock) func->builder.blocks.data[0];
    CowInstr *current_instr = (CowInstr *) current_block->instructions.data[0];

    CowInterpValue *registers = cow_alloc(sizeof(CowInterpValue) * func->builder.values.size);

    for (int i = 0; i < func->args_count; ++i) {
        registers[i] = values[i];
    }

    bool run = true;

    while (run) {

        switch (current_instr->opcode) {

            case COW_OPCODE_CONST_I64: {
                CowInterpValue value;
                value.value_i64 = current_instr->const_value;
                registers[current_instr->gen_value->id] = value;
            }
                break;

            case COW_OPCODE_CONST_I32: {
                CowInterpValue value;
                value.value_i32 = current_instr->const_value;
                registers[current_instr->gen_value->id] = value;
            }
                break;

            case COW_OPCODE_CONST_I16: {
                CowInterpValue value;
                value.value_i16 = current_instr->const_value;
                registers[current_instr->gen_value->id] = value;
            }
                break;

            case COW_OPCODE_CONST_I8: {
                CowInterpValue value;
                value.value_i8 = current_instr->const_value;
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
                        registers[current_instr->gen_value->id].value_i8 = *((int32_t *) addr_to_load);
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
                CowFunc func_to_call = current_instr->call_func.func_value;

                CowInterpValue *inter_values = cow_alloc(sizeof(CowInterpValue) * func_to_call->args_count);
                for (size_t i = 0; i < func->args_count; ++i) {
                    inter_values[i].value_i64 = registers[current_instr->call_func.args[i]->id].value_i64;
                }

                registers[current_instr->gen_value->id] = eval_func(func_to_call, inter_values);

                cow_free(inter_values);
            }
                break;

            case COW_OPCODE_RET: {
                CowInterpValue return_value;
                return_value = registers[current_instr->return_value->id];
                return return_value;
            }

            case COW_OPCODE_RET_VOID:
                return (CowInterpValue) {};
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
}