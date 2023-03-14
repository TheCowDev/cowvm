cmake_minimum_required(VERSION 3.23)
project(cowvm C)

set(CMAKE_C_STANDARD 11)

add_executable(cowvm cowvm/main.c
        cowvm/cow.h
        cowvm/lang/opcode.h
        cowvm/lang/instr_builder.h
        cowvm/lang/type.h
        cowvm/lang/value.h
        cowvm/lang/block.h
        cowvm/misc/data/memory.h
        cowvm/lang/instruction.h)
