/// --------------------------------------------------------------------------------------
/// Copyright 2025 Omar Sherif Fathy
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
/// --------------------------------------------------------------------------------------

#pragma once

#include "config.h"

#ifdef __SLANG__
    #define XPU_ENUM enum
#else
    #define XPU_ENUM enum class
#endif

XPU_ENUM XPXPUEReg
{
    R0  = 0,  // (zero) hardwired with all bits set to ZERO
    R1  = 1,  // (ra) holds the return address for a call
    R2  = 2,  // (sp) stack pointer
    R3  = 3,  // (gp)
    R4  = 4,  // (tp)
    R5  = 5,  // (t0)
    R6  = 6,  // (t1)
    R7  = 7,  // (t2)
    R8  = 8,  // (s0)
    R9  = 9,  // (s1)
    R10 = 10, // (a0)
    R11 = 11, // (a1)
    R12 = 12, // (a2)
    R13 = 13, // (a3)
    R14 = 14, // (a4)
    R15 = 15, // (a5)
    R16 = 16, // (a6)
    R17 = 17, // (a7)
    R18 = 18, // (s2)
    R19 = 19, // (s3)
    R20 = 20, // (s4)
    R21 = 21, // (s5)
    R22 = 22, // (s6)
    R23 = 23, // (s7)
    R24 = 24, // (s8)
    R25 = 25, // (s9)
    R26 = 26, // (s10)
    R27 = 27, // (s11)
    R28 = 28, // (t3)
    R29 = 29, // (t4)
    R30 = 30, // (t5)
    R31 = 31, // (t6)

    RCount = 32
};

XPU_ENUM XPXPUEInstructionFormat
{
    Undefined,

    // clang-format off
    
    // ---  31              25   24        20   19        15   14  12   11        07   06              00
    R, //  +--------F7--------+ +-----rs2----+ +----rs1-----+ +--F3--+ +-----rd-----+ +------opcode------+
    I, //  +           imm[11:0]             + +----rs1-----+ +--F3--+ +-----rd-----+ +------opcode------+
    S, //  +     imm[11:5]    + +-----rs2----+ +----rs1-----+ +--F3--+ +  imm[4:0]  + +------opcode------+
    B, //  +   imm[12|10:5]   + +-----rs2----+ +----rs1-----+ +--F3--+ + imm[4:1|11]+ +------opcode------+
    U, //  +                      imm[31:12]                         + +-----rd-----+ +------opcode------+
    J, //  +                imm[20|10:1|11|19:12]                    + +-----rd-----+ +------opcode------+

    // clang-format on
};

XPU_ENUM XPXPUEInstructionType
{
    Undefined,

    LUI,    // U-Type
    AUIPC,  // U-Type
    JAL,    // J-Type
    JALR,   // I-Type
    BEQ,    // B-Type
    BNE,    // B-Type
    BLT,    // B-Type
    BGE,    // B-Type
    BLTU,   // B-Type
    BGEU,   // B-Type
    LB,     // I-Type
    LH,     // I-Type
    LW,     // I-Type
    LBU,    // I-Type
    LHU,    // I-Type
    SB,     // S-Type
    SH,     // S-Type
    SW,     // S-Type
    ADDI,   // I-Type
    SLTI,   // I-Type
    SLTIU,  // I-Type
    XORI,   // I-Type
    ORI,    // I-Type
    ANDI,   // I-Type
    SLLI,   // I-Type
    SRLI,   // I-Type
    SRAI,   // I-Type
    ADD,    // R-Type
    SUB,    // R-Type
    SLL,    // R-Type
    SLT,    // R-Type
    SLTU,   // R-Type
    XOR,    // R-Type
    SRL,    // R-Type
    SRA,    // R-Type
    OR,     // R-Type
    AND,    // R-Type
    MUL,    // R-TYPE
    MULH,   // R-TYPE
    MULHSU, // R-TYPE
    MULHU,  // R-TYPE
    DIV,    // R-TYPE
    DIVU,   // R-TYPE
    REM,    // R-TYPE
    REMU,   // R-TYPE
    FENCE,  // I-Type
    ECALL,  // I-Type
    EBREAK  // I-Type
};

XPU_ENUM XPXPUProgramExitCode
{
    Success              = 0,
    UndefinedInstruction = -1,
    Unknown              = -2,
    Ebreak               = -3,
    UnhandledSyscall     = -4,
    FenceUnimplemented   = -5,
};
