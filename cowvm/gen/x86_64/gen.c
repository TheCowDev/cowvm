#include "gen.h"
#include <stdint.h>
#include <stdbool.h>
#include "../../lang/instruction.h"

typedef struct {
    CowBlock block;
    size_t offset;
    size_t instr_size;
} BlockToReplace;

typedef struct {
    uint8_t reg_value;
    CowValue value;
} X86Register;

#define X86_REGISTERS_COUNT 14
#define X86_XMM_COUNT 15

typedef struct {
    X86Register registers[X86_REGISTERS_COUNT];
    X86Register registers_xmm[X86_XMM_COUNT];
    int stack_allocation;
    Array block_offsets_to_set;
} X86RegisterAllocator;

#define REGISTER_RAX 0
#define REGISTER_RCX 1
#define REGISTER_RDX 2
#define REGISTER_RBX 3
#define REGISTER_RSP 4 //stack pointer
#define REGISTER_RBP 5 //base pointer
#define REGISTER_RDI 6
#define REGISTER_RSI 7
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

//Calling convention dependant
uint8_t int_registers_for_call[] = {REGISTER_RCX, REGISTER_RDX, REGISTER_R8, REGISTER_R9};
uint8_t float_registers_for_call[] = {REGISTER_XMM0, REGISTER_XMM1, REGISTER_XMM2, REGISTER_XMM3};

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

    allocator->registers_xmm[0] = (X86Register) {REGISTER_XMM1, NULL};
    allocator->registers_xmm[1] = (X86Register) {REGISTER_XMM2, NULL};
    allocator->registers_xmm[2] = (X86Register) {REGISTER_XMM3, NULL};
    allocator->registers_xmm[3] = (X86Register) {REGISTER_XMM4, NULL};
    allocator->registers_xmm[4] = (X86Register) {REGISTER_XMM5, NULL};
    allocator->registers_xmm[5] = (X86Register) {REGISTER_XMM6, NULL};
    allocator->registers_xmm[6] = (X86Register) {REGISTER_XMM7, NULL};
    allocator->registers_xmm[7] = (X86Register) {REGISTER_XMM8, NULL};
    allocator->registers_xmm[8] = (X86Register) {REGISTER_XMM9, NULL};
    allocator->registers_xmm[9] = (X86Register) {REGISTER_XMM10, NULL};
    allocator->registers_xmm[10] = (X86Register) {REGISTER_XMM11, NULL};
    allocator->registers_xmm[11] = (X86Register) {REGISTER_XMM12, NULL};
    allocator->registers_xmm[12] = (X86Register) {REGISTER_XMM13, NULL};
    allocator->registers_xmm[13] = (X86Register) {REGISTER_XMM14, NULL};
    allocator->registers_xmm[14] = (X86Register) {REGISTER_XMM15, NULL};
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

uint8_t allocate_xmm(X86RegisterAllocator *allocator, CowValue value) {
    for (size_t i = 0; i < X86_XMM_COUNT; ++i) {
        if (allocator->registers_xmm[i].value == NULL) {
            allocator->registers_xmm[i].value = value;
            return allocator->registers_xmm[i].reg_value;
        }
    }

    printf("No empty xmm register left");

    return 0;
}

bool assign_register_for_value(X86RegisterAllocator *allocator, uint8_t reg, CowValue value) {
    for (size_t i = 0; i < X86_REGISTERS_COUNT; ++i) {
        if (allocator->registers[i].reg_value == reg) {
            allocator->registers[i].value = value;
            return true;
        }
    }

    return false;
}

bool assign_register_for_value_xmm(X86RegisterAllocator *allocator, uint8_t reg, CowValue value) {
    for (size_t i = 0; i < X86_XMM_COUNT; ++i) {
        if (allocator->registers_xmm[i].reg_value == reg) {
            allocator->registers_xmm[i].value = value;
            return true;
        }
    }

    return false;
}

bool is_register_assigned(X86RegisterAllocator *allocator, uint8_t reg) {
    for (size_t i = 0; i < X86_REGISTERS_COUNT; ++i) {
        if (allocator->registers[i].reg_value == reg && allocator->registers[i].value != NULL) {
            return true;
        }
    }

    return false;
}

bool is_register_assigned_xmm(X86RegisterAllocator *allocator, uint8_t reg) {
    for (size_t i = 0; i < X86_XMM_COUNT; ++i) {
        if (allocator->registers_xmm[i].reg_value == reg && allocator->registers_xmm[i].value != NULL) {
            return true;
        }
    }

    return false;
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

bool allocator_switch_xmm(X86RegisterAllocator *allocator, CowValue old_value, CowValue new_value) {
    for (size_t i = 0; i < X86_XMM_COUNT; ++i) {
        if (allocator->registers_xmm[i].value == old_value) {
            allocator->registers_xmm[i].value = new_value;
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

uint8_t get_xmm_for_value(X86RegisterAllocator *allocator, CowValue value) {
    for (size_t i = 0; i < X86_XMM_COUNT; ++i) {
        if (allocator->registers_xmm[i].value == value) {
            return allocator->registers_xmm[i].reg_value;
        }
    }

    return 0;
}

static void write_push_xmm(ByteWriter *writer, uint8_t xmm) {
    // sub rsp, 16
    byte_writer_uint8(writer, 0x48); // rex.w prefix
    byte_writer_uint8(writer, 0x83); // opcode for 'sub'
    byte_writer_uint8(writer, 0xEC); // ModR/M byte for 'sub' with rsp
    byte_writer_uint8(writer, 0x10); // Immediate 16

    // movaps [rsp], xmm
    byte_writer_uint8(writer, 0x48);
    byte_writer_uint8(writer, 0x0F);
    byte_writer_uint8(writer, 0x29);
    byte_writer_uint8(writer, 0x04 | (xmm << 3));
}

static void push_shadow(ByteWriter *writer) {
    byte_writer_uint8(writer, 0x48);
    byte_writer_uint8(writer, 0x83);
    byte_writer_uint8(writer, 0xec);
    byte_writer_uint8(writer, 0x20);
}

static void pop_shadow(ByteWriter *writer) {
    byte_writer_uint8(writer, 0x48);
    byte_writer_uint8(writer, 0x83);
    byte_writer_uint8(writer, 0xc4);
    byte_writer_uint8(writer, 0x20);
}

static void write_pop_xmm(ByteWriter *writer, uint8_t xmm) {
    // movaps xmmN, [rsp]
    byte_writer_uint8(writer, 0x48);  // REX.W prefix (clear REX.R, set REX.W)
    byte_writer_uint8(writer, 0x0F); // Opcode escape byte for 'movaps'
    byte_writer_uint8(writer, 0x28); // Opcode for 'movaps'
    byte_writer_uint8(writer, 0x04 | (xmm << 3)); // ModR/M byte for 'movaps' with xmmN and [rsp]

    // movaps [rsp], xmm
    byte_writer_uint8(writer, 0x48); // REX.W prefix
    byte_writer_uint8(writer, 0x83); // Opcode for 'add'
    byte_writer_uint8(writer, 0xC4); // ModR/M byte for 'add' with rsp
    byte_writer_uint8(writer, 0x10); // Immediate 16
}

static void write_push_reg(ByteWriter *writer, uint8_t reg) {

    if (reg == REGISTER_RSP) {
        return;
    }

    if (reg < REGISTER_R8) {
        byte_writer_uint8(writer, 0x50 + reg);
    } else { //R8 and up
        byte_writer_uint8(writer, 0x41); // rex prefix for extended registers
        byte_writer_uint8(writer, 0x50 + (reg - REGISTER_R8));
    }
}

static void write_pop_reg(ByteWriter *writer, uint8_t reg) {

    if (reg == REGISTER_RSP) {
        return;
    }

    if (reg <= REGISTER_RDI) {
        byte_writer_uint8(writer, 0x58 + reg);
    } else {
        byte_writer_uint8(writer, 0x41); // rex prefix for extended registers
        byte_writer_uint8(writer, 0x58 + (reg - REGISTER_R8));
    }

}

static size_t write_mov_reg_int64(ByteWriter *writer, uint8_t dest_reg, int64_t value) {
    byte_writer_uint8(writer, 0x48); // REX prefix
    byte_writer_uint8(writer, 0xB8 | (dest_reg & 0x7)); // src_register
    return byte_writer_int64(writer, value); // const
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

    if (src_reg == dest_reg)
        return;

    unsigned char rex = 0x40; // REX prefix base value
    rex |= (1 << 3); // REX.W bit for 64-bit operation
    rex |= ((src_reg & 8) >> 1) | ((dest_reg & 8) >> 3); // REX.R and REX.B bits for register extension
    byte_writer_uint8(writer, rex); // Write REX prefix

    byte_writer_uint8(writer, 0x89); // opcode for mov instruction
    unsigned char modrm = 0;
    modrm |= (3 << 6); // register-to-register encoding
    modrm |= ((src_reg & 7) << 3); // 3 bits for destination register
    modrm |= (dest_reg & 7); // 3 bits for source register
    byte_writer_uint8(writer, modrm); // ModR/M byte specifying registers
}

static void write_mov_xmm_to_xmm(ByteWriter *writer, uint8_t src_xmm, uint8_t dest_xmm) {

    if (src_xmm == dest_xmm)
        return;

    if (src_xmm >= REGISTER_XMM8 || dest_xmm >= REGISTER_XMM8) {
        // REX prefix: 0x40 | ((dest_xmm & 8) >> 1) | ((src_xmm & 8) >> 3)
        byte_writer_uint8(writer, 0x40 | ((dest_xmm & 8) >> 1) | ((src_xmm & 8) >> 3));

        // MOVAPS instruction opcode
        byte_writer_uint8(writer, 0x0F);
        byte_writer_uint8(writer, 0x28);

        // ModR/M byte: 0xC0 | ((dest_xmm & 7) << 3) | (src_xmm & 7)
        byte_writer_uint8(writer, 0xC0 | ((dest_xmm & 7) << 3) | (src_xmm & 7));
    } else {
        // MOVAPS instruction opcode
        byte_writer_uint8(writer, 0x0F);
        byte_writer_uint8(writer, 0X28);
        byte_writer_uint8(writer, 0xC0 | (dest_xmm << 3) | src_xmm);
    }
}


static void write_mov_reg_to_xmm(ByteWriter *writer, uint8_t src_reg, uint8_t dest_xmm) {
    byte_writer_uint8(writer, 0x66); // prefix for the xmm
    byte_writer_uint8(writer, 0x48); // REX
    byte_writer_uint8(writer, 0X0F); // opcode for extension
    byte_writer_uint8(writer, 0x6E); // opcode for movq
    byte_writer_uint8(writer, (0xC0 | (dest_xmm & 0x7) << 3 | (src_reg & 0x7)));
}

static void write_move_xmm_to_reg(ByteWriter *writer, uint8_t xmm, uint8_t reg) {
    byte_writer_uint8(writer, 0x66); // prefix for xmm register
    byte_writer_uint8(writer, 0x48); // REX prefix
    byte_writer_uint8(writer, 0x0F); // opcode extension
    byte_writer_uint8(writer, 0x7E);  // opcode for "movq" instruction
    byte_writer_uint8(writer, (0xC0 | (xmm & 0x7) << 3 | (reg & 0x7)));
}

static size_t write_mov_xmm_f64(ByteWriter *writer, uint8_t xmm, int64_t value) {
    write_mov_reg_int64(writer, REGISTER_RAX, value);
    write_mov_reg_to_xmm(writer, REGISTER_RAX, xmm);
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


static void write_add_xmm_to_xmm(ByteWriter *writer, uint8_t xmm1, uint8_t xmm2) {
    uint8_t rex = 0x40;
    if (xmm1 >= 8) rex |= 0x04;
    if (xmm2 >= 8) rex |= 0x01;

    byte_writer_uint8(writer, rex);
    byte_writer_uint8(writer, 0x66);
    byte_writer_uint8(writer, 0x0F);
    byte_writer_uint8(writer, 0x58);
    byte_writer_uint8(writer, 0xC0 | ((xmm1 & 0x07) << 3) | (xmm2 & 0x07));
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

static void write_call_indirect(ByteWriter *writer, uint8_t reg) {
    byte_writer_uint8(writer, 0x48); // REX prefix for 64-bit mode
    byte_writer_uint8(writer, 0xFF); // Opcode for the CALL instruction (indirect)
    byte_writer_uint8(writer, 0xD0 | (reg & 0x07)); // ModR/M byte with register as operand
}

static size_t write_call_direct(ByteWriter *writer) {
    size_t offset = write_mov_reg_int64(writer, REGISTER_RAX, 0);
    write_call_indirect(writer, REGISTER_RAX);
    return offset;
}

static void write_ret(ByteWriter *writer) {
    byte_writer_uint8(writer, 0xC3);
}

typedef struct {
    uint8_t reg;
    bool is_xmm;
} ArgsRegisterToPop;

static void
setup_arguments_into_registers(X86RegisterAllocator *allocator, ByteWriter *writer, CowFunc func, CowValue *values,
                               size_t args_count, int *pop_counts, ArgsRegisterToPop *args_to_pop) {
    *pop_counts = 0;

    int int_count = 0;
    float float_count = 0;

    for (size_t i = 0; i < args_count; ++i) {
        uint8_t call_conv_reg = float_registers_for_call[int_count];
        if (cow_type_is_decimal(values[i]->type)) {
            if (is_register_assigned_xmm(allocator, call_conv_reg)) {
                write_push_xmm(writer, call_conv_reg);
                args_to_pop[*pop_counts].is_xmm = true;
                args_to_pop[*pop_counts].reg = call_conv_reg;
                ++(*pop_counts);
            }

            ++float_count;
        } else {
            uint8_t call_conv_reg = int_registers_for_call[int_count];
            if (is_register_assigned(allocator, call_conv_reg)) {
                write_push_reg(writer, call_conv_reg);
                args_to_pop[*pop_counts].is_xmm = false;
                args_to_pop[*pop_counts].reg = call_conv_reg;
                ++(*pop_counts);
            }

            ++int_count;
        }
    }
}

void clear_argument(ByteWriter *writer, int pop_counts, ArgsRegisterToPop *args_to_pop) {
    for (int i = 0; i < pop_counts; ++i) {
        if (args_to_pop[i].is_xmm) {
            write_pop_xmm(writer, args_to_pop[i].reg);
        } else {
            write_pop_reg(writer, args_to_pop[i].reg);
        }
    }
}


void jit_instr(CowFunc func, CowInstr *instr, X86RegisterAllocator *allocator) {
    ByteWriter *writer = &func->jit_func.code;

    switch (instr->opcode) {

        case COW_OPCODE_CONST_I32: {
            if (cow_type_is_decimal(instr->gen_value->type)) {
                uint8_t const_reg = allocate_xmm(allocator, instr->gen_value);
                write_mov_xmm_f64(writer, const_reg, instr->const_value);
            } else {
                uint8_t const_reg = allocate_register(allocator, instr->gen_value);
                write_mov_reg_int64(writer, const_reg, instr->const_value);
            }
        }
            break;

        case COW_OPCODE_CONST_I64: {
            if (cow_type_is_decimal(instr->gen_value->type)) {
                uint8_t const_reg = allocate_xmm(allocator, instr->gen_value);
                write_mov_xmm_f64(writer, const_reg, instr->const_value);
            } else {
                uint8_t const_reg = allocate_register(allocator, instr->gen_value);
                write_mov_reg_int64(writer, const_reg, instr->const_value);
            }
        }
            break;

        case COW_OPCODE_CONST_PTR: {
            uint8_t const_reg = allocate_register(allocator, instr->gen_value);
            write_mov_reg_int64(writer, const_reg, instr->const_value);
        }
            break;

        case COW_OPCODE_ADD: {
            if (cow_type_is_decimal(instr->op.left_value->type)) {
                uint8_t add_result_reg = get_xmm_for_value(allocator, instr->op.left_value);
                uint8_t right_reg = get_xmm_for_value(allocator, instr->op.right_value);
                allocator_switch_xmm(allocator, instr->op.left_value, instr->gen_value);

                write_add_xmm_to_xmm(writer, add_result_reg, right_reg);
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

        case COW_OPCODE_CALL_PTR: {
            int args_push_count = 0;
            ArgsRegisterToPop args_pop[50];

            setup_arguments_into_registers(allocator, writer, func, instr->call_ptr.args, instr->call_ptr.args_count,
                                           &args_push_count,
                                           (ArgsRegisterToPop *) &args_pop);

            int int_arg =0;
            int float_arg =0;
            for (size_t i = 0; i < instr->call_func.args_count; ++i) {
                if (cow_type_is_decimal(instr->call_func.args[i]->type)) {
                    write_mov_xmm_to_xmm(writer, get_xmm_for_value(allocator, instr->call_ptr.args[i]),
                                         float_registers_for_call[float_arg]);
                    ++float_arg;
                } else {
                    printf("%d %d\n",get_register_for_value(allocator, instr->call_ptr.args[i]), int_registers_for_call[int_arg]);
                    write_mov_reg_to_reg(writer, get_register_for_value(allocator, instr->call_ptr.args[i]),
                                         int_registers_for_call[int_arg]);
                    ++int_arg;
                }
            }

            push_shadow(writer);

            write_call_indirect(writer, get_register_for_value(allocator, instr->call_ptr.func_ptr_value));

            pop_shadow(writer);

            if (instr->call_ptr.call_return_type.data_type != COW_DATA_TYPE_VOID) {
                if (cow_type_is_decimal(instr->call_ptr.call_return_type)) {
                    uint8_t call_result_reg = allocate_xmm(allocator, instr->gen_value);
                    //move the result into the right float register
                    write_mov_xmm_to_xmm(writer, REGISTER_XMM0, call_result_reg);
                } else {
                    uint8_t call_result_reg = allocate_register(allocator, instr->gen_value);
                    //move the result into the right register
                    write_mov_reg_to_reg(writer, REGISTER_RAX, call_result_reg);
                }
            }

            clear_argument(writer, args_push_count, (ArgsRegisterToPop *) &args_pop);
        }
            break;

        case COW_OPCODE_CALL_FUNC: {
            size_t call_offset = write_call_direct(writer);
            _CowJitCallOffset *offset = cow_alloc(sizeof(_CowJitCallOffset));
            offset->offset = call_offset;
            offset->func = instr->call_func.func_value;
            array_add(&func->jit_func.direct_call_offset, offset);

            if (instr->call_ptr.call_return_type.data_type != COW_DATA_TYPE_VOID) {
                if (cow_type_is_decimal(instr->call_ptr.call_return_type)) {
                    uint8_t call_result_reg = allocate_xmm(allocator, instr->gen_value);
                    //move the result into the right float register
                    write_mov_reg_to_mem(writer, REGISTER_XMM0, call_result_reg);
                } else {
                    uint8_t call_result_reg = allocate_register(allocator, instr->gen_value);
                    //move the result into the right register
                    write_mov_reg_to_reg(writer, REGISTER_RAX, call_result_reg);
                }
            }
        }
            break;

        case COW_OPCODE_RET:
            if (func->return_type.data_type == COW_DATA_TYPE_F64 || func->return_type.data_type == COW_DATA_TYPE_F32) {
                uint8_t return_register = get_xmm_for_value(allocator, instr->return_value);
                write_mov_xmm_to_xmm(writer, return_register, REGISTER_XMM0);
                write_ret(writer);
            } else {
                uint8_t return_register = get_register_for_value(allocator, instr->return_value);
                write_mov_reg_to_reg(writer, return_register, REGISTER_RAX);
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

static void assign_arg_to_register(CowFunc func, X86RegisterAllocator *allocator) {
    //assign the arguments into specific register
    //following the x86_64 calling convention

    int integer_arg = 0;
    int float_arg = 0;
    int stack_args = 0;
    for (int i = 0; i < func->args_count; ++i) {
        CowValue arg = cow_builder_get_arg(&func->builder, i);
        int arg_reg = 0;
        if (cow_type_is_decimal(arg->type)) {
            switch (float_arg) {
                case 0:
                    arg_reg = REGISTER_XMM0;
                    break;

                case 1:
                    arg_reg = REGISTER_XMM1;
                    break;

                case 2:
                    arg_reg = REGISTER_XMM2;
                    break;

                case 3:
                    arg_reg = REGISTER_XMM3;
                    break;

                default:
                    /* argument is passed on the stack in reversed order */
                    ++stack_args;
                    break;
            }

            ++float_arg;
        } else {
            switch (integer_arg) {

                case 0:
                    arg_reg = REGISTER_RCX;
                    break;

                case 1:
                    arg_reg = REGISTER_RDX;
                    break;

                case 2:
                    arg_reg = REGISTER_R8;
                    break;

                case 3:
                    arg_reg = REGISTER_R9;
                    break;

                default:
                    /* argument is passed on the stack in reversed order */
                    ++stack_args;
                    break;
            }

            ++integer_arg;
        }

        assign_register_for_value(allocator, arg_reg, arg);
    }
}

void jit_func(CowFunc func) {
    X86RegisterAllocator allocator = (X86RegisterAllocator) {0};
    allocate_init(&allocator);

    assign_arg_to_register(func, &allocator);

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