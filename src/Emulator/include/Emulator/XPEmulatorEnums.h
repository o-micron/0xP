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

#include <Emulator/XPEmulatorConfig.h>

enum XPEmulatorEOpcode
{
    //
    XPEmulatorEOpcode_Count
};

enum XPEmulatorEAddressType
{
    //
    XPEmulatorEAddressType_Count
};

enum EReg
{
    XPEmulatorEReg0,  // (zero) hardwired with all bits set to ZERO
    XPEmulatorEReg1,  // (ra) holds the return address for a call
    XPEmulatorEReg2,  // (sp) stack pointer
    XPEmulatorEReg3,  // (gp)
    XPEmulatorEReg4,  // (tp)
    XPEmulatorEReg5,  // (t0)
    XPEmulatorEReg6,  // (t1)
    XPEmulatorEReg7,  // (t2)
    XPEmulatorEReg8,  // (s0)
    XPEmulatorEReg9,  // (s1)
    XPEmulatorEReg10, // (a0)
    XPEmulatorEReg11, // (a1)
    XPEmulatorEReg12, // (a2)
    XPEmulatorEReg13, // (a3)
    XPEmulatorEReg14, // (a4)
    XPEmulatorEReg15, // (a5)
    XPEmulatorEReg16, // (a6)
    XPEmulatorEReg17, // (a7)
    XPEmulatorEReg18, // (s2)
    XPEmulatorEReg19, // (s3)
    XPEmulatorEReg20, // (s4)
    XPEmulatorEReg21, // (s5)
    XPEmulatorEReg22, // (s6)
    XPEmulatorEReg23, // (s7)
    XPEmulatorEReg24, // (s8)
    XPEmulatorEReg25, // (s9)
    XPEmulatorEReg26, // (s10)
    XPEmulatorEReg27, // (s11)
    XPEmulatorEReg28, // (t3)
    XPEmulatorEReg29, // (t4)
    XPEmulatorEReg30, // (t5)
    XPEmulatorEReg31, // (t6)

    XPEmulatorEReg_Count
};

enum XPEmulatorEInstructionFormat
{
    XPEmulatorEInstructionFormat_Undefined,

    // clang-format off
    
    // -----------------------------------   31    27     26 25   24        20   19        15   14  12   11        07   06              00
    XPEmulatorEInstructionFormat_R,     //  +--------F7--------+ +-----rs2----+ +----rs1-----+ +--F3--+ +-----rd-----+ +------opcode------+
    XPEmulatorEInstructionFormat_I,     //  +           imm[11:0]             + +----rs1-----+ +--F3--+ +-----rd-----+ +------opcode------+
    XPEmulatorEInstructionFormat_S,     //  +     imm[11:5]    + +-----rs2----+ +----rs1-----+ +--F3--+ +  imm[4:0]  + +------opcode------+
    XPEmulatorEInstructionFormat_B,     //  +   imm[12|10:5]   + +-----rs2----+ +----rs1-----+ +--F3--+ + imm[4:1|11]+ +------opcode------+
    XPEmulatorEInstructionFormat_U,     //  +                      imm[31:12]                         + +-----rd-----+ +------opcode------+
    XPEmulatorEInstructionFormat_J,     //  +                imm[20|10:1|11|19:12]                    + +-----rd-----+ +------opcode------+

    // clang-format on
};

enum XPEmulatorEInstructionType
{
    XPEmulatorEInstructionType_Undefined,

    XPEmulatorEInstructionType_LUI,   // U-Type
    XPEmulatorEInstructionType_AUIPC, // U-Type
    XPEmulatorEInstructionType_JAL,   // J-Type
    XPEmulatorEInstructionType_JALR,  // I-Type
    XPEmulatorEInstructionType_BEQ,   // B-Type
    XPEmulatorEInstructionType_BNE,   // B-Type
    XPEmulatorEInstructionType_BLT,   // B-Type
    XPEmulatorEInstructionType_BGE,   // B-Type
    XPEmulatorEInstructionType_BLTU,  // B-Type
    XPEmulatorEInstructionType_BGEU,  // B-Type
    XPEmulatorEInstructionType_LB,    // I-Type
    XPEmulatorEInstructionType_LH,    // I-Type
    XPEmulatorEInstructionType_LW,    // I-Type
    XPEmulatorEInstructionType_LBU,   // I-Type
    XPEmulatorEInstructionType_LHU,   // I-Type
    XPEmulatorEInstructionType_SB,    // S-Type
    XPEmulatorEInstructionType_SH,    // S-Type
    XPEmulatorEInstructionType_SW,    // S-Type
    XPEmulatorEInstructionType_ADDI,  // I-Type
    XPEmulatorEInstructionType_SLTI,  // I-Type
    XPEmulatorEInstructionType_SLTIU, // I-Type
    XPEmulatorEInstructionType_XORI,  // I-Type
    XPEmulatorEInstructionType_ORI,   // I-Type
    XPEmulatorEInstructionType_ANDI,  // I-Type
    XPEmulatorEInstructionType_SLLI,  // I-Type
    XPEmulatorEInstructionType_SRLI,  // I-Type
    XPEmulatorEInstructionType_SRAI,  // I-Type
    XPEmulatorEInstructionType_ADD,   // R-Type
    XPEmulatorEInstructionType_SUB,   // R-Type
    XPEmulatorEInstructionType_SLL,   // R-Type
    XPEmulatorEInstructionType_SLT,   // R-Type
    XPEmulatorEInstructionType_SLTU,  // R-Type
    XPEmulatorEInstructionType_XOR,   // R-Type
    XPEmulatorEInstructionType_SRL,   // R-Type
    XPEmulatorEInstructionType_SRA,   // R-Type
    XPEmulatorEInstructionType_OR,    // R-Type
    XPEmulatorEInstructionType_AND,   // R-Type
#if defined(XP_EMULATOR_USE_M_EXTENSION)
    XPEmulatorEInstructionType_MUL,    // R-TYPE
    XPEmulatorEInstructionType_MULH,   // R-TYPE
    XPEmulatorEInstructionType_MULHSU, // R-TYPE
    XPEmulatorEInstructionType_MULHU,  // R-TYPE
    XPEmulatorEInstructionType_DIV,    // R-TYPE
    XPEmulatorEInstructionType_DIVU,   // R-TYPE
    XPEmulatorEInstructionType_REM,    // R-TYPE
    XPEmulatorEInstructionType_REMU,   // R-TYPE
    #if defined(XP_EMULATOR_USE_XLEN_64)
    XPEmulatorEInstructionType_MULW,  // R-TYPE
    XPEmulatorEInstructionType_DIVW,  // R-TYPE
    XPEmulatorEInstructionType_DIVUW, // R-TYPE
    XPEmulatorEInstructionType_REMW,  // R-TYPE
    XPEmulatorEInstructionType_REMUW, // R-TYPE
    #endif
#endif
    XPEmulatorEInstructionType_FENCE, // I-Type
    XPEmulatorEInstructionType_ECALL, // I-Type
    XPEmulatorEInstructionType_EBREAK // I-Type
};
