#include "gen.h"
#include "../../lang/instruction.h"

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

static void write_mov_reg_int64(ByteWriter *writer, uint8_t dest_reg, int64_t value) {
    byte_writer_uint8(writer, 0x48); // REX prefix
    byte_writer_uint8(writer, 0xB8 | (dest_reg & 0x7)); // src_register
    byte_writer_int64(writer, value); // const
}


static void write_mov_reg_to_xmm(ByteWriter *writer, uint8_t src_reg, uint8_t dest_reg) {
    byte_writer_uint8(writer, 0x66); // prefix for the xmm
    byte_writer_uint8(writer, 0x48); // REX
    byte_writer_uint8(writer, 0X0F); // opcode for extension
    byte_writer_uint8(writer, 0x6E); // opcode for movq
    byte_writer_uint8(writer, (0xC0 | (dest_reg & 0x7) << 3 | (src_reg & 0x7)));


}

static void write_add_reg_to_reg(ByteWriter *writer, uint8_t left_reg, uint8_t right_reg) {
    byte_writer_uint8(writer, 0x01); //opcode for ADD
    unsigned char modrm = 0;
    modrm |= (3 << 6); // set operation to 11 (register-to-register)
    modrm |= (right_reg << 3); // set destination register
    modrm |= left_reg; // set source register
    byte_writer_uint8(writer, modrm); // ModR/M byte specifying registers
}

static void write_ret(ByteWriter *writer) {
    byte_writer_uint8(writer, 0xC3);
}


void jit_instr(CowFunc func, CowInstr *instr) {
    ByteWriter *writer = &func->jit_func.code;

    switch (instr->opcode) {

        case COW_OPCODE_CONST_I32:
            write_mov_reg_int64(writer, REGISTER_RBX, instr->const_value);
            break;

        case COW_OPCODE_CONST_I64:
            write_mov_reg_int64(writer, REGISTER_RAX, instr->const_value);
            break;

        case COW_OPCODE_ADD:
            write_add_reg_to_reg(writer, REGISTER_RAX, REGISTER_RBX);
            break;

        case COW_OPCODE_RET:
            if (func->return_type.data_type == COW_DATA_TYPE_F64 || func->return_type.data_type == COW_DATA_TYPE_F32) {
                write_mov_reg_to_xmm(writer, REGISTER_RAX, REGISTER_XMM0);
                write_ret(writer);
            } else {
                write_ret(writer);
            }
            break;

        case COW_OPCODE_RET_VOID:
            write_ret(&func->jit_func.code);
            break;
    }

}

void jit_func(CowFunc func) {

    for (int i = 0; i < func->builder.blocks.size; ++i) {
        CowBlock current_block = (CowBlock) func->builder.blocks.data[i];

        for (int j = 0; j < current_block->instructions.size; ++j) {
            CowInstr *instr = (CowInstr *) current_block->instructions.data[j];
            jit_instr(func, instr);
        }

    }

}

void cow_x86_64_jit(CowModule module) {
    for (int i = 0; i < module->funcs.size; ++i) {
        CowFunc func = (CowFunc) module->funcs.data[i];
        jit_func(func);
    }
}