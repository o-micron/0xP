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

#include <Emulator/XPEmulatorDecoder.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

XP_EMULATOR_EXTERN enum XPEmulatorEInstructionType
xp_emulator_decoder_decode_instruction_type(uint32_t encodedInstruction)
{
    uint32_t opcode = encodedInstruction & 0b1111111;
    uint32_t funct3 = (encodedInstruction >> 12) & 0b111;
    uint32_t funct7 = (encodedInstruction >> 25) & 0b1111111;

    if (opcode == 0b0110111) {
        return XPEmulatorEInstructionType_LUI;
    } else if (opcode == 0b0010111) {
        return XPEmulatorEInstructionType_AUIPC;
    } else if (opcode == 0b1101111) {
        return XPEmulatorEInstructionType_JAL;
    } else if (opcode == 0b1100111) {
        return XPEmulatorEInstructionType_JALR;
    } else if (opcode == 0b1100011) {
        if (funct3 == 0) {
            return XPEmulatorEInstructionType_BEQ;
        } else if (funct3 == 0b001) {
            return XPEmulatorEInstructionType_BNE;
        } else if (funct3 == 0b100) {
            return XPEmulatorEInstructionType_BLT;
        } else if (funct3 == 0b101) {
            return XPEmulatorEInstructionType_BGE;
        } else if (funct3 == 0b110) {
            return XPEmulatorEInstructionType_BLTU;
        } else if (funct3 == 0b111) {
            return XPEmulatorEInstructionType_BGEU;
        }
    } else if (opcode == 0b0000011) {
        if (funct3 == 0b000) {
            return XPEmulatorEInstructionType_LB;
        } else if (funct3 == 0b001) {
            return XPEmulatorEInstructionType_LH;
        } else if (funct3 == 0b010) {
            return XPEmulatorEInstructionType_LW;
        } else if (funct3 == 0b100) {
            return XPEmulatorEInstructionType_LBU;
        } else if (funct3 == 0b101) {
            return XPEmulatorEInstructionType_LHU;
        }
    } else if (opcode == 0b0100011) {
        if (funct3 == 0b000) {
            return XPEmulatorEInstructionType_SB;
        } else if (funct3 == 0b001) {
            return XPEmulatorEInstructionType_SH;
        } else if (funct3 == 0b010) {
            return XPEmulatorEInstructionType_SW;
        }
    } else if (opcode == 0b0010011) {
        if (funct3 == 0b000) {
            return XPEmulatorEInstructionType_ADDI;
        } else if (funct3 == 0b010) {
            return XPEmulatorEInstructionType_SLTI;
        } else if (funct3 == 0b011) {
            return XPEmulatorEInstructionType_SLTIU;
        } else if (funct3 == 0b100) {
            return XPEmulatorEInstructionType_XORI;
        } else if (funct3 == 0b110) {
            return XPEmulatorEInstructionType_ORI;
        } else if (funct3 == 0b111) {
            return XPEmulatorEInstructionType_ANDI;
        } else if (funct3 == 0b001) {
            return XPEmulatorEInstructionType_SLLI;
        } else if (funct3 == 0b101) {
            if (funct7 == 0) {
                return XPEmulatorEInstructionType_SRLI;
            } else if (funct7 == 0b0100000) {
                return XPEmulatorEInstructionType_SRAI;
            }
        }
    } else if (opcode == 0b0110011) {
        if (funct7 == 0) {
            if (funct3 == 0) {
                return XPEmulatorEInstructionType_ADD;
            } else if (funct3 == 0b001) {
                return XPEmulatorEInstructionType_SLL;
            } else if (funct3 == 0b010) {
                return XPEmulatorEInstructionType_SLT;
            } else if (funct3 == 0b011) {
                return XPEmulatorEInstructionType_SLTU;
            } else if (funct3 == 0b100) {
                return XPEmulatorEInstructionType_XOR;
            } else if (funct3 == 0b101) {
                return XPEmulatorEInstructionType_SRL;
            } else if (funct3 == 0b110) {
                return XPEmulatorEInstructionType_OR;
            } else if (funct3 == 0b111) {
                return XPEmulatorEInstructionType_AND;
            }
        } else if (funct7 == 0b0100000) {
            if (funct3 == 0b000) {
                return XPEmulatorEInstructionType_SUB;
            } else if (funct3 == 0b101) {
                return XPEmulatorEInstructionType_SRA;
            }
        }
#if defined(XP_EMULATOR_USE_M_EXTENSION)
        else if (funct7 == 0b0000001) {
            if (funct3 == 0b000) {
                return XPEmulatorEInstructionType_MUL;
            } else if (funct3 == 0b001) {
                return XPEmulatorEInstructionType_MULH;
            } else if (funct3 == 0b010) {
                return XPEmulatorEInstructionType_MULHSU;
            } else if (funct3 == 0b011) {
                return XPEmulatorEInstructionType_MULHU;
            } else if (funct3 == 0b100) {
                return XPEmulatorEInstructionType_DIV;
            } else if (funct3 == 0b101) {
                return XPEmulatorEInstructionType_DIVU;
            } else if (funct3 == 0b110) {
                return XPEmulatorEInstructionType_REM;
            } else if (funct3 == 0b111) {
                return XPEmulatorEInstructionType_REMU;
            }
        }
#endif
    }
#if defined(XP_EMULATOR_USE_M_EXTENSION)
    #if defined(XP_EMULATOR_USE_XLEN_64)
    else if (opcode == 0b0111011) {
        if (funct7 == 0b0000001) {
            if (funct3 == 0b000) {
                return XPEmulatorEInstructionType_MULW;
            } else if (funct3 == 0b100) {
                return XPEmulatorEInstructionType_DIVW
            } else if (funct3 == 0b101) {
                return XPEmulatorEInstructionType_DIVUW
            } else if (funct3 == 0b110) {
                return XPEmulatorEInstructionType_REMW
            } else if (funct3 == 0b111) {
                return XPEmulatorEInstructionType_REMUW
            }
        }
    }
    #endif
#endif
    else if (opcode == 0b0001111) {
        return XPEmulatorEInstructionType_FENCE;
    } else if (opcode == 0b1110011) {
        uint32_t imm_11_0 = (encodedInstruction >> 20) & 0b111111111111;
        if (imm_11_0 == 0b000000000000) {
            return XPEmulatorEInstructionType_ECALL;
        } else if (imm_11_0 == 0b000000000001) {
            return XPEmulatorEInstructionType_EBREAK;
        }
    }

    return -1;
}

XP_EMULATOR_EXTERN struct XPEmulatorEncodedInstruction
xp_emulator_decoder_decode_instruction(uint32_t encodedInstruction)
{
    enum XPEmulatorEInstructionType type      = xp_emulator_decoder_decode_instruction_type(encodedInstruction);
    uint32_t                        opcode    = encodedInstruction & 0b1111111;
    uint32_t                        rd        = (encodedInstruction >> 7) & 0b11111;
    uint32_t                        funct3    = (encodedInstruction >> 12) & 0b111;
    uint32_t                        imm_31_12 = (encodedInstruction >> 12) & 0b11111111111111111111;
    uint32_t                        rs1       = (encodedInstruction >> 15) & 0b11111;
    uint32_t                        rs2       = (encodedInstruction >> 20) & 0b11111;
    uint32_t                        imm_11_0  = (encodedInstruction >> 20) & 0b111111111111;
    uint32_t                        funct7    = (encodedInstruction >> 25) & 0b1111111;

    // Decode the base type fields
    uint32_t imml2105 = (encodedInstruction >> 25) & 0b1111111;
    uint32_t imm4111  = (encodedInstruction >> 7) & 0b11111;

    uint32_t imm12  = (imml2105 & 0b1000000) >> 6;
    uint32_t imm105 = imml2105 & 0b0111111;
    uint32_t imm41  = (imm4111 & 0b11110) >> 1;
    uint32_t imm11  = imm4111 & 0b00001;

    // Merge and sign extend the innediate
    uint32_t imm = (imm12 << 12) | (imm11 << 11) | (imm105 << 5) | (imm41 << 1);
    imm          = (imm << 19) >> 19;

    struct XPEmulatorEncodedInstruction instr = { 0 };
    instr.type                                = XPEmulatorEInstructionType_Undefined;

    if (opcode == 0b0110111) {
        instr.type       = XPEmulatorEInstructionType_LUI;
        instr.LUI.opcode = opcode;
        instr.LUI.rd     = rd;
        instr.LUI.imm    = imm_31_12;
    } else if (opcode == 0b0010111) {
        instr.type         = XPEmulatorEInstructionType_AUIPC;
        instr.AUIPC.opcode = opcode;
        instr.AUIPC.rd     = rd;
        instr.AUIPC.imm    = imm_31_12;
    } else if (opcode == 0b1101111) {
        instr.type       = XPEmulatorEInstructionType_JAL;
        instr.JAL.opcode = opcode;
        instr.JAL.rd     = rd;
        instr.JAL.imm    = imm_31_12;
    } else if (opcode == 0b1100111) {
        instr.type        = XPEmulatorEInstructionType_JALR;
        instr.JALR.opcode = opcode;
        instr.JALR.rd     = rd;
        instr.JALR.funct3 = funct3;
        instr.JALR.rs1    = rs1;
        instr.JALR.imm    = imm_11_0;
    } else if (opcode == 0b1100011) {
        if (funct3 == 0) {
            instr.type           = XPEmulatorEInstructionType_BEQ;
            instr.BEQ.opcode     = opcode;
            instr.BEQ.imm_rd     = rd;
            instr.BEQ.funct3     = funct3;
            instr.BEQ.rs1        = rs1;
            instr.BEQ.rs2        = rs2;
            instr.BEQ.imm_funct7 = funct7;
        } else if (funct3 == 1) {
            instr.type           = XPEmulatorEInstructionType_BNE;
            instr.BNE.opcode     = opcode;
            instr.BNE.imm_rd     = rd;
            instr.BNE.funct3     = funct3;
            instr.BNE.rs1        = rs1;
            instr.BNE.rs2        = rs2;
            instr.BNE.imm_funct7 = funct7;
        } else if (funct3 == 4) {
            instr.type           = XPEmulatorEInstructionType_BLT;
            instr.BLT.opcode     = opcode;
            instr.BLT.imm_rd     = rd;
            instr.BLT.funct3     = funct3;
            instr.BLT.rs1        = rs1;
            instr.BLT.rs2        = rs2;
            instr.BLT.imm_funct7 = funct7;
        } else if (funct3 == 5) {
            instr.type           = XPEmulatorEInstructionType_BGE;
            instr.BGE.opcode     = opcode;
            instr.BGE.imm_rd     = rd;
            instr.BGE.funct3     = funct3;
            instr.BGE.rs1        = rs1;
            instr.BGE.rs2        = rs2;
            instr.BGE.imm_funct7 = funct7;
        } else if (funct3 == 6) {
            instr.type            = XPEmulatorEInstructionType_BLTU;
            instr.BLTU.opcode     = opcode;
            instr.BLTU.imm_rd     = rd;
            instr.BLTU.funct3     = funct3;
            instr.BLTU.rs1        = rs1;
            instr.BLTU.rs2        = rs2;
            instr.BLTU.imm_funct7 = funct7;
        } else if (funct3 == 7) {
            instr.type            = XPEmulatorEInstructionType_BGEU;
            instr.BGEU.opcode     = opcode;
            instr.BGEU.imm_rd     = rd;
            instr.BGEU.funct3     = funct3;
            instr.BGEU.rs1        = rs1;
            instr.BGEU.rs2        = rs2;
            instr.BGEU.imm_funct7 = funct7;
        }
    } else if (opcode == 0b0000011) {
        if (funct3 == 0) {
            instr.type      = XPEmulatorEInstructionType_LB;
            instr.LB.opcode = opcode;
            instr.LB.rd     = rd;
            instr.LB.funct3 = funct3;
            instr.LB.rs1    = rs1;
            instr.LB.imm    = imm_11_0;
        } else if (funct3 == 1) {
            instr.type      = XPEmulatorEInstructionType_LH;
            instr.LH.opcode = opcode;
            instr.LH.rd     = rd;
            instr.LH.funct3 = funct3;
            instr.LH.rs1    = rs1;
            instr.LH.imm    = imm_11_0;
        } else if (funct3 == 2) {
            instr.type      = XPEmulatorEInstructionType_LW;
            instr.LW.opcode = opcode;
            instr.LW.rd     = rd;
            instr.LW.funct3 = funct3;
            instr.LW.rs1    = rs1;
            instr.LW.imm    = imm_11_0;
        } else if (funct3 == 4) {
            instr.type       = XPEmulatorEInstructionType_LBU;
            instr.LBU.opcode = opcode;
            instr.LBU.rd     = rd;
            instr.LBU.funct3 = funct3;
            instr.LBU.rs1    = rs1;
            instr.LBU.imm    = imm_11_0;
        } else if (funct3 == 5) {
            instr.type       = XPEmulatorEInstructionType_LHU;
            instr.LHU.opcode = opcode;
            instr.LHU.rd     = rd;
            instr.LHU.funct3 = funct3;
            instr.LHU.rs1    = rs1;
            instr.LHU.imm    = imm_11_0;
        }
    } else if (opcode == 0b0100011) {
        if (funct3 == 0) {
            instr.type          = XPEmulatorEInstructionType_SB;
            instr.SB.opcode     = opcode;
            instr.SB.imm_rd     = rd;
            instr.SB.funct3     = funct3;
            instr.SB.rs1        = rs1;
            instr.SB.rs2        = rs2;
            instr.SB.imm_funct7 = funct7;
        } else if (funct3 == 1) {
            instr.type          = XPEmulatorEInstructionType_SH;
            instr.SH.opcode     = opcode;
            instr.SH.imm_rd     = rd;
            instr.SH.funct3     = funct3;
            instr.SH.rs1        = rs1;
            instr.SH.rs2        = rs2;
            instr.SH.imm_funct7 = funct7;
        } else if (funct3 == 2) {
            instr.type          = XPEmulatorEInstructionType_SW;
            instr.SW.opcode     = opcode;
            instr.SW.imm_rd     = rd;
            instr.SW.funct3     = funct3;
            instr.SW.rs1        = rs1;
            instr.SW.rs2        = rs2;
            instr.SW.imm_funct7 = funct7;
        }
    } else if (opcode == 0b0010011) {
        if (funct3 == 0b001) {
            instr.type        = XPEmulatorEInstructionType_SLLI;
            instr.SLLI.opcode = opcode;
            instr.SLLI.rd     = rd;
            instr.SLLI.funct3 = funct3;
            instr.SLLI.rs1    = rs1;
            instr.SLLI.imm    = imm_11_0;
        } else if (funct3 == 0b000) {
            instr.type        = XPEmulatorEInstructionType_ADDI;
            instr.ADDI.opcode = opcode;
            instr.ADDI.rd     = rd;
            instr.ADDI.funct3 = funct3;
            instr.ADDI.rs1    = rs1;
            instr.ADDI.imm    = imm_11_0;
        } else if (funct3 == 0b010) {
            instr.type        = XPEmulatorEInstructionType_SLTI;
            instr.SLTI.opcode = opcode;
            instr.SLTI.rd     = rd;
            instr.SLTI.funct3 = funct3;
            instr.SLTI.rs1    = rs1;
            instr.SLTI.imm    = imm_11_0;
        } else if (funct3 == 0b011) {
            instr.type         = XPEmulatorEInstructionType_SLTIU;
            instr.SLTIU.opcode = opcode;
            instr.SLTIU.rd     = rd;
            instr.SLTIU.funct3 = funct3;
            instr.SLTIU.rs1    = rs1;
            instr.SLTIU.imm    = imm_11_0;
        } else if (funct3 == 0b100) {
            instr.type        = XPEmulatorEInstructionType_XORI;
            instr.XORI.opcode = opcode;
            instr.XORI.rd     = rd;
            instr.XORI.funct3 = funct3;
            instr.XORI.rs1    = rs1;
            instr.XORI.imm    = imm_11_0;
        } else if (funct3 == 0b110) {
            instr.type       = XPEmulatorEInstructionType_ORI;
            instr.ORI.opcode = opcode;
            instr.ORI.rd     = rd;
            instr.ORI.funct3 = funct3;
            instr.ORI.rs1    = rs1;
            instr.ORI.imm    = imm_11_0;
        } else if (funct3 == 0b111) {
            instr.type        = XPEmulatorEInstructionType_ANDI;
            instr.ANDI.opcode = opcode;
            instr.ANDI.rd     = rd;
            instr.ANDI.funct3 = funct3;
            instr.ANDI.rs1    = rs1;
            instr.ANDI.imm    = imm_11_0;
        } else if (funct3 == 0b101) {
            if (funct7 == 0) {
                instr.type        = XPEmulatorEInstructionType_SRLI;
                instr.SRLI.opcode = opcode;
                instr.SRLI.rd     = rd;
                instr.SRLI.funct3 = funct3;
                instr.SRLI.rs1    = rs1;
                instr.SRLI.imm    = imm_11_0;
            } else if (funct7 == 0b0100000) {
                instr.type        = XPEmulatorEInstructionType_SRAI;
                instr.SRAI.opcode = opcode;
                instr.SRAI.rd     = rd;
                instr.SRAI.funct3 = funct3;
                instr.SRAI.rs1    = rs1;
                instr.SRAI.imm    = imm_11_0;
            }
        }
    } else if (opcode == 0b0110011) {
        if (funct7 == 0) {
            if (funct3 == 0) {
                instr.type       = XPEmulatorEInstructionType_ADD;
                instr.ADD.opcode = opcode;
                instr.ADD.rd     = rd;
                instr.ADD.funct3 = funct3;
                instr.ADD.rs1    = rs1;
                instr.ADD.rs2    = rs2;
                instr.ADD.funct7 = funct7;
            } else if (funct3 == 0b001) {
                instr.type       = XPEmulatorEInstructionType_SLL;
                instr.SLL.opcode = opcode;
                instr.SLL.rd     = rd;
                instr.SLL.funct3 = funct3;
                instr.SLL.rs1    = rs1;
                instr.SLL.rs2    = rs2;
                instr.SLL.funct7 = funct7;
            } else if (funct3 == 0b010) {
                instr.type       = XPEmulatorEInstructionType_SLT;
                instr.SLT.opcode = opcode;
                instr.SLT.rd     = rd;
                instr.SLT.funct3 = funct3;
                instr.SLT.rs1    = rs1;
                instr.SLT.rs2    = rs2;
                instr.SLT.funct7 = funct7;
            } else if (funct3 == 0b011) {
                instr.type        = XPEmulatorEInstructionType_SLTU;
                instr.SLTU.opcode = opcode;
                instr.SLTU.rd     = rd;
                instr.SLTU.funct3 = funct3;
                instr.SLTU.rs1    = rs1;
                instr.SLTU.rs2    = rs2;
                instr.SLTU.funct7 = funct7;
            } else if (funct3 == 0b100) {
                instr.type       = XPEmulatorEInstructionType_XOR;
                instr.XOR.opcode = opcode;
                instr.XOR.rd     = rd;
                instr.XOR.funct3 = funct3;
                instr.XOR.rs1    = rs1;
                instr.XOR.rs2    = rs2;
                instr.XOR.funct7 = funct7;
            } else if (funct3 == 0b101) {
                instr.type       = XPEmulatorEInstructionType_SRL;
                instr.SRL.opcode = opcode;
                instr.SRL.rd     = rd;
                instr.SRL.funct3 = funct3;
                instr.SRL.rs1    = rs1;
                instr.SRL.rs2    = rs2;
                instr.SRL.funct7 = funct7;
            } else if (funct3 == 0b110) {
                instr.type      = XPEmulatorEInstructionType_OR;
                instr.OR.opcode = opcode;
                instr.OR.rd     = rd;
                instr.OR.funct3 = funct3;
                instr.OR.rs1    = rs1;
                instr.OR.rs2    = rs2;
                instr.OR.funct7 = funct7;
            } else if (funct3 == 0b111) {
                instr.type       = XPEmulatorEInstructionType_AND;
                instr.AND.opcode = opcode;
                instr.AND.rd     = rd;
                instr.AND.funct3 = funct3;
                instr.AND.rs1    = rs1;
                instr.AND.rs2    = rs2;
                instr.AND.funct7 = funct7;
            }
        } else if (funct7 == 0b0100000) {
            if (funct3 == 0b000) {
                instr.type       = XPEmulatorEInstructionType_SUB;
                instr.SUB.opcode = opcode;
                instr.SUB.rd     = rd;
                instr.SUB.funct3 = funct3;
                instr.SUB.rs1    = rs1;
                instr.SUB.rs2    = rs2;
                instr.SUB.funct7 = funct7;
            } else if (funct3 == 0b101) {
                instr.type       = XPEmulatorEInstructionType_SRA;
                instr.SRA.opcode = opcode;
                instr.SRA.rd     = rd;
                instr.SRA.funct3 = funct3;
                instr.SRA.rs1    = rs1;
                instr.SRA.rs2    = rs2;
                instr.SRA.funct7 = funct7;
            }
        }
#if defined(XP_EMULATOR_USE_M_EXTENSION)
        else if (funct7 == 0b0000001) {
            if (funct3 == 0b000) {
                instr.type       = XPEmulatorEInstructionType_MUL;
                instr.MUL.opcode = opcode;
                instr.MUL.rd     = rd;
                instr.MUL.funct3 = funct3;
                instr.MUL.rs1    = rs1;
                instr.MUL.rs2    = rs2;
                instr.MUL.funct7 = funct7;
            } else if (funct3 == 0b001) {
                instr.type        = XPEmulatorEInstructionType_MULH;
                instr.MULH.opcode = opcode;
                instr.MULH.rd     = rd;
                instr.MULH.funct3 = funct3;
                instr.MULH.rs1    = rs1;
                instr.MULH.rs2    = rs2;
                instr.MULH.funct7 = funct7;
            } else if (funct3 == 0b010) {
                instr.type          = XPEmulatorEInstructionType_MULHSU;
                instr.MULHSU.opcode = opcode;
                instr.MULHSU.rd     = rd;
                instr.MULHSU.funct3 = funct3;
                instr.MULHSU.rs1    = rs1;
                instr.MULHSU.rs2    = rs2;
                instr.MULHSU.funct7 = funct7;
            } else if (funct3 == 0b011) {
                instr.type         = XPEmulatorEInstructionType_MULHU;
                instr.MULHU.opcode = opcode;
                instr.MULHU.rd     = rd;
                instr.MULHU.funct3 = funct3;
                instr.MULHU.rs1    = rs1;
                instr.MULHU.rs2    = rs2;
                instr.MULHU.funct7 = funct7;
            } else if (funct3 == 0b100) {
                instr.type       = XPEmulatorEInstructionType_DIV;
                instr.DIV.opcode = opcode;
                instr.DIV.rd     = rd;
                instr.DIV.funct3 = funct3;
                instr.DIV.rs1    = rs1;
                instr.DIV.rs2    = rs2;
                instr.DIV.funct7 = funct7;
            } else if (funct3 == 0b101) {
                instr.type        = XPEmulatorEInstructionType_DIVU;
                instr.DIVU.opcode = opcode;
                instr.DIVU.rd     = rd;
                instr.DIVU.funct3 = funct3;
                instr.DIVU.rs1    = rs1;
                instr.DIVU.rs2    = rs2;
                instr.DIVU.funct7 = funct7;
            } else if (funct3 == 0b110) {
                instr.type       = XPEmulatorEInstructionType_REM;
                instr.REM.opcode = opcode;
                instr.REM.rd     = rd;
                instr.REM.funct3 = funct3;
                instr.REM.rs1    = rs1;
                instr.REM.rs2    = rs2;
                instr.REM.funct7 = funct7;
            } else if (funct3 == 0b111) {
                instr.type        = XPEmulatorEInstructionType_REMU;
                instr.REMU.opcode = opcode;
                instr.REMU.rd     = rd;
                instr.REMU.funct3 = funct3;
                instr.REMU.rs1    = rs1;
                instr.REMU.rs2    = rs2;
                instr.REMU.funct7 = funct7;
            }
        }
#endif
    }
#if defined(XP_EMULATOR_USE_M_EXTENSION)
    #if defined(XP_EMULATOR_USE_XLEN_64)
    else if (opcode == 0b0111011) {
        if (funct7 == 0b0000001) {
            if (funct3 == 0b000) {
                instr.type        = XPEmulatorEInstructionType_MULW;
                instr.MULW.opcode = opcode;
                instr.MULW.rd     = rd;
                instr.MULW.funct3 = funct3;
                instr.MULW.rs1    = rs1;
                instr.MULW.rs2    = rs2;
                instr.MULW.funct7 = funct7;
            } else if (funct3 == 0b100) {
                instr.type        = XPEmulatorEInstructionType_DIVW;
                instr.DIVW.opcode = opcode;
                instr.DIVW.rd     = rd;
                instr.DIVW.funct3 = funct3;
                instr.DIVW.rs1    = rs1;
                instr.DIVW.rs2    = rs2;
                instr.DIVW.funct7 = funct7;
                return XPEmulatorEInstructionType_DIVW
            } else if (funct3 == 0b101) {
                instr.type         = XPEmulatorEInstructionType_DIVUW;
                instr.DIVUW.opcode = opcode;
                instr.DIVUW.rd     = rd;
                instr.DIVUW.funct3 = funct3;
                instr.DIVUW.rs1    = rs1;
                instr.DIVUW.rs2    = rs2;
                instr.DIVUW.funct7 = funct7;
            } else if (funct3 == 0b110) {
                instr.type        = XPEmulatorEInstructionType_REMW;
                instr.REMW.opcode = opcode;
                instr.REMW.rd     = rd;
                instr.REMW.funct3 = funct3;
                instr.REMW.rs1    = rs1;
                instr.REMW.rs2    = rs2;
                instr.REMW.funct7 = funct7;
            } else if (funct3 == 0b111) {
                instr.type         = XPEmulatorEInstructionType_REMUW;
                instr.REMUW.opcode = opcode;
                instr.REMUW.rd     = rd;
                instr.REMUW.funct3 = funct3;
                instr.REMUW.rs1    = rs1;
                instr.REMUW.rs2    = rs2;
                instr.REMUW.funct7 = funct7;
            }
        }
    }
    #endif
#endif
    else if (opcode == 0b0001111) {
        instr.type         = XPEmulatorEInstructionType_FENCE;
        instr.FENCE.opcode = opcode;
        instr.FENCE.rd     = rd;
        instr.FENCE.funct3 = funct3;
        instr.FENCE.rs1    = rs1;
        instr.FENCE.succ   = (encodedInstruction >> 20) & 0b1111;
        instr.FENCE.pred   = (encodedInstruction >> 24) & 0b1111;
        instr.FENCE.fm     = (encodedInstruction >> 28) & 0b1111;
    } else if (opcode == 0b1110011) {
        if (imm_11_0 == 0b000000000000) {
            instr.type         = XPEmulatorEInstructionType_ECALL;
            instr.ECALL.opcode = opcode;
            instr.ECALL.rd     = rd;
            instr.ECALL.funct3 = funct3;
            instr.ECALL.rs1    = rs1;
            instr.ECALL.imm    = imm_11_0;
        } else if (imm_11_0 == 0b000000000001) {
            instr.type          = XPEmulatorEInstructionType_EBREAK;
            instr.EBREAK.opcode = opcode;
            instr.EBREAK.rd     = rd;
            instr.EBREAK.funct3 = funct3;
            instr.EBREAK.rs1    = rs1;
            instr.EBREAK.imm    = imm_11_0;
        }
    }

    return instr;
}

//XP_EMULATOR_EXTERN uint32_t
//imm_I(struct XPEmulatorEncodedInstruction instr)
//{
//    // imm[11:0] = inst[31:20]
//    return ((int32_t)(int32_t)(instr.instruction.value & 0xfff00000)) >> 20; // right shift as signed?
//}

XP_EMULATOR_EXTERN uint32_t
imm_S(struct XPEmulatorEncodedInstruction instr)
{
    // imm[11:5] = inst[31:25], imm[4:0] = inst[11:7]
    return ((int32_t)(int32_t)(instr.instruction.value & 0xfe000000) >> 20) | ((instr.instruction.value >> 7) & 0x1f);
}

XP_EMULATOR_EXTERN uint32_t
imm_B(struct XPEmulatorEncodedInstruction instr)
{
    // imm[12|10:5|4:1|11] = inst[31|30:25|11:8|7]
    return ((int32_t)(int32_t)(instr.instruction.value & 0x80000000) >> 19) |
           ((instr.instruction.value & 0x80) << 4)     // imm[11]
           | ((instr.instruction.value >> 20) & 0x7e0) // imm[10:5]
           | ((instr.instruction.value >> 7) & 0x1e);  // imm[4:1]
}

XP_EMULATOR_EXTERN uint32_t
imm_U(struct XPEmulatorEncodedInstruction instr)
{
    // imm[31:12] = inst[31:12]
    return (int32_t)(int32_t)(instr.instruction.value & 0xfffff000);
}

XP_EMULATOR_EXTERN int32_t
imm_J(struct XPEmulatorEncodedInstruction instr)
{
    // // imm[20|10:1|11|19:12] = inst[31|30:21|20|19:12]
    // return ((int32_t)(int32_t)(instr.instruction.value & 0x80000000) >> 11) |
    //        (instr.instruction.value & 0xff000)          // imm[19:12]
    //        | ((instr.instruction.value >> 9) & 0x800)   // imm[11]
    //        | ((instr.instruction.value >> 20) & 0x7fe); // imm[10:1]

    int32_t imm = 0;

    imm |= (instr.instruction.value & 0x80000000) >> 11; // imm[20] -> Bit 31 (sign bit)
    imm |= (instr.instruction.value & 0x7FE00000) >> 20; // imm[10:1] -> Bits 21-30
    imm |= (instr.instruction.value & 0x00100000) >> 9;  // imm[11] -> Bit 20
    imm |= (instr.instruction.value & 0x000FF000);       // imm[19:12] -> Bits 12-19

    // Sign extend imm (ensure 32-bit signed value)
    if (imm & 0x00100000) { // If imm[20] (bit 20) is set
        imm |= 0xFFE00000;  // Extend the sign to higher bits
    }

    return imm;
}

//XP_EMULATOR_EXTERN uint32_t
//shamt(struct XPEmulatorEncodedInstruction instr)
//{
//    // shamt(shift amount) only required for immediate shift instructions
//    // shamt[4:5] = imm[5:0]
//    return (uint32_t)(imm_I(instr) & 0x1f); // TODO: 0x1f / 0x3f ?
//}