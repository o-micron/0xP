#include "instruction.slang"

uint32_t
encode_TYPE_R(TYPE_R r)
{
    return (r.opcode & 0x7F) | ((r.rd & 0x1F) << 7) | ((r.funct3 & 0x7) << 12) | ((r.rs1 & 0x1F) << 15) |
           ((r.rs2 & 0x1F) << 20) | ((r.funct7 & 0x7F) << 25);
}

TYPE_R
decode_TYPE_R(uint32_t val)
{
    TYPE_R t;
    t.opcode = val & 0x7F;
    t.rd     = (val >> 7) & 0x1F;
    t.funct3 = (val >> 12) & 0x7;
    t.rs1    = (val >> 15) & 0x1F;
    t.rs2    = (val >> 20) & 0x1F;
    t.funct7 = (val >> 25) & 0x7F;
    return t;
}

uint32_t
encode_TYPE_I(TYPE_I i)
{
    return (i.opcode & 0x7F) | ((i.rd & 0x1F) << 7) | ((i.funct3 & 0x7) << 12) | ((i.rs1 & 0x1F) << 15) |
           ((i.imm & 0xFFF) << 20);
}

TYPE_I
decode_TYPE_I(uint32_t val)
{
    TYPE_I t;
    t.opcode = val & 0x7F;
    t.rd     = (val >> 7) & 0x1F;
    t.funct3 = (val >> 12) & 0x7;
    t.rs1    = (val >> 15) & 0x1F;
    t.imm    = (val >> 20) & 0xFFF;
    return t;
}

uint32_t
encode_TYPE_S(TYPE_S s)
{
    uint32_t imm = ((s.imm_funct7 & 0x7F) << 5) | (s.imm_rd & 0x1F);
    return (s.opcode & 0x7F) | ((imm & 0x1F) << 7) | ((s.funct3 & 0x7) << 12) | ((s.rs1 & 0x1F) << 15) |
           ((s.rs2 & 0x1F) << 20) | ((imm >> 5) << 25);
}

TYPE_S
decode_TYPE_S(uint32_t val)
{
    TYPE_S   t;
    uint32_t imm_low  = (val >> 7) & 0x1F;
    uint32_t imm_high = (val >> 25) & 0x7F;
    t.opcode          = val & 0x7F;
    t.imm_rd          = imm_low;
    t.funct3          = (val >> 12) & 0x7;
    t.rs1             = (val >> 15) & 0x1F;
    t.rs2             = (val >> 20) & 0x1F;
    t.imm_funct7      = imm_high;
    return t;
}

uint32_t
encode_TYPE_B(TYPE_B b)
{
    uint32_t imm = ((b.imm_funct7 & 0x7F) << 5) | (b.imm_rd & 0x1F);
    return (b.opcode & 0x7F) | ((imm & 0x1F) << 7) | ((b.funct3 & 0x7) << 12) | ((b.rs1 & 0x1F) << 15) |
           ((b.rs2 & 0x1F) << 20) | ((imm >> 5) << 25);
}

TYPE_B
decode_TYPE_B(uint32_t val)
{
    TYPE_B   t;
    uint32_t imm_low  = (val >> 7) & 0x1F;
    uint32_t imm_high = (val >> 25) & 0x7F;
    t.opcode          = val & 0x7F;
    t.imm_rd          = imm_low;
    t.funct3          = (val >> 12) & 0x7;
    t.rs1             = (val >> 15) & 0x1F;
    t.rs2             = (val >> 20) & 0x1F;
    t.imm_funct7      = imm_high;
    return t;
}

uint32_t
encode_TYPE_U(TYPE_U u)
{
    return (u.opcode & 0x7F) | ((u.rd & 0x1F) << 7) | ((u.imm & 0xFFFFF) << 12);
}

TYPE_U
decode_TYPE_U(uint32_t val)
{
    TYPE_U t;
    t.opcode = val & 0x7F;
    t.rd     = (val >> 7) & 0x1F;
    t.imm    = (val >> 12) & 0xFFFFF;
    return t;
}

uint32_t
encode_TYPE_J(TYPE_J j)
{
    return (j.opcode & 0x7F) | ((j.rd & 0x1F) << 7) | ((j.imm & 0xFFFFF) << 12);
}

TYPE_J
decode_TYPE_J(uint32_t val)
{
    TYPE_J t;
    t.opcode = val & 0x7F;
    t.rd     = (val >> 7) & 0x1F;
    t.imm    = (val >> 12) & 0xFFFFF;
    return t;
}

uint32_t
decode_imm_S(uint32_t instr)
{
    return ((int32_t)(int32_t)(instr & 0xfe000000) >> 20) | ((instr >> 7) & 0x1f);
}

uint32_t
decode_imm_B(uint32_t instr)
{
    return ((int32_t)(int32_t)(instr & 0x80000000) >> 19) | ((instr & 0x80) << 4) | ((instr >> 20) & 0x7e0) |
           ((instr >> 7) & 0x1e);
}

uint32_t
decode_imm_U(uint32_t instr)
{
    return (int32_t)(int32_t)(instr & 0xfffff000);
}

int32_t
decode_imm_J(uint32_t instr)
{
    int32_t imm = 0;

    imm |= (instr & 0x80000000) >> 11;
    imm |= (instr & 0x7FE00000) >> 20;
    imm |= (instr & 0x00100000) >> 9;
    imm |= (instr & 0x000FF000);

    if ((imm & 0x00100000) != 0) {
        imm |= 0xFFE00000;
        return imm;
    } else {
        return imm;
    }
}

XPXPUEInstructionType
xp_xpu_decoder_decode_instruction_type(uint32_t encodedInstruction)
{
    uint32_t opcode   = encodedInstruction & 0b1111111;
    uint32_t funct3   = (encodedInstruction >> 12) & 0b111;
    uint32_t imm_11_0 = (encodedInstruction >> 20) & 0b111111111111;
    uint32_t funct7   = (encodedInstruction >> 25) & 0b1111111;

    if (opcode == 0b0110111) {
        return XPXPUEInstructionType::LUI;
    } else if (opcode == 0b0010111) {
        return XPXPUEInstructionType::AUIPC;
    } else if (opcode == 0b1101111) {
        return XPXPUEInstructionType::JAL;
    } else if (opcode == 0b1100111) {
        return XPXPUEInstructionType::JALR;
    } else if (opcode == 0b1100011) {
        if (funct3 == 0) {
            return XPXPUEInstructionType::BEQ;
        } else if (funct3 == 1) {
            return XPXPUEInstructionType::BNE;
        } else if (funct3 == 4) {
            return XPXPUEInstructionType::BLT;
        } else if (funct3 == 5) {
            return XPXPUEInstructionType::BGE;
        } else if (funct3 == 6) {
            return XPXPUEInstructionType::BLTU;
        } else if (funct3 == 7) {
            return XPXPUEInstructionType::BGEU;
        } else {
            return XPXPUEInstructionType::Undefined;
        }
    } else if (opcode == 0b0000011) {
        if (funct3 == 0) {
            return XPXPUEInstructionType::LB;
        } else if (funct3 == 1) {
            return XPXPUEInstructionType::LH;
        } else if (funct3 == 2) {
            return XPXPUEInstructionType::LW;
        } else if (funct3 == 4) {
            return XPXPUEInstructionType::LBU;
        } else if (funct3 == 5) {
            return XPXPUEInstructionType::LHU;
        } else {
            return XPXPUEInstructionType::Undefined;
        }
    } else if (opcode == 0b0100011) {
        if (funct3 == 0) {
            return XPXPUEInstructionType::SB;
        } else if (funct3 == 1) {
            return XPXPUEInstructionType::SH;
        } else if (funct3 == 2) {
            return XPXPUEInstructionType::SW;
        } else {
            return XPXPUEInstructionType::Undefined;
        }
    } else if (opcode == 0b0010011) {
        if (funct3 == 0b001) {
            return XPXPUEInstructionType::SLLI;
        } else if (funct3 == 0b000) {
            return XPXPUEInstructionType::ADDI;
        } else if (funct3 == 0b010) {
            return XPXPUEInstructionType::SLTI;
        } else if (funct3 == 0b011) {
            return XPXPUEInstructionType::SLTIU;
        } else if (funct3 == 0b100) {
            return XPXPUEInstructionType::XORI;
        } else if (funct3 == 0b110) {
            return XPXPUEInstructionType::ORI;
        } else if (funct3 == 0b111) {
            return XPXPUEInstructionType::ANDI;
        } else if (funct3 == 0b101) {
            if (funct7 == 0) {
                return XPXPUEInstructionType::SRLI;
            } else if (funct7 == 0b0100000) {
                return XPXPUEInstructionType::SRAI;
            } else {
                return XPXPUEInstructionType::Undefined;
            }
        } else {
            return XPXPUEInstructionType::Undefined;
        }
    } else if (opcode == 0b0110011) {
        if (funct7 == 0) {
            if (funct3 == 0) {
                return XPXPUEInstructionType::ADD;
            } else if (funct3 == 0b001) {
                return XPXPUEInstructionType::SLL;
            } else if (funct3 == 0b010) {
                return XPXPUEInstructionType::SLT;
            } else if (funct3 == 0b011) {
                return XPXPUEInstructionType::SLTU;
            } else if (funct3 == 0b100) {
                return XPXPUEInstructionType::XOR;
            } else if (funct3 == 0b101) {
                return XPXPUEInstructionType::SRL;
            } else if (funct3 == 0b110) {
                return XPXPUEInstructionType::OR;
            } else if (funct3 == 0b111) {
                return XPXPUEInstructionType::AND;
            } else {
                return XPXPUEInstructionType::Undefined;
            }
        } else if (funct7 == 0b0100000) {
            if (funct3 == 0b000) {
                return XPXPUEInstructionType::SUB;
            } else if (funct3 == 0b101) {
                return XPXPUEInstructionType::SRA;
            } else {
                return XPXPUEInstructionType::Undefined;
            }
        } else if (funct7 == 0b0000001) {
            if (funct3 == 0b000) {
                return XPXPUEInstructionType::MUL;
            } else if (funct3 == 0b001) {
                return XPXPUEInstructionType::MULH;
            } else if (funct3 == 0b010) {
                return XPXPUEInstructionType::MULHSU;
            } else if (funct3 == 0b011) {
                return XPXPUEInstructionType::MULHU;
            } else if (funct3 == 0b100) {
                return XPXPUEInstructionType::DIV;
            } else if (funct3 == 0b101) {
                return XPXPUEInstructionType::DIVU;
            } else if (funct3 == 0b110) {
                return XPXPUEInstructionType::REM;
            } else if (funct3 == 0b111) {
                return XPXPUEInstructionType::REMU;
            } else {
                return XPXPUEInstructionType::Undefined;
            }
        } else {
            return XPXPUEInstructionType::Undefined;
        }
    } else if (opcode == 0b0001111) {
        return XPXPUEInstructionType::FENCE;
    } else if (opcode == 0b1110011) {
        if (imm_11_0 == 0b000000000000) {
            return XPXPUEInstructionType::ECALL;
        } else if (imm_11_0 == 0b000000000001) {
            return XPXPUEInstructionType::EBREAK;
        } else {
            return XPXPUEInstructionType::Undefined;
        }
    } else {
        return XPXPUEInstructionType::Undefined;
    }
}

XPXPUEncodedInstruction
xp_xpu_decoder_decode_instruction(uint32_t encodedInstruction)
{
    XPXPUEncodedInstruction instr = {};
    instr.type                    = xp_xpu_decoder_decode_instruction_type(encodedInstruction);
    instr.value                   = encodedInstruction;
    return instr;
}

