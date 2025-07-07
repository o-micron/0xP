#include "shared/structures.h"

#include "bus.slang"
#include "decoder.slang"
#include "instruction.slang"
#include "syscalls.slang"

uint32_t
fetch(XPXPUProcessor* processor)
{
    return xp_xpu_bus_load(&processor->bus, processor->pc, 32);
}

XPXPUEncodedInstruction
decode(uint32_t instruction)
{
    return xp_xpu_decoder_decode_instruction(instruction);
}

int
execute(XPXPUProcessor* processor, XPXPUEncodedInstruction instr)
{
    int pcIncr = 4;

    switch (instr.type) {
        case XPXPUEInstructionType::Undefined: {
            return XPXPUProgramExitCode::UndefinedInstruction;
        }
        case XPXPUEInstructionType::LUI: {
            LUI      lui                        = (LUI)instr.value;
            uint32_t dest                       = (instr.value & 0b11111111111111111111000000000000);
            processor->regs[lui.rd]             = dest;
            processor->regs[int(XPXPUEReg::R0)] = 0;
            processor->pc += pcIncr;
            return XPXPUProgramExitCode::Success;
        }
        case XPXPUEInstructionType::AUIPC: {
            AUIPC    auipc                      = (AUIPC)instr.value;
            uint32_t dest                       = processor->pc + (instr.value & 0b11111111111111111111000000000000);
            processor->regs[auipc.rd]           = dest;
            processor->regs[int(XPXPUEReg::R0)] = 0;
            processor->pc += pcIncr;
            return XPXPUProgramExitCode::Success;
        }
        case XPXPUEInstructionType::JAL: {
            JAL     jal = (JAL)instr.value;
            int32_t imm = 0;
            imm |= (instr.value & 0x80000000) >> 11;
            imm |= (instr.value & 0x7FE00000) >> 20;
            imm |= (instr.value & 0x00100000) >> 9;
            imm |= (instr.value & 0x000FF000);
            if ((imm & 0x00100000) != 0) { imm |= 0xFFE00000; }
            processor->regs[jal.rd]             = processor->pc + 4;
            pcIncr                              = imm;
            processor->regs[int(XPXPUEReg::R0)] = 0;
            processor->pc += pcIncr;
            return XPXPUProgramExitCode::Success;
        }
        case XPXPUEInstructionType::JALR: {
            JALR    jalr = (JALR)instr.value;
            int32_t imm  = (instr.value >> 20) & 0xFFF;
            if ((imm & 0x800) != 0) { imm |= 0xFFFFF000; }
            uint32_t nextInstrAddress           = processor->pc + 4;
            processor->pc                       = (processor->regs[jalr.rs1] + imm) & 0xFFFFFFFE;
            processor->regs[jalr.rd]            = nextInstrAddress;
            pcIncr                              = 0;
            processor->regs[int(XPXPUEReg::R0)] = 0;
            processor->pc += pcIncr;
            return XPXPUProgramExitCode::Success;
        }
        case XPXPUEInstructionType::BEQ: {
            BEQ     beq = (BEQ)instr.value;
            int32_t imm = 0;
            imm |= (instr.value & 0x80000000) >> 19;
            imm |= (instr.value & 0x7E000000) >> 20;
            imm |= (instr.value & 0x00000F00) >> 7;
            imm |= (instr.value & 0x00000080) << 4;
            if ((imm & 0x1000) != 0) { imm |= 0xFFFFE000; }
            if ((int32_t)processor->regs[beq.rs1] == (int32_t)processor->regs[beq.rs2]) { pcIncr = imm; }
            processor->regs[int(XPXPUEReg::R0)] = 0;
            processor->pc += pcIncr;
            return XPXPUProgramExitCode::Success;
        }
        case XPXPUEInstructionType::BNE: {
            BNE     bne = (BNE)instr.value;
            int32_t imm = 0;
            imm |= (instr.value & 0x80000000) >> 19;
            imm |= (instr.value & 0x7E000000) >> 20;
            imm |= (instr.value & 0x00000F00) >> 7;
            imm |= (instr.value & 0x00000080) << 4;
            if ((imm & 0x1000) != 0) { imm |= 0xFFFFE000; }
            if ((int32_t)processor->regs[bne.rs1] != (int32_t)processor->regs[bne.rs2]) { pcIncr = imm; }
            processor->regs[int(XPXPUEReg::R0)] = 0;
            processor->pc += pcIncr;
            return XPXPUProgramExitCode::Success;
        }
        case XPXPUEInstructionType::BLT: {
            BLT     blt = (BLT)instr.value;
            int32_t imm = 0;
            imm |= (instr.value & 0x80000000) >> 19;
            imm |= (instr.value & 0x7E000000) >> 20;
            imm |= (instr.value & 0x00000F00) >> 7;
            imm |= (instr.value & 0x00000080) << 4;
            if ((imm & 0x1000) != 0) { imm |= 0xFFFFE000; }
            if ((int32_t)processor->regs[blt.rs1] < (int32_t)processor->regs[blt.rs2]) { pcIncr = imm; }
            processor->regs[int(XPXPUEReg::R0)] = 0;
            processor->pc += pcIncr;
            return XPXPUProgramExitCode::Success;
        }
        case XPXPUEInstructionType::BGE: {
            BGE     bge = (BGE)instr.value;
            int32_t imm = 0;
            imm |= (instr.value & 0x80000000) >> 19;
            imm |= (instr.value & 0x7E000000) >> 20;
            imm |= (instr.value & 0x00000F00) >> 7;
            imm |= (instr.value & 0x00000080) << 4;
            if ((imm & 0x1000) != 0) { imm |= 0xFFFFE000; }
            if ((int32_t)processor->regs[bge.rs1] >= (int32_t)processor->regs[bge.rs2]) { pcIncr = imm; }
            processor->regs[int(XPXPUEReg::R0)] = 0;
            processor->pc += pcIncr;
            return XPXPUProgramExitCode::Success;
        }
        case XPXPUEInstructionType::BLTU: {
            BLTU    bltu = (BLTU)instr.value;
            int32_t imm  = 0;
            imm |= (instr.value & 0x80000000) >> 19;
            imm |= (instr.value & 0x7E000000) >> 20;
            imm |= (instr.value & 0x00000F00) >> 7;
            imm |= (instr.value & 0x00000080) << 4;
            if ((imm & 0x1000) != 0) { imm |= 0xFFFFE000; }
            if (processor->regs[bltu.rs1] < processor->regs[bltu.rs2]) { pcIncr = imm; }
            processor->regs[int(XPXPUEReg::R0)] = 0;
            processor->pc += pcIncr;
            return XPXPUProgramExitCode::Success;
        }
        case XPXPUEInstructionType::BGEU: {
            BGEU    bgeu = (BGEU)instr.value;
            int32_t imm  = 0;
            imm |= (instr.value & 0x80000000) >> 19;
            imm |= (instr.value & 0x7E000000) >> 20;
            imm |= (instr.value & 0x00000F00) >> 7;
            imm |= (instr.value & 0x00000080) << 4;
            if ((imm & 0x1000) != 0) { imm |= 0xFFFFE000; }
            if (processor->regs[bgeu.rs1] >= processor->regs[bgeu.rs2]) { pcIncr = imm; }
            processor->regs[int(XPXPUEReg::R0)] = 0;
            processor->pc += pcIncr;
            return XPXPUProgramExitCode::Success;
        }
        case XPXPUEInstructionType::LB: {
            LB       lb                         = (LB)instr.value;
            uint32_t imm                        = ((int32_t)(int32_t)(instr.value & 0xFFF00000)) >> 20;
            uint32_t addr                       = processor->regs[lb.rs1] + (int32_t)imm;
            processor->regs[lb.rd]              = (int32_t)(int8_t)load(processor, addr, 8);
            processor->regs[int(XPXPUEReg::R0)] = 0;
            processor->pc += pcIncr;
            return XPXPUProgramExitCode::Success;
        }
        case XPXPUEInstructionType::LH: {
            LH       lh                         = (LH)instr.value;
            uint32_t imm                        = ((int32_t)(int32_t)(instr.value & 0xFFF00000)) >> 20;
            uint32_t addr                       = processor->regs[lh.rs1] + (int32_t)imm;
            processor->regs[lh.rd]              = (int32_t)(int16_t)load(processor, addr, 16);
            processor->regs[int(XPXPUEReg::R0)] = 0;
            processor->pc += pcIncr;
            return XPXPUProgramExitCode::Success;
        }
        case XPXPUEInstructionType::LW: {
            LW       lw                         = (LW)instr.value;
            uint32_t imm                        = ((int32_t)(int32_t)(instr.value & 0xFFF00000)) >> 20;
            uint32_t addr                       = processor->regs[lw.rs1] + (int32_t)imm;
            processor->regs[lw.rd]              = (int32_t)(int32_t)load(processor, addr, 32);
            processor->regs[int(XPXPUEReg::R0)] = 0;
            processor->pc += pcIncr;
            return XPXPUProgramExitCode::Success;
        }
        case XPXPUEInstructionType::LBU: {
            LBU      lbu                        = (LBU)instr.value;
            uint32_t imm                        = ((int32_t)(int32_t)(instr.value & 0xFFF00000)) >> 20;
            uint32_t addr                       = processor->regs[lbu.rs1] + (int32_t)imm;
            processor->regs[lbu.rd]             = load(processor, addr, 8);
            processor->regs[int(XPXPUEReg::R0)] = 0;
            processor->pc += pcIncr;
            return XPXPUProgramExitCode::Success;
        }
        case XPXPUEInstructionType::LHU: {
            LHU      lhu                        = (LHU)instr.value;
            uint32_t imm                        = ((int32_t)(int32_t)(instr.value & 0xFFF00000)) >> 20;
            uint32_t addr                       = processor->regs[lhu.rs1] + (int32_t)imm;
            processor->regs[lhu.rd]             = load(processor, addr, 16);
            processor->regs[int(XPXPUEReg::R0)] = 0;
            processor->pc += pcIncr;
            return XPXPUProgramExitCode::Success;
        }
        case XPXPUEInstructionType::SB: {
            SB       sb      = (SB)instr.value;
            int64_t  imm     = decode_imm_S(instr.value);
            uint32_t address = processor->regs[sb.rs1] + (int32_t)imm;
            uint32_t value   = (processor->regs[sb.rs2] & 0x000000FF);
            store(processor, address, 8, value);
            processor->regs[int(XPXPUEReg::R0)] = 0;
            processor->pc += pcIncr;
            return XPXPUProgramExitCode::Success;
        }
        case XPXPUEInstructionType::SH: {
            SH       sh      = (SH)instr.value;
            int64_t  imm     = decode_imm_S(instr.value);
            uint32_t address = processor->regs[sh.rs1] + (int32_t)imm;
            uint32_t value   = (processor->regs[sh.rs2] & 0x0000FFFF);
            store(processor, address, 16, value);
            processor->regs[int(XPXPUEReg::R0)] = 0;
            processor->pc += pcIncr;
            return XPXPUProgramExitCode::Success;
        }
        case XPXPUEInstructionType::SW: {
            SW       sw      = (SW)instr.value;
            int64_t  imm     = decode_imm_S(instr.value);
            uint32_t address = processor->regs[sw.rs1] + (int32_t)imm;
            uint32_t value   = processor->regs[sw.rs2];
            store(processor, address, 32, value);
            processor->regs[int(XPXPUEReg::R0)] = 0;
            processor->pc += pcIncr;
            return XPXPUProgramExitCode::Success;
        }
        case XPXPUEInstructionType::ADDI: {
            ADDI     addi                       = (ADDI)instr.value;
            uint32_t imm                        = ((int32_t)(int32_t)(instr.value & 0xFFF00000)) >> 20;
            processor->regs[addi.rd]            = processor->regs[addi.rs1] + (int32_t)imm;
            processor->regs[int(XPXPUEReg::R0)] = 0;
            processor->pc += pcIncr;
            return XPXPUProgramExitCode::Success;
        }
        case XPXPUEInstructionType::SLTI: {
            SLTI     slti                       = (SLTI)instr.value;
            uint32_t imm                        = ((int32_t)(int32_t)(instr.value & 0xFFF00000)) >> 20;
            processor->regs[slti.rd]            = (processor->regs[slti.rs1] < (int32_t)imm) ? 1 : 0;
            processor->regs[int(XPXPUEReg::R0)] = 0;
            processor->pc += pcIncr;
            return XPXPUProgramExitCode::Success;
        }
        case XPXPUEInstructionType::SLTIU: {
            SLTIU    sltiu                      = (SLTIU)instr.value;
            uint32_t imm                        = ((int32_t)(int32_t)(instr.value & 0xFFF00000)) >> 20;
            processor->regs[sltiu.rd]           = (processor->regs[sltiu.rs1] < imm) ? 1 : 0;
            processor->regs[int(XPXPUEReg::R0)] = 0;
            processor->pc += pcIncr;
            return XPXPUProgramExitCode::Success;
        }
        case XPXPUEInstructionType::XORI: {
            XORI     xori                       = (XORI)instr.value;
            uint32_t imm                        = ((int32_t)(int32_t)(instr.value & 0xFFF00000)) >> 20;
            processor->regs[xori.rd]            = processor->regs[xori.rs1] ^ imm;
            processor->regs[int(XPXPUEReg::R0)] = 0;
            processor->pc += pcIncr;
            return XPXPUProgramExitCode::Success;
        }
        case XPXPUEInstructionType::ORI: {
            ORI      ori                        = (ORI)instr.value;
            uint32_t imm                        = ((int32_t)(int32_t)(instr.value & 0xFFF00000)) >> 20;
            processor->regs[ori.rd]             = processor->regs[ori.rs1] | imm;
            processor->regs[int(XPXPUEReg::R0)] = 0;
            processor->pc += pcIncr;
            return XPXPUProgramExitCode::Success;
        }
        case XPXPUEInstructionType::ANDI: {
            ANDI     andi                       = (ANDI)instr.value;
            uint32_t imm                        = ((int32_t)(int32_t)(instr.value & 0xFFF00000)) >> 20;
            processor->regs[andi.rd]            = processor->regs[andi.rs1] & imm;
            processor->regs[int(XPXPUEReg::R0)] = 0;
            processor->pc += pcIncr;
            return XPXPUProgramExitCode::Success;
        }
        case XPXPUEInstructionType::SLLI: {
            SLLI     slli                       = (SLLI)instr.value;
            uint32_t imm                        = ((int32_t)(int32_t)(instr.value & 0xFFF00000)) >> 20;
            uint32_t shamt                      = (uint32_t)(imm & 0x1F);
            processor->regs[slli.rd]            = processor->regs[slli.rs1] << shamt;
            processor->regs[int(XPXPUEReg::R0)] = 0;
            processor->pc += pcIncr;
            return XPXPUProgramExitCode::Success;
        }
        case XPXPUEInstructionType::SRLI: {
            SRLI     srli                       = (SRLI)instr.value;
            uint32_t imm                        = ((int32_t)(int32_t)(instr.value & 0xFFF00000)) >> 20;
            uint32_t shamt                      = (uint32_t)(imm & 0x1F);
            processor->regs[srli.rd]            = processor->regs[srli.rs1] >> shamt;
            processor->regs[int(XPXPUEReg::R0)] = 0;
            processor->pc += pcIncr;
            return XPXPUProgramExitCode::Success;
        }
        case XPXPUEInstructionType::SRAI: {
            SRAI     srai                       = (SRAI)instr.value;
            uint32_t imm                        = ((int32_t)(int32_t)(instr.value & 0xFFF00000)) >> 20;
            uint32_t shamt                      = (uint32_t)(imm & 0x1F);
            processor->regs[srai.rd]            = (uint32_t)((int32_t)processor->regs[srai.rs1] >> shamt);
            processor->regs[int(XPXPUEReg::R0)] = 0;
            processor->pc += pcIncr;
            return XPXPUProgramExitCode::Success;
        }
        case XPXPUEInstructionType::ADD: {
            ADD add                 = (ADD)instr.value;
            processor->regs[add.rd] = (uint32_t)((int32_t)processor->regs[add.rs1] + (int32_t)processor->regs[add.rs2]);
            processor->regs[int(XPXPUEReg::R0)] = 0;
            processor->pc += pcIncr;
            return XPXPUProgramExitCode::Success;
        }
        case XPXPUEInstructionType::SUB: {
            SUB sub                 = (SUB)instr.value;
            processor->regs[sub.rd] = (uint32_t)((int32_t)processor->regs[sub.rs1] - (int32_t)processor->regs[sub.rs2]);
            processor->regs[int(XPXPUEReg::R0)] = 0;
            processor->pc += pcIncr;
            return XPXPUProgramExitCode::Success;
        }
        case XPXPUEInstructionType::SLL: {
            SLL sll                             = (SLL)instr.value;
            processor->regs[sll.rd]             = (int32_t)(processor->regs[sll.rs1] << processor->regs[sll.rs2]);
            processor->regs[int(XPXPUEReg::R0)] = 0;
            processor->pc += pcIncr;
            return XPXPUProgramExitCode::Success;
        }
        case XPXPUEInstructionType::SLT: {
            SLT slt                 = (SLT)instr.value;
            processor->regs[slt.rd] = (processor->regs[slt.rs1] < (int32_t)processor->regs[slt.rs2]) ? 1 : 0;
            processor->regs[int(XPXPUEReg::R0)] = 0;
            processor->pc += pcIncr;
            return XPXPUProgramExitCode::Success;
        }
        case XPXPUEInstructionType::SLTU: {
            SLTU sltu                           = (SLTU)instr.value;
            processor->regs[sltu.rd]            = (processor->regs[sltu.rs1] < processor->regs[sltu.rs2]) ? 1 : 0;
            processor->regs[int(XPXPUEReg::R0)] = 0;
            processor->pc += pcIncr;
            return XPXPUProgramExitCode::Success;
        }
        case XPXPUEInstructionType::XOR: {
            XOR xor                             = (XOR)instr.value;
            processor->regs[xor.rd]             = processor->regs[xor.rs1] ^ processor->regs[xor.rs2];
            processor->regs[int(XPXPUEReg::R0)] = 0;
            processor->pc += pcIncr;
            return XPXPUProgramExitCode::Success;
        }
        case XPXPUEInstructionType::SRL: {
            SRL srl                             = (SRL)instr.value;
            processor->regs[srl.rd]             = processor->regs[srl.rs1] >> processor->regs[srl.rs2];
            processor->regs[int(XPXPUEReg::R0)] = 0;
            processor->pc += pcIncr;
            return XPXPUProgramExitCode::Success;
        }
        case XPXPUEInstructionType::SRA: {
            SRA sra = (SRA)instr.value;
            processor->regs[sra.rd] =
              (uint32_t)((int32_t)processor->regs[sra.rs1] >> (int32_t)processor->regs[sra.rs2]);
            processor->regs[int(XPXPUEReg::R0)] = 0;
            processor->pc += pcIncr;
            return XPXPUProgramExitCode::Success;
        }
        case XPXPUEInstructionType::OR: {
            OR or                               = (OR)instr.value;
            processor->regs[or.rd]              = processor->regs[or.rs1] | processor->regs[or.rs2];
            processor->regs[int(XPXPUEReg::R0)] = 0;
            processor->pc += pcIncr;
            return XPXPUProgramExitCode::Success;
        }
        case XPXPUEInstructionType::AND: {
            AND and                             = (AND)instr.value;
            processor->regs[and.rd]             = processor->regs[and.rs1] & processor->regs[and.rs2];
            processor->regs[int(XPXPUEReg::R0)] = 0;
            processor->pc += pcIncr;
            return XPXPUProgramExitCode::Success;
        }
        case XPXPUEInstructionType::MUL: {
            MUL mul                             = (MUL)instr.value;
            processor->regs[mul.rd]             = (uint32_t)(processor->regs[mul.rs1] * processor->regs[mul.rs2]);
            processor->regs[int(XPXPUEReg::R0)] = 0;
            processor->pc += pcIncr;
            return XPXPUProgramExitCode::Success;
        }
        case XPXPUEInstructionType::MULH: {
            MULH mulh                           = (MULH)instr.value;
            processor->regs[mulh.rd]            = (uint32_t)(((int64_t)((int32_t)processor->regs[mulh.rs1]) *
                                                   (int64_t)((int32_t)processor->regs[mulh.rs2])) >>
                                                  32);
            processor->regs[int(XPXPUEReg::R0)] = 0;
            processor->pc += pcIncr;
            return XPXPUProgramExitCode::Success;
        }
        case XPXPUEInstructionType::MULHSU: {
            MULHSU mulhsu = (MULHSU)instr.value;
            processor->regs[mulhsu.rd] =
              (uint32_t)(((int64_t)((int32_t)processor->regs[mulhsu.rs1]) * (uint64_t)processor->regs[mulhsu.rs2]) >>
                         32);
            processor->regs[int(XPXPUEReg::R0)] = 0;
            processor->pc += pcIncr;
            return XPXPUProgramExitCode::Success;
        }
        case XPXPUEInstructionType::MULHU: {
            MULHU mulhu = (MULHU)instr.value;
            processor->regs[mulhu.rd] =
              (uint32_t)(((uint64_t)processor->regs[mulhu.rs1] * (uint64_t)processor->regs[mulhu.rs2]) >> 32);
            processor->regs[int(XPXPUEReg::R0)] = 0;
            processor->pc += pcIncr;
            return XPXPUProgramExitCode::Success;
        }
        case XPXPUEInstructionType::DIV: {
            AUIPC  auipc = (AUIPC)instr.value;
            TYPE_R div   = decode_TYPE_R(instr.value);
            if (processor->regs[div.rs2] == 0) {
                processor->regs[div.rd]             = -1;
                processor->regs[int(XPXPUEReg::R0)] = 0;
                processor->pc += pcIncr;
                return XPXPUProgramExitCode::Success;
            } else {
                const int INT32_MIN = -2147483648;
                if ((processor->regs[div.rs1] == INT32_MIN) && ((int32_t)processor->regs[div.rs2] == -1)) {
                    processor->regs[div.rd]             = (uint32_t)((int32_t)processor->regs[div.rs1]);
                    processor->regs[int(XPXPUEReg::R0)] = 0;
                    processor->pc += pcIncr;
                    return XPXPUProgramExitCode::Success;
                } else {
                    processor->regs[div.rd] =
                      (uint32_t)((int32_t)((int32_t)processor->regs[div.rs1] / (int32_t)processor->regs[div.rs2]));
                    processor->regs[int(XPXPUEReg::R0)] = 0;
                    processor->pc += pcIncr;
                    return XPXPUProgramExitCode::Success;
                }
            }
        }
        case XPXPUEInstructionType::DIVU: {
            DIVU divu = (DIVU)instr.value;
            if (processor->regs[divu.rs2] == 0) {
                processor->regs[divu.rd]            = 0xFFFFFFFF;
                processor->regs[int(XPXPUEReg::R0)] = 0;
                processor->pc += pcIncr;
                return XPXPUProgramExitCode::Success;
            } else {
                processor->regs[divu.rd]            = (uint32_t)(processor->regs[divu.rs1] / processor->regs[divu.rs2]);
                processor->regs[int(XPXPUEReg::R0)] = 0;
                processor->pc += pcIncr;
                return XPXPUProgramExitCode::Success;
            }
        }
        case XPXPUEInstructionType::REM: {
            REM rem = (REM)instr.value;
            if (processor->regs[rem.rs2] == 0) {
                processor->regs[rem.rd]             = processor->regs[rem.rs1];
                processor->regs[int(XPXPUEReg::R0)] = 0;
                processor->pc += pcIncr;
                return XPXPUProgramExitCode::Success;
            } else {
                const int INT32_MIN = -2147483648;
                processor->regs[rem.rd] =
                  (uint32_t)(((int32_t)processor->regs[rem.rs1] == INT32_MIN && (int32_t)processor->regs[rem.rs2] == -1)
                               ? 0
                               : ((uint32_t)((int32_t)processor->regs[rem.rs1] % (int32_t)processor->regs[rem.rs2])));
                processor->regs[int(XPXPUEReg::R0)] = 0;
                processor->pc += pcIncr;
                return XPXPUProgramExitCode::Success;
            }
        }
        case XPXPUEInstructionType::REMU: {
            REMU remu = (REMU)instr.value;
            if (processor->regs[remu.rs2] == 0) {
                processor->regs[remu.rd]            = processor->regs[remu.rs1];
                processor->regs[int(XPXPUEReg::R0)] = 0;
                processor->pc += pcIncr;
                return XPXPUProgramExitCode::Success;
            } else {
                processor->regs[remu.rd]            = (uint32_t)(processor->regs[remu.rs1] % processor->regs[remu.rs2]);
                processor->regs[int(XPXPUEReg::R0)] = 0;
                processor->pc += pcIncr;
                return XPXPUProgramExitCode::Success;
            }
        }
        case XPXPUEInstructionType::FENCE: {
            FENCE fence = (FENCE)instr.value;
            return XPXPUProgramExitCode::FenceUnimplemented;
        }
        case XPXPUEInstructionType::ECALL: {
            ECALL    ecall       = (ECALL)instr.value;
            uint32_t syscall_num = processor->regs[int(XPXPUEReg::R17)];
            int32_t  arg0        = processor->regs[int(XPXPUEReg::R10)];
            int32_t  arg1        = processor->regs[int(XPXPUEReg::R11)];
            int32_t  arg2        = processor->regs[int(XPXPUEReg::R12)];
            int32_t  arg3        = processor->regs[int(XPXPUEReg::R13)];
            switch (syscall_num) {
                case XP_XPU_SYSCALL_OPEN: {
                    processor->regs[int(XPXPUEReg::R0)] = 0;
                    processor->pc += pcIncr;
                    return XPXPUProgramExitCode::Success;
                }
                case XP_XPU_SYSCALL_CLOCKGETTIME: {
                    return XPXPUProgramExitCode::UnhandledSyscall;
                }
                case XP_XPU_SYSCALL_BRK: {
                    return XPXPUProgramExitCode::UnhandledSyscall;
                }
                case XP_XPU_SYSCALL_GETTIMEOFDAY: {
                    return XPXPUProgramExitCode::UnhandledSyscall;
                }
                case XP_XPU_SYSCALL_EXIT: {
                    processor->regs[int(XPXPUEReg::R0)] = 0;
                    processor->pc += pcIncr;
                    return XPXPUProgramExitCode::Success;
                }
                case XP_XPU_SYSCALL_FSTAT: {
                    processor->regs[int(XPXPUEReg::R0)] = 0;
                    processor->pc += pcIncr;
                    return XPXPUProgramExitCode::Success;
                }
                case XP_XPU_SYSCALL_WRITE: {
                    processor->regs[int(XPXPUEReg::R0)] = 0;
                    processor->pc += pcIncr;
                    return XPXPUProgramExitCode::Success;
                }
                case XP_XPU_SYSCALL_READ: {
                    processor->regs[int(XPXPUEReg::R0)] = 0;
                    processor->pc += pcIncr;
                    return XPXPUProgramExitCode::Success;
                }
                case XP_XPU_SYSCALL_LSEEK: {
                    processor->regs[int(XPXPUEReg::R0)] = 0;
                    processor->pc += pcIncr;
                    return XPXPUProgramExitCode::Success;
                }
                case XP_XPU_SYSCALL_CLOSE: {
                    processor->regs[int(XPXPUEReg::R0)] = 0;
                    processor->pc += pcIncr;
                    return XPXPUProgramExitCode::Success;
                }
                default: {
                    return XPXPUProgramExitCode::UnhandledSyscall;
                }
            }
        }
        case XPXPUEInstructionType::EBREAK: {
            EBREAK ebreak = (EBREAK)instr.value;
            return XPXPUProgramExitCode::Ebreak;
        }

        default: {
            return XPXPUProgramExitCode::Unknown;
        }
    }
}

uint32_t
load(XPXPUProcessor* processor, uint32_t addr, uint32_t size)
{
    return xp_xpu_bus_load(&processor->bus, addr, size);
}

void
store(XPXPUProcessor* processor, uint32_t addr, uint32_t size, uint32_t value)
{
    xp_xpu_bus_store(&processor->bus, addr, size, value);
}

int
xp_xpu_processor_step(XPXPUProcessor* processor)
{
    uint32_t                encodedInstr = fetch(processor);
    XPXPUEncodedInstruction instruction  = decode(encodedInstr);
    int                     ret          = execute(processor, instruction);
    if (ret != XPXPUProgramExitCode::Success) {
        return ret;
    } else {
        return 0;
    }
}

void
xp_xpu_processor_run(XPXPUProcessor* processor)
{
    int result = 0;
    while (result == 0) { result = xp_xpu_processor_step(processor); }
}
