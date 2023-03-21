#include "gen.h"
#include <stdint.h>
#include <stdbool.h>
#include "../../lang/instruction.h"

typedef struct {
    CowBlock block;
    size_t offset;
    int instr_size;
} BlockToReplace;

typedef struct {
    uint8_t reg_value;
    CowValue value;
} X86Register;

#define X86_REGISTERS_COUNT 14

typedef struct {
    X86Register registers[X86_REGISTERS_COUNT];
    int stack_allocation;
    Array block_offsets_to_set;
} X86RegisterAllocator;

#define REGISTER_RAX 0 //return value
#define REGISTER_RCX 1
#define REGISTER_RDX 2
#define REGISTER_RBX 3
#define REGISTER_RSP 4 //stack pointer
#define REGISTER_RBP 5 //base pointer
#define REGISTER_RSI 6
#define REGISTER_RDI 7
#define REGISTER_R8 8
#define REGISTER_R9 9
#define REGISTER_R10 10
#define REGISTER_R11 11
#define REGISTER_R12 12
#define REGISTER_R13 13
#define REGISTER_R14 14
#define REGISTER_R15 15

#define REGISTER_XMM0 0
#define REGISTER_XMM1 1
#define REGISTER_XMM2 2
#define REGISTER_XMM3 3
#define REGISTER_XMM4 4
#define REGISTER_XMM5 5
#define REGISTER_XMM6 6
#define REGISTER_XMM7 7
#define REGISTER_XMM8 8
#define REGISTER_XMM9 9
#define REGISTER_XMM10 10
#define REGISTER_XMM11 11
#define REGISTER_XMM12 12
#define REGISTER_XMM13 13
#define REGISTER_XMM14 14
#define REGISTER_XMM15 15

void allocate_init(X86RegisterAllocator *allocator) {
    allocator->registers[0] = (X86Register) {REGISTER_RCX, NULL};
    allocator->registers[1] = (X86Register) {REGISTER_RDX, NULL};
    allocator->registers[2] = (X86Register) {REGISTER_RBX, NULL};
    allocator->registers[3] = (X86Register) {REGISTER_RSI, NULL};
    allocator->registers[4] = (X86Register) {REGISTER_RDI, NULL};
    allocator->registers[5] = (X86Register) {REGISTER_R8, NULL};
    allocator->registers[6] = (X86Register) {REGISTER_R9, NULL};
    allocator->registers[7] = (X86Register) {REGISTER_R10, NULL};
    allocator->registers[8] = (X86Register) {REGISTER_R11, NULL};
    allocator->registers[9] = (X86Register) {REGISTER_R12, NULL};
    allocator->registers[10] = (X86Register) {REGISTER_R13, NULL};
    allocator->registers[11] = (X86Register) {REGISTER_R14, NULL};
    allocator->registers[12] = (X86Register) {REGISTER_R15, NULL};
}

uint8_t allocate_register(X86RegisterAllocator *allocator, CowValue value) {
    for (size_t i = 0; i < X86_REGISTERS_COUNT; ++i) {
        if (allocator->registers[i].value == NULL) {
            allocator->registers[i].value = value;
            return allocator->registers[i].reg_value;
        }
    }

    printf("No empty register left");

    return 0;
}

bool allocator_switch(X86RegisterAllocator *allocator, CowValue old_value, CowValue new_value) {
    for (size_t i = 0; i < X86_REGISTERS_COUNT; ++i) {
        if (allocator->registers[i].value == old_value) {
            allocator->registers[i].value = new_value;
            return true;
        }
    }

    return false;
}

bool free_register(X86RegisterAllocator *allocator, CowValue value) {
    for (size_t i = 0; i < X86_REGISTERS_COUNT; ++i) {
        if (allocator->registers[i].value == value) {
            allocator->registers[i].value = NULL;
            return true;
        }
    }

    return false;
}

uint8_t get_register_for_value(X86RegisterAllocator *allocator, CowValue value) {
    for (size_t i = 0; i < X86_REGISTERS_COUNT; ++i) {
        if (allocator->registers[i].value == value) {
            return allocator->registers[i].reg_value;
        }
    }

    return 0;
}

static void write_mov_reg_int64(ByteWriter *writer, uint8_t dest_reg, int64_t value) {
    byte_writer_uint8(writer, 0x48); // REX prefix
    byte_writer_uint8(writer, 0xB8 | (dest_reg & 0x7)); // src_register
    byte_writer_int64(writer, value); // const
}

//store
static void write_mov_reg_to_mem(ByteWriter *writer, uint8_t src_reg, uint8_t dst_reg) {
    byte_writer_uint8(writer, 0x48);
    byte_writer_uint8(writer, 0x89);
    byte_writer_uint8(writer, 0x00 | ((src_reg & 0x07) << 3) | (dst_reg & 0x07));
}

//load
static void write_mov_mem_to_reg(ByteWriter *writer, uint8_t src_reg, uint8_t dst_reg) {
    byte_writer_uint8(writer, 0x48);
    byte_writer_uint8(writer, 0x8B);
    byte_writer_uint8(writer, 0x00 | ((dst_reg & 0x07) << 3) | (src_reg & 0x07));
}

static void write_mov_reg_to_reg(ByteWriter *writer, uint8_t src_reg, uint8_t dest_reg) {
    byte_writer_uint8(writer, 0x89); // opcode for mov instruction
    unsigned char modrm = 0;
    modrm |= (3 << 6); // register-to-register encoding
    modrm |= (src_reg << 3); // 3 bits for destination register
    modrm |= dest_reg; // 3 bits for source register
    byte_writer_uint8(writer, modrm); // ModR/M byte specifying registers
}


static void write_mov_reg_to_xmm(ByteWriter *writer, uint8_t src_reg, uint8_t dest_reg) {
    byte_writer_uint8(writer, 0x66); // prefix for the xmm
    byte_writer_uint8(writer, 0x48); // REX
    byte_writer_uint8(writer, 0X0F); // opcode for extension
    byte_writer_uint8(writer, 0x6E); // opcode for movq
    byte_writer_uint8(writer, (0xC0 | (dest_reg & 0x7) << 3 | (src_reg & 0x7)));
}

static void write_move_xmm_to_reg(ByteWriter *writer, uint8_t xmm, uint8_t reg) {
    byte_writer_uint8(writer, 0x66); // prefix for xmm register
    byte_writer_uint8(writer, 0x48); // REX prefix
    byte_writer_uint8(writer, 0x0F); // opcode extension
    byte_writer_uint8(writer, 0x7E);  // opcode for "movq" instruction
    byte_writer_uint8(writer, (0xC0 | (xmm & 0x7) << 3 | (reg & 0x7)));
}

static void write_add_reg_to_reg(ByteWriter *writer, uint8_t left_reg, uint8_t right_reg) {
    byte_writer_uint8(writer, 0x01); //opcode for ADD
    unsigned char modrm = 0;
    modrm |= (3 << 6); // set operation to 11 (register-to-register)
    modrm |= (right_reg << 3); // set destination register
    modrm |= left_reg; // set source register
    byte_writer_uint8(writer, modrm); // ModR/M byte specifying registers
}

static void write_sub_reg_to_reg(ByteWriter *writer, uint8_t left_reg, uint8_t right_reg) {
    byte_writer_uint8(writer, 0x29); // opcode for SUB
    unsigned char modrm = 0;
    modrm |= (3 << 6); // set operation to 11 (register-to-register)
    modrm |= (right_reg << 3); // set destination register
    modrm |= left_reg; // set source register
    byte_writer_uint8(writer, modrm); // ModR/M byte specifying registers
}

static void write_div_reg(ByteWriter *writer, uint8_t reg) {
    byte_writer_uint8(writer, 0xF7); // opcode for DIV
    unsigned char modrm = 0;
    modrm |= (3 << 6); // set operation to 11 (register-to-register)
    modrm |= (6 << 3); // set destination register (/w extension)
    modrm |= reg; // set source register
    byte_writer_uint8(writer, modrm); // ModR/M byte specifying registers
}

static void write_mod_reg_to_reg(ByteWriter *writer, uint8_t left_reg, uint8_t right_reg) {
    byte_writer_uint8(writer, 0xF7); // opcode for IDIV
    unsigned char modrm = 0;
    modrm |= (3 << 6); // set operation to 11 (register-to-register)
    modrm |= (7 << 3); // set destination register (/w extension)
    modrm |= right_reg; // set source register
    byte_writer_uint8(writer, modrm); // ModR/M byte specifying registers
}


static void write_add_reg_to_reg_xmm(ByteWriter *writer, uint8_t xmm1, uint8_t xmm2) {
    const uint8_t mod = 3;
    xmm1 &= 0x0F;
    xmm2 &= 0x0F;
    uint8_t modrm = (mod << 6) | (xmm1 << 3) | xmm2;

    if (xmm1 >= 8 || xmm2 >= 8) { //if beyond XMM8, we need the REX prefix
        uint8_t rex = 0x40 | ((xmm1 & 0x08) >> 1) | (xmm2 & 0x08);
        byte_writer_uint8(writer, rex);
    }

    byte_writer_uint8(writer, 0x0F);
    byte_writer_uint8(writer, 0x58);
    byte_writer_uint8(writer, modrm);
}

static void write_sub_reg_to_reg_xmm(ByteWriter *writer, uint8_t xmm1, uint8_t xmm2) {
    const uint8_t mod = 3;
    xmm1 &= 0x0F;
    xmm2 &= 0x0F;
    uint8_t modrm = (mod << 6) | (xmm1 << 3) | xmm2;

    if (xmm1 >= 8 || xmm2 >= 8) { // if beyond XMM8, we need the REX prefix
        uint8_t rex = 0x40 | ((xmm1 & 0x08) >> 1) | (xmm2 & 0x08);
        byte_writer_uint8(writer, rex);
    }

    byte_writer_uint8(writer, 0x0F);
    byte_writer_uint8(writer, 0x5C);
    byte_writer_uint8(writer, modrm);
}

static void write_mul_reg_to_reg_xmm(ByteWriter *writer, uint8_t xmm1, uint8_t xmm2) {
    const uint8_t mod = 3;
    xmm1 &= 0x0F;
    xmm2 &= 0x0F;
    uint8_t modrm = (mod << 6) | (xmm1 << 3) | xmm2;

    if (xmm1 >= 8 || xmm2 >= 8) { // if beyond XMM8, we need the REX prefix
        uint8_t rex = 0x40 | ((xmm1 & 0x08) >> 1) | (xmm2 & 0x08);
        byte_writer_uint8(writer, rex);
    }

    byte_writer_uint8(writer, 0x0F);
    byte_writer_uint8(writer, 0x59);
    byte_writer_uint8(writer, modrm);
}

static void write_div_reg_to_reg_xmm(ByteWriter *writer, uint8_t xmm1, uint8_t xmm2) {
    const uint8_t mod = 3;
    xmm1 &= 0x0F;
    xmm2 &= 0x0F;
    uint8_t modrm = (mod << 6) | (xmm1 << 3) | xmm2;

    if (xmm1 >= 8 || xmm2 >= 8) { // if beyond XMM8, we need the REX prefix
        uint8_t rex = 0x40 | ((xmm1 & 0x08) >> 1) | (xmm2 & 0x08);
        byte_writer_uint8(writer, rex);
    }

    byte_writer_uint8(writer, 0x0F);
    byte_writer_uint8(writer, 0x5E);
    byte_writer_uint8(writer, modrm);
}

static size_t write_jmp(ByteWriter *writer) {

    // write the jmp instruction
    byte_writer_uint8(writer, 0xE9);

    // Write the 32-bit offset
    size_t result = byte_writer_int32(writer, 0);

    return result;
}

static size_t write_cond_jmp(ByteWriter *writer, uint8_t reg) {
    //cmp reg,0
    byte_writer_uint8(writer, 0x48);
    byte_writer_uint8(writer, 0x83);
    byte_writer_uint8(writer, 0xF8 | (reg & 7));
    byte_writer_uint8(writer, 0x00);

    //jz, jump if zero
    byte_writer_uint8(writer, 0x0F);
    byte_writer_uint8(writer, 0x084);
    size_t result = byte_writer_int32(writer, 0);
    return result;
}

static void write_ret(ByteWriter *writer) {
    byte_writer_uint8(writer, 0xC3);
}


void jit_instr(CowFunc func, CowInstr *instr, X86RegisterAllocator *allocator) {
    ByteWriter *writer = &func->jit_func.code;

    switch (instr->opcode) {

        case COW_OPCODE_CONST_I32: {
            uint8_t const_reg = allocate_register(allocator, instr->gen_value);
            write_mov_reg_int64(writer, const_reg, instr->const_value);
        }
            break;

        case COW_OPCODE_CONST_I64: {
            uint8_t const_reg = allocate_register(allocator, instr->gen_value);
            write_mov_reg_int64(writer, const_reg, instr->const_value);
        }
            break;

        case COW_OPCODE_CONST_PTR: {
            uint8_t const_reg = allocate_register(allocator, instr->gen_value);
            write_mov_reg_int64(writer, const_reg, instr->const_value);
        }
            break;

        case COW_OPCODE_ADD: {
            if (cow_type_is_decimal(instr->op.left_value->type)) {
                uint8_t add_result_reg = get_register_for_value(allocator, instr->op.left_value);
                uint8_t right_reg = get_register_for_value(allocator, instr->op.right_value);

                write_mov_reg_to_xmm(writer, add_result_reg, REGISTER_XMM0);
                write_mov_reg_to_xmm(writer, right_reg, REGISTER_XMM1);

                write_add_reg_to_reg_xmm(writer, REGISTER_XMM0, REGISTER_XMM1);
                write_move_xmm_to_reg(writer, REGISTER_XMM0, add_result_reg);

                allocator_switch(allocator, instr->op.left_value, instr->gen_value);

            } else {
                uint8_t add_result_reg = get_register_for_value(allocator, instr->op.left_value);
                uint8_t right_reg = get_register_for_value(allocator, instr->op.right_value);
                allocator_switch(allocator, instr->op.left_value, instr->gen_value);

                write_add_reg_to_reg(writer, add_result_reg, right_reg);
            }
        }
            break;

        case COW_OPCODE_LOAD: {
            uint8_t load_reg = allocate_register(allocator, instr->gen_value);
            write_mov_mem_to_reg(writer, get_register_for_value(allocator, instr->load.addr), load_reg);
        }
            break;

        case COW_OPCODE_STORE: {
            write_mov_reg_to_mem(writer, get_register_for_value(allocator, instr->store.to_store),
                                 get_register_for_value(allocator, instr->store.addr));
        }
            break;

        case COW_OPCODE_RET:
            if (func->return_type.data_type == COW_DATA_TYPE_F64 || func->return_type.data_type == COW_DATA_TYPE_F32) {
                uint8_t return_register = get_register_for_value(allocator, instr->return_value);
                write_mov_reg_to_xmm(writer, return_register, REGISTER_XMM0);
                write_ret(writer);
            } else {
                write_mov_reg_to_reg(writer, get_register_for_value(allocator, instr->return_value), REGISTER_RAX);
                write_ret(writer);
            }
            break;

        case COW_OPCODE_BR: {
            size_t offset_index = write_jmp(writer);
            BlockToReplace *replace = (BlockToReplace *) cow_alloc(sizeof(BlockToReplace));
            replace->offset = offset_index;
            replace->block = instr->br.block_to_br;
            replace->instr_size = 4;
            array_add(&allocator->block_offsets_to_set, replace);
        }
            break;

        case COW_OPCODE_COND_BR: {
            size_t offset_index = write_cond_jmp(writer, get_register_for_value(allocator, instr->cond_br.cond_value));
            BlockToReplace *replace = (BlockToReplace *) cow_alloc(sizeof(BlockToReplace));
            replace->offset = offset_index;
            replace->block = instr->cond_br.block_to_br_false;
            replace->instr_size = 4;
            array_add(&allocator->block_offsets_to_set, replace);

            offset_index = write_jmp(writer);
            replace = (BlockToReplace *) cow_alloc(sizeof(BlockToReplace));
            replace->offset = offset_index;
            replace->block = instr->cond_br.block_to_br_true;
            replace->instr_size = 4;
            array_add(&allocator->block_offsets_to_set, replace);
        }
            break;

        case COW_OPCODE_RET_VOID:
            write_ret(&func->jit_func.code);
            break;
    }

}

void jit_func(CowFunc func) {
    X86RegisterAllocator allocator = (X86RegisterAllocator) {0};
    allocate_init(&allocator);

    for (int i = 0; i < func->builder.blocks.size; ++i) {
        CowBlock current_block = (CowBlock) func->builder.blocks.data[i];
        current_block->code_offset = func->jit_func.code.current_index;

        for (int j = 0; j < current_block->instructions.size; ++j) {
            CowInstr *instr = (CowInstr *) current_block->instructions.data[j];
            jit_instr(func, instr, &allocator);
        }

    }

    // replace the block offsets
    for (int i = 0; i < allocator.block_offsets_to_set.size; ++i) {
        BlockToReplace *replace = (BlockToReplace *) allocator.block_offsets_to_set.data[i];
        byte_writer_int32_at(&func->jit_func.code, replace->block->code_offset - replace->offset - replace->instr_size,
                             replace->offset);
        cow_free(replace);
    }

    array_free(&allocator.block_offsets_to_set);

}

void cow_x86_64_jit(CowModule module) {
    for (int i = 0; i < module->funcs.size; ++i) {
        CowFunc func = (CowFunc) module->funcs.data[i];
        jit_func(func);
    }
}