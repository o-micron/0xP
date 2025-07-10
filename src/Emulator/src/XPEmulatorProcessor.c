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

#include "../../Compute/include/Compute/XPImageWorks.h"

#include <Emulator/XPEmulatorCommon.h>
#include <Emulator/XPEmulatorDecoder.h>
#include <Emulator/XPEmulatorElfLoader.h>
#include <Emulator/XPEmulatorInstruction.h>
#include <Emulator/XPEmulatorLogger.h>
#include <Emulator/XPEmulatorProcessor.h>
#include <Emulator/XPEmulatorSyscalls.h>
#include <Emulator/XPEmulatorUART.h>

#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#if defined(__APPLE__)
    #include <regex.h>
    #include <unistd.h>
    #define XP_EMULATOR_FD_FROM_FILE(FILE) FILE->_file
#elif defined(__linux__)
    #include <regex.h>
    #include <unistd.h>
    #define XP_EMULATOR_FD_FROM_FILE(FILE) FILE->_fileno
#elif defined(_WIN32)
    #define STDOUT_FILENO                  _fileno(stdout)
    #define STDERR_FILENO                  _fileno(stderr)
    #define STDIN_FILENO                   _fileno(stdin)
    #define XP_EMULATOR_FD_FROM_FILE(FILE) _fileno(FILE)
#endif

uint32_t
fetch(XPEmulatorProcessor* processor);

struct XPEmulatorEncodedInstruction
decode(uint32_t instruction);

int
execute(XPEmulatorProcessor* processor, struct XPEmulatorEncodedInstruction instr);

uint32_t
load(XPEmulatorProcessor* processor, uint32_t addr, uint32_t size);

void
store(XPEmulatorProcessor* processor, uint32_t addr, uint32_t size, uint32_t value);

void
print_registers(XPEmulatorProcessor* processor);

XP_EMULATOR_EXTERN void
xp_emulator_processor_initialize(XPEmulatorProcessor* processor)
{
    xp_emulator_bus_initialize(&processor->bus);

    processor->regs[XPEmulatorEReg0] = 0;
    processor->regs[XPEmulatorEReg2] = XP_EMULATOR_CONFIG_HMM_TOP_STACK_PTR;
    processor->pc                    = 0;
}

XP_EMULATOR_EXTERN int
xp_emulator_processor_load_program(XPEmulatorProcessor* processor, RiscvElfLoader* loader)
{
    assert(loader->segments[0].vaddr == XP_EMULATOR_CONFIG_MEMORY_FLASH_BASE);
    assert(loader->segments[1].vaddr == XP_EMULATOR_CONFIG_MEMORY_RAM_BASE);
    assert(loader->segments[2].vaddr == XP_EMULATOR_CONFIG_MEMORY_HEAP_BASE);

    memcpy(processor->bus.memory.flash, loader->segments[0].data, loader->segments[0].size);
    memcpy(processor->bus.memory.ram, loader->segments[1].data, loader->segments[1].size);
    memcpy(processor->bus.memory.heap, loader->segments[2].data, loader->segments[2].size);

    processor->pc = loader->entry_point;

    return 0;
}

XP_EMULATOR_EXTERN int
xp_emulator_processor_step(XPEmulatorProcessor* processor)
{
    // fetch
    uint32_t encodedInstr = fetch(processor);

    // decode
    struct XPEmulatorEncodedInstruction instruction = decode(encodedInstr);

    // execute
    int ret = execute(processor, instruction);
    if (ret != XP_EMULATOR_PROGRAM_EXIT_CODE_SUCCESS) {
        XP_EMULATOR_LOGV("UNEXPECTEDLY EXITED {%i}", ret);
        return ret;
    }

    return 0;
}

XP_EMULATOR_EXTERN void
xp_emulator_processor_run(XPEmulatorProcessor* processor)
{
    print_registers(processor);

    while (1) {
        print_registers(processor);
        int result = xp_emulator_processor_step(processor);
        if (result != 0) {
            //
            break;
        }
    }
    printf("CPU EMULATOR\n");
    printf("stackBottomAddress is %08" PRIx32 "\n", processor->bus.hostMappedMemory.deviceBottomStack);
    printf("stackTopAddress is %08" PRIx32 "\n", processor->bus.hostMappedMemory.deviceTopStack);
    printf("heapStartAddress is %08" PRIx32 "\n", processor->bus.hostMappedMemory.deviceStartHeap);
    printf("heapEndAddress is %08" PRIx32 "\n", processor->bus.hostMappedMemory.deviceEndHeap);
    printf("FramebufferAddress is %08" PRIx32 "\n", processor->bus.hostMappedMemory.deviceFramebuffer);
    printf("FrameMemoryPoolAddress is %08" PRIx32 "\n", processor->bus.hostMappedMemory.deviceFrameMemoryPool);
    printf("program.pc = %u\n", processor->pc);
    {
        uint32_t fbAddr = processor->bus.hostMappedMemory.deviceFramebuffer;
        if (fbAddr != 0) {
#define WIDTH  120
#define HEIGHT 120

            float    f32texture[WIDTH * HEIGHT * 3];
            uint8_t  u8texture[WIDTH * HEIGHT * 3];
            uint32_t mem_address = fbAddr - XP_EMULATOR_CONFIG_MEMORY_HEAP_BASE;
            uint8_t* etex        = (uint8_t*)&(processor->bus.memory.heap[mem_address]);
            for (int y = 0; y < HEIGHT; y++) {
                for (int x = 0; x < WIDTH; x++) {
                    int pixel_index = (y * WIDTH + x) * 3;

                    uint8_t rVal0 = etex[(pixel_index + 0) * 4];
                    uint8_t rVal1 = etex[(pixel_index + 0) * 4 + 1];
                    uint8_t rVal2 = etex[(pixel_index + 0) * 4 + 2];
                    uint8_t rVal3 = etex[(pixel_index + 0) * 4 + 3];
                    float   rVal  = (float)(uint32_t)((uint32_t)rVal3 | (uint32_t)rVal2 << 8 | (uint32_t)rVal1 << 16 |
                                                   (uint32_t)rVal0 << 24);

                    uint8_t gVal0 = etex[(pixel_index + 1) * 4];
                    uint8_t gVal1 = etex[(pixel_index + 1) * 4 + 1];
                    uint8_t gVal2 = etex[(pixel_index + 1) * 4 + 2];
                    uint8_t gVal3 = etex[(pixel_index + 1) * 4 + 3];
                    float   gVal  = (float)(uint32_t)((uint32_t)gVal3 | (uint32_t)gVal2 << 8 | (uint32_t)gVal1 << 16 |
                                                   (uint32_t)gVal0 << 24);

                    uint8_t bVal0 = etex[(pixel_index + 2) * 4];
                    uint8_t bVal1 = etex[(pixel_index + 2) * 4 + 1];
                    uint8_t bVal2 = etex[(pixel_index + 2) * 4 + 2];
                    uint8_t bVal3 = etex[(pixel_index + 2) * 4 + 3];
                    float   bVal  = (float)(uint32_t)((uint32_t)bVal3 | (uint32_t)bVal2 << 8 | (uint32_t)bVal1 << 16 |
                                                   (uint32_t)bVal0 << 24);

                    u8texture[pixel_index + 0] = (uint8_t)(rVal * 255.0f);
                    u8texture[pixel_index + 1] = (uint8_t)(gVal * 255.0f);
                    u8texture[pixel_index + 2] = (uint8_t)(bVal * 255.0f);

                    f32texture[pixel_index + 0] = rVal;
                    f32texture[pixel_index + 1] = gVal;
                    f32texture[pixel_index + 2] = bVal;
                }
            }
            save_as_ppm("rt0.ppm", u8texture, WIDTH, HEIGHT);
        }
    }
    print_registers(processor);
}

XP_EMULATOR_EXTERN void
xp_emulator_processor_finalize(XPEmulatorProcessor* processor)
{
    xp_emulator_bus_finalize(&processor->bus);
}

uint32_t
fetch(XPEmulatorProcessor* processor)
{
    return xp_emulator_bus_load(&processor->bus, processor->pc, 32);
}

struct XPEmulatorEncodedInstruction
decode(uint32_t instruction)
{
    return xp_emulator_decoder_decode_instruction(instruction);
}

int
execute(XPEmulatorProcessor* processor, struct XPEmulatorEncodedInstruction instr)
{
    int pcIncr = 4;

    switch (instr.type) {
        case XPEmulatorEInstructionType_Undefined: {
            return XP_EMULATOR_PROGRAM_EXIT_CODE_UNDEFINED_INSTRUCTION;
        }
        case XPEmulatorEInstructionType_LUI: {
            xp_emulator_print_op("LUI");
            // LUI places upper 20 bits of U-immediate value to rd and zeros in the lowest 12 bits
            uint32_t dest                 = (instr.instruction.value & 0b11111111111111111111000000000000);
            processor->regs[instr.LUI.rd] = dest;
            break;
        }
        case XPEmulatorEInstructionType_AUIPC: {
            xp_emulator_print_op("AUIPC");
            // AUIPC forms a 32-bit offset from the 20 upper bits of the U-immediate
            uint32_t dest = processor->pc + (instr.instruction.value & 0b11111111111111111111000000000000);
            processor->regs[instr.AUIPC.rd] = dest;
            break;
        }
        case XPEmulatorEInstructionType_JAL: {
            xp_emulator_print_op("JAL");
            int32_t imm = 0;
            imm |= (instr.instruction.value & 0x80000000) >> 11; // imm[20] -> Bit 31 (sign bit)
            imm |= (instr.instruction.value & 0x7FE00000) >> 20; // imm[10:1] -> Bits 21-30
            imm |= (instr.instruction.value & 0x00100000) >> 9;  // imm[11] -> Bit 20
            imm |= (instr.instruction.value & 0x000FF000);       // imm[19:12] -> Bits 12-19
            // Sign extend imm (ensure 32-bit signed value)
            if (imm & 0x00100000) { // If imm[20] (bit 20) is set
                imm |= 0xFFE00000;  // Extend the sign to higher bits
            }
            processor->regs[instr.JAL.rd] = processor->pc + 4;
            pcIncr                        = imm;
            break;
        }
        case XPEmulatorEInstructionType_JALR: {
            xp_emulator_print_op("JALR");
            // Extract bits 20-31 (12-bit immediate)
            int32_t imm = (instr.instruction.value >> 20) & 0xFFF;
            // Sign-extend to 32-bit if imm[11] (bit 11) is set
            if (imm & 0x800) {
                imm |= 0xFFFFF000; // Extend sign to higher bits
            }
            uint32_t nextInstrAddress      = processor->pc + 4;
            processor->pc                  = (processor->regs[instr.JALR.rs1] + imm) & 0xFFFFFFFE;
            processor->regs[instr.JALR.rd] = nextInstrAddress;
            pcIncr                         = 0;
            break;
        }
        case XPEmulatorEInstructionType_BEQ: {
            xp_emulator_print_op("BEQ");
            int32_t imm = 0;
            imm |= (instr.instruction.value & 0x80000000) >> 19; // imm[12] -> Bit 31 (sign bit)
            imm |= (instr.instruction.value & 0x7E000000) >> 20; // imm[10:5] -> Bits 25-30
            imm |= (instr.instruction.value & 0x00000F00) >> 7;  // imm[4:1] -> Bits 8-11
            imm |= (instr.instruction.value & 0x00000080) << 4;  // imm[11] -> Bit 7
            // Sign-extend imm to 32-bit if imm[12] (bit 12) is set
            if (imm & 0x1000) { imm |= 0xFFFFE000; }
            if ((int32_t)processor->regs[instr.BEQ.rs1] == (int32_t)processor->regs[instr.BEQ.rs2]) { pcIncr = imm; }
            break;
        }
        case XPEmulatorEInstructionType_BNE: {
            xp_emulator_print_op("BNE");
            int32_t imm = 0;
            imm |= (instr.instruction.value & 0x80000000) >> 19; // imm[12] -> Bit 31 (sign bit)
            imm |= (instr.instruction.value & 0x7E000000) >> 20; // imm[10:5] -> Bits 25-30
            imm |= (instr.instruction.value & 0x00000F00) >> 7;  // imm[4:1] -> Bits 8-11
            imm |= (instr.instruction.value & 0x00000080) << 4;  // imm[11] -> Bit 7
            // Sign-extend imm to 32-bit if imm[12] (bit 12) is set
            if (imm & 0x1000) { imm |= 0xFFFFE000; }
            if ((int32_t)processor->regs[instr.BNE.rs1] != (int32_t)processor->regs[instr.BNE.rs2]) { pcIncr = imm; }
            break;
        }
        case XPEmulatorEInstructionType_BLT: {
            xp_emulator_print_op("BLT");
            int32_t imm = 0;
            imm |= (instr.instruction.value & 0x80000000) >> 19; // imm[12] -> Bit 31 (sign bit)
            imm |= (instr.instruction.value & 0x7E000000) >> 20; // imm[10:5] -> Bits 25-30
            imm |= (instr.instruction.value & 0x00000F00) >> 7;  // imm[4:1] -> Bits 8-11
            imm |= (instr.instruction.value & 0x00000080) << 4;  // imm[11] -> Bit 7
            // Sign-extend imm to 32-bit if imm[12] (bit 12) is set
            if (imm & 0x1000) { imm |= 0xFFFFE000; }
            if ((int32_t)processor->regs[instr.BLT.rs1] < (int32_t)processor->regs[instr.BLT.rs2]) { pcIncr = imm; }
            break;
        }
        case XPEmulatorEInstructionType_BGE: {
            xp_emulator_print_op("BGE");
            int32_t imm = 0;
            imm |= (instr.instruction.value & 0x80000000) >> 19; // imm[12] -> Bit 31 (sign bit)
            imm |= (instr.instruction.value & 0x7E000000) >> 20; // imm[10:5] -> Bits 25-30
            imm |= (instr.instruction.value & 0x00000F00) >> 7;  // imm[4:1] -> Bits 8-11
            imm |= (instr.instruction.value & 0x00000080) << 4;  // imm[11] -> Bit 7
            // Sign-extend imm to 32-bit if imm[12] (bit 12) is set
            if (imm & 0x1000) { imm |= 0xFFFFE000; }
            if ((int32_t)processor->regs[instr.BGE.rs1] >= (int32_t)processor->regs[instr.BGE.rs2]) { pcIncr = imm; }
            break;
        }
        case XPEmulatorEInstructionType_BLTU: {
            xp_emulator_print_op("BLTU");
            int32_t imm = 0;
            imm |= (instr.instruction.value & 0x80000000) >> 19; // imm[12] -> Bit 31 (sign bit)
            imm |= (instr.instruction.value & 0x7E000000) >> 20; // imm[10:5] -> Bits 25-30
            imm |= (instr.instruction.value & 0x00000F00) >> 7;  // imm[4:1] -> Bits 8-11
            imm |= (instr.instruction.value & 0x00000080) << 4;  // imm[11] -> Bit 7
            // Sign-extend imm to 32-bit if imm[12] (bit 12) is set
            if (imm & 0x1000) { imm |= 0xFFFFE000; }
            if (processor->regs[instr.BLTU.rs1] < processor->regs[instr.BLTU.rs2]) { pcIncr = imm; }
            break;
        }
        case XPEmulatorEInstructionType_BGEU: {
            xp_emulator_print_op("BGEU");
            int32_t imm = 0;
            imm |= (instr.instruction.value & 0x80000000) >> 19; // imm[12] -> Bit 31 (sign bit)
            imm |= (instr.instruction.value & 0x7E000000) >> 20; // imm[10:5] -> Bits 25-30
            imm |= (instr.instruction.value & 0x00000F00) >> 7;  // imm[4:1] -> Bits 8-11
            imm |= (instr.instruction.value & 0x00000080) << 4;  // imm[11] -> Bit 7
            // Sign-extend imm to 32-bit if imm[12] (bit 12) is set
            if (imm & 0x1000) { imm |= 0xFFFFE000; }
            if (processor->regs[instr.BGEU.rs1] >= processor->regs[instr.BGEU.rs2]) { pcIncr = imm; }
            break;
        }
        case XPEmulatorEInstructionType_LB: {
            xp_emulator_print_op("LB");
            // imm[11:0] = inst[31:20]
            uint32_t imm                 = ((int32_t)(int32_t)(instr.instruction.value & 0xFFF00000)) >> 20;
            uint32_t addr                = processor->regs[instr.LB.rs1] + (int32_t)imm;
            processor->regs[instr.LB.rd] = (int32_t)(int8_t)load(processor, addr, 8);
            break;
        }
        case XPEmulatorEInstructionType_LH: {
            xp_emulator_print_op("LH");
            // imm[11:0] = inst[31:20]
            uint32_t imm = (instr.instruction.value >> 20) & 0xFFF;
            // Sign extend the 12-bit immediate to 32 bits
            if (imm & 0x800) {     // Check if sign bit is set
                imm |= 0xFFFFF000; // Sign extend
            }

            uint32_t addr                = processor->regs[instr.LH.rs1] + (int32_t)imm;
            processor->regs[instr.LH.rd] = (int32_t)(int16_t)load(processor, addr, 16);
            break;
        }
        case XPEmulatorEInstructionType_LW: {
            xp_emulator_print_op("LW");
            // imm[11:0] = inst[31:20]
            uint32_t imm                 = ((int32_t)(int32_t)(instr.instruction.value & 0xFFF00000)) >> 20;
            uint32_t addr                = processor->regs[instr.LW.rs1] + (int32_t)imm;
            processor->regs[instr.LW.rd] = (int32_t)(int32_t)load(processor, addr, 32);
            break;
        }
        case XPEmulatorEInstructionType_LBU: {
            xp_emulator_print_op("LBU");
            // imm[11:0] = inst[31:20]
            uint32_t imm                  = ((int32_t)(int32_t)(instr.instruction.value & 0xFFF00000)) >> 20;
            uint32_t addr                 = processor->regs[instr.LBU.rs1] + (int32_t)imm;
            processor->regs[instr.LBU.rd] = load(processor, addr, 8);
            break;
        }
        case XPEmulatorEInstructionType_LHU: {
            xp_emulator_print_op("LHU");
            // imm[11:0] = inst[31:20]
            uint32_t imm                  = ((int32_t)(int32_t)(instr.instruction.value & 0xFFF00000)) >> 20;
            uint32_t addr                 = processor->regs[instr.LHU.rs1] + (int32_t)imm;
            processor->regs[instr.LHU.rd] = load(processor, addr, 16);
            break;
        }
        case XPEmulatorEInstructionType_SB: {
            xp_emulator_print_op("SB");
            int64_t  imm     = imm_S(instr);
            uint32_t address = processor->regs[instr.SB.rs1] + (int32_t)imm;
            uint32_t value   = (processor->regs[instr.SB.rs2] & 0x000000FF);
            store(processor, address, 8, value);
            break;
        }
        case XPEmulatorEInstructionType_SH: {
            xp_emulator_print_op("SH");
            int64_t  imm     = imm_S(instr);
            uint32_t address = processor->regs[instr.SH.rs1] + (int32_t)imm;
            uint32_t value   = (processor->regs[instr.SH.rs2] & 0x0000FFFF);
            store(processor, address, 16, value);
            break;
        }
        case XPEmulatorEInstructionType_SW: {
            xp_emulator_print_op("SW");
            int64_t  imm     = imm_S(instr);
            uint32_t address = processor->regs[instr.SW.rs1] + (int32_t)imm;
            uint32_t value   = processor->regs[instr.SW.rs2];
            store(processor, address, 32, value);
            break;
        }
        case XPEmulatorEInstructionType_ADDI: {
            xp_emulator_print_op("ADDI");
            // imm[11:0] = inst[31:20]
            uint32_t imm                   = ((int32_t)(int32_t)(instr.instruction.value & 0xFFF00000)) >> 20;
            processor->regs[instr.ADDI.rd] = processor->regs[instr.ADDI.rs1] + (int32_t)imm;
            break;
        }
        case XPEmulatorEInstructionType_SLTI: {
            xp_emulator_print_op("SLTI");
            // imm[11:0] = inst[31:20]
            uint32_t imm                   = ((int32_t)(int32_t)(instr.instruction.value & 0xFFF00000)) >> 20;
            processor->regs[instr.SLTI.rd] = (processor->regs[instr.SLTI.rs1] < (int32_t)imm) ? 1 : 0;
            break;
        }
        case XPEmulatorEInstructionType_SLTIU: {
            xp_emulator_print_op("SLTIU");
            // imm[11:0] = inst[31:20]
            uint32_t imm                    = ((int32_t)(int32_t)(instr.instruction.value & 0xFFF00000)) >> 20;
            processor->regs[instr.SLTIU.rd] = (processor->regs[instr.SLTIU.rs1] < imm) ? 1 : 0;
            break;
        }
        case XPEmulatorEInstructionType_XORI: {
            xp_emulator_print_op("XORI");
            // imm[11:0] = inst[31:20]
            uint32_t imm                   = ((int32_t)(int32_t)(instr.instruction.value & 0xFFF00000)) >> 20;
            processor->regs[instr.XORI.rd] = processor->regs[instr.XORI.rs1] ^ imm;
            break;
        }
        case XPEmulatorEInstructionType_ORI: {
            xp_emulator_print_op("ORI");
            // imm[11:0] = inst[31:20]
            uint32_t imm                  = ((int32_t)(int32_t)(instr.instruction.value & 0xFFF00000)) >> 20;
            processor->regs[instr.ORI.rd] = processor->regs[instr.ORI.rs1] | imm;
            break;
        }
        case XPEmulatorEInstructionType_ANDI: {
            xp_emulator_print_op("ANDI");
            // imm[11:0] = inst[31:20]
            uint32_t imm                   = ((int32_t)(int32_t)(instr.instruction.value & 0xFFF00000)) >> 20;
            processor->regs[instr.ANDI.rd] = processor->regs[instr.ANDI.rs1] & imm;
            break;
        }
        case XPEmulatorEInstructionType_SLLI: {
            xp_emulator_print_op("SLLI");
            // imm[11:0] = inst[31:20]
            uint32_t imm = ((int32_t)(int32_t)(instr.instruction.value & 0xFFF00000)) >> 20;
            // shamt[4:5] = imm[5:0]
            uint32_t shamt                 = (uint32_t)(imm & 0x1F);
            processor->regs[instr.SLLI.rd] = processor->regs[instr.SLLI.rs1] << shamt;
            break;
        }
        case XPEmulatorEInstructionType_SRLI: {
            xp_emulator_print_op("SRLI");
            // imm[11:0] = inst[31:20]
            uint32_t imm = ((int32_t)(int32_t)(instr.instruction.value & 0xFFF00000)) >> 20;
            // shamt[4:5] = imm[5:0]
            uint32_t shamt                 = (uint32_t)(imm & 0x1F);
            processor->regs[instr.SRLI.rd] = processor->regs[instr.SRLI.rs1] >> shamt;
            break;
        }
        case XPEmulatorEInstructionType_SRAI: {
            xp_emulator_print_op("SRAI");
            // imm[11:0] = inst[31:20]
            uint32_t imm = ((int32_t)(int32_t)(instr.instruction.value & 0xFFF00000)) >> 20;
            // shamt[4:5] = imm[5:0]
            uint32_t shamt                 = (uint32_t)(imm & 0x1F);
            processor->regs[instr.SRAI.rd] = (uint32_t)((int32_t)processor->regs[instr.SRAI.rs1] >> shamt);
            break;
        }
        case XPEmulatorEInstructionType_ADD: {
            xp_emulator_print_op("ADD");
            processor->regs[instr.ADD.rd] =
              (uint32_t)((int32_t)processor->regs[instr.ADD.rs1] + (int32_t)processor->regs[instr.ADD.rs2]);
            break;
        }
        case XPEmulatorEInstructionType_SUB: {
            xp_emulator_print_op("SUB");
            processor->regs[instr.SUB.rd] =
              (uint32_t)((int32_t)processor->regs[instr.SUB.rs1] - (int32_t)processor->regs[instr.SUB.rs2]);
            break;
        }
        case XPEmulatorEInstructionType_SLL: {
            xp_emulator_print_op("SLL");
            processor->regs[instr.SLL.rd] = (int32_t)(processor->regs[instr.SLL.rs1] << processor->regs[instr.SLL.rs2]);
            break;
        }
        case XPEmulatorEInstructionType_SLT: {
            xp_emulator_print_op("SLT");
            processor->regs[instr.SLT.rd] =
              (processor->regs[instr.SLT.rs1] < (int32_t)processor->regs[instr.SLT.rs2]) ? 1 : 0;
            break;
        }
        case XPEmulatorEInstructionType_SLTU: {
            xp_emulator_print_op("SLTU");
            processor->regs[instr.SLTU.rd] =
              (processor->regs[instr.SLTU.rs1] < processor->regs[instr.SLTU.rs2]) ? 1 : 0;
            break;
        }
        case XPEmulatorEInstructionType_XOR: {
            xp_emulator_print_op("XOR");
            processor->regs[instr.XOR.rd] = processor->regs[instr.XOR.rs1] ^ processor->regs[instr.XOR.rs2];
            break;
        }
        case XPEmulatorEInstructionType_SRL: {
            xp_emulator_print_op("SRL");
            processor->regs[instr.SRL.rd] = processor->regs[instr.SRL.rs1] >> processor->regs[instr.SRL.rs2];
            break;
        }
        case XPEmulatorEInstructionType_SRA: {
            xp_emulator_print_op("SRA");
            processor->regs[instr.SRA.rd] =
              (uint32_t)((int32_t)processor->regs[instr.SRA.rs1] >> (int32_t)processor->regs[instr.SRA.rs2]);
            break;
        }
        case XPEmulatorEInstructionType_OR: {
            xp_emulator_print_op("OR");
            processor->regs[instr.OR.rd] = processor->regs[instr.OR.rs1] | processor->regs[instr.OR.rs2];
            break;
        }
        case XPEmulatorEInstructionType_AND: {
            xp_emulator_print_op("AND");
            processor->regs[instr.AND.rd] = processor->regs[instr.AND.rs1] & processor->regs[instr.AND.rs2];
            break;
        }
#if defined(XP_EMULATOR_USE_M_EXTENSION)
        case XPEmulatorEInstructionType_MUL: {
            xp_emulator_print_op("MUL");
            processor->regs[instr.MUL.rd] = (uint32_t)(processor->regs[instr.MUL.rs1] * processor->regs[instr.MUL.rs2]);
            break;
        }
        case XPEmulatorEInstructionType_MULH: {
            xp_emulator_print_op("MULH");
            processor->regs[instr.MULH.rd] = (uint32_t)(((int64_t)((int32_t)processor->regs[instr.MULH.rs1]) *
                                                         (int64_t)((int32_t)processor->regs[instr.MULH.rs2])) >>
                                                        32);
            break;
        }
        case XPEmulatorEInstructionType_MULHSU: {
            xp_emulator_print_op("MULHSU");
            processor->regs[instr.MULHSU.rd] = (uint32_t)(((int64_t)((int32_t)processor->regs[instr.MULHSU.rs1]) *
                                                           (uint64_t)processor->regs[instr.MULHSU.rs2]) >>
                                                          32);
            break;
        }
        case XPEmulatorEInstructionType_MULHU: {
            xp_emulator_print_op("MULHU");
            processor->regs[instr.MULHU.rd] =
              (uint32_t)(((uint64_t)processor->regs[instr.MULHU.rs1] * (uint64_t)processor->regs[instr.MULHU.rs2]) >>
                         32);
            break;
        }
        case XPEmulatorEInstructionType_DIV: {
            xp_emulator_print_op("DIV");
            if (processor->regs[instr.DIV.rs2] == 0) {
                processor->regs[instr.DIV.rd] = -1;
            } else {
                processor->regs[instr.DIV.rd] =
                  (uint32_t)((int32_t)(processor->regs[instr.DIV.rs1] == INT32_MIN &&
                                       (int32_t)processor->regs[instr.DIV.rs2] == -1)
                               ? processor->regs[instr.DIV.rs1]
                               : ((int32_t)processor->regs[instr.DIV.rs1] / (int32_t)processor->regs[instr.DIV.rs2]));
            }
            break;
        }
        case XPEmulatorEInstructionType_DIVU: {
            xp_emulator_print_op("DIVU");
            if (processor->regs[instr.DIVU.rs2] == 0) {
                processor->regs[instr.DIVU.rd] = 0xFFFFFFFF;
            } else {
                processor->regs[instr.DIVU.rd] =
                  (uint32_t)(processor->regs[instr.DIVU.rs1] / processor->regs[instr.DIVU.rs2]);
            }
            break;
        }
        case XPEmulatorEInstructionType_REM: {
            xp_emulator_print_op("REM");
            if (processor->regs[instr.REM.rs2] == 0) {
                processor->regs[instr.REM.rd] = processor->regs[instr.REM.rs1];
            } else {
                processor->regs[instr.REM.rd] = (uint32_t)(((int32_t)processor->regs[instr.REM.rs1] == INT32_MIN &&
                                                            (int32_t)processor->regs[instr.REM.rs2] == -1)
                                                             ? 0
                                                             : ((uint32_t)((int32_t)processor->regs[instr.REM.rs1] %
                                                                           (int32_t)processor->regs[instr.REM.rs2])));
            }
            break;
        }
        case XPEmulatorEInstructionType_REMU: {
            xp_emulator_print_op("REMU");
            if (processor->regs[instr.REMU.rs2] == 0) {
                processor->regs[instr.REMU.rd] = processor->regs[instr.REMU.rs1];
            } else {
                processor->regs[instr.REMU.rd] =
                  (uint32_t)(processor->regs[instr.REMU.rs1] % processor->regs[instr.REMU.rs2]);
            }
            break;
        }
    #if defined(XP_EMULATOR_USE_XLEN_64)
            // case XPEmulatorEInstructionType_MULW: {
            //     xp_emulator_print_op("MULW");
            //     break;
            // }
            // case XPEmulatorEInstructionType_DIVW: {
            //     xp_emulator_print_op("DIVW");
            //     break;
            // }
            // case XPEmulatorEInstructionType_DIVUW: {
            //     xp_emulator_print_op("DIVUW");
            //     break;
            // }
            // case XPEmulatorEInstructionType_REMW: {
            //     xp_emulator_print_op("REMW");
            //     break;
            // }
            // case XPEmulatorEInstructionType_REMUW: {
            //     xp_emulator_print_op("REMUW");
            //     break;
            // }
    #endif
#endif
        case XPEmulatorEInstructionType_FENCE: {
            xp_emulator_print_op("FENCE");
            return XP_EMULATOR_PROGRAM_EXIT_CODE_FENCE_UNIMPLEMENTED;
        }
        case XPEmulatorEInstructionType_ECALL: {
            // TODO: properly handle file system calls etc ..
            // we're only allowing a single file to be read/written to at the same time
            static FILE* file = NULL;

            xp_emulator_print_op("ECALL");
            uint32_t syscall_num = processor->regs[XPEmulatorEReg17];
            int32_t  arg0        = processor->regs[XPEmulatorEReg10];
            int32_t  arg1        = processor->regs[XPEmulatorEReg11];
            int32_t  arg2        = processor->regs[XPEmulatorEReg12];
            int32_t  arg3        = processor->regs[XPEmulatorEReg13];
            switch (syscall_num) {
                case XP_EMULATOR_SYSCALL_OPEN: {
                    assert(arg0 == -100 && "We're assuming dirfd is always AT_FDCWD");
                    XP_EMULATOR_LOGV_SYSCALL("SYSCALL OPEN (%i, %i, %i, %i)", arg0, arg1, arg2, arg3);
                    char     filePathBuffer[256];
                    uint32_t numFilePathBufferBytesWritten = xp_emulator_bus_load_str(
                      &processor->bus, (uint32_t)arg1, 8, filePathBuffer, sizeof(filePathBuffer));
                    XP_EMULATOR_LOGV_SYSCALL(
                      "fopen filepath: %s, flags: %i, mode: %i", filePathBuffer, (int32_t)arg2, (int32_t)arg3);
#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif
                    file = fopen((const char*)filePathBuffer, "r");
#ifdef __clang__
    #pragma clang diagnostic pop
#endif
                    if (file) {
                        // we are supposed to set a0 to the file descriptor ?
                        // f->_file is fd if unix, else is -1 !!
                        processor->regs[XPEmulatorEReg10] = XP_EMULATOR_FD_FROM_FILE(file);
                    }
                    break;
                }
                case XP_EMULATOR_SYSCALL_CLOCKGETTIME: {
                    XP_EMULATOR_LOGV_SYSCALL("SYSCALL CLOCKGETTIME (%i, %i, %i)", arg0, arg1, arg2);
                    XP_EMULATOR_LOG_SYSCALL("Unimplemented syscall CLOCKGETTIME");
                    return XP_EMULATOR_PROGRAM_EXIT_CODE_UNHANDLED_SYSCALL;
                }
                case XP_EMULATOR_SYSCALL_BRK: {
                    XP_EMULATOR_LOGV_SYSCALL("SYSCALL BRK (%i, %i, %i)", arg0, arg1, arg2);
                    XP_EMULATOR_LOG_SYSCALL("Unimplemented syscall BRK");
                    return XP_EMULATOR_PROGRAM_EXIT_CODE_UNHANDLED_SYSCALL;
                }
                case XP_EMULATOR_SYSCALL_GETTIMEOFDAY: {
                    XP_EMULATOR_LOGV_SYSCALL("SYSCALL GETTIMEOFDAY (%i, %i, %i)", arg0, arg1, arg2);
                    XP_EMULATOR_LOG_SYSCALL("Unimplemented syscall GETTIMEOFDAY");
                    return XP_EMULATOR_PROGRAM_EXIT_CODE_UNHANDLED_SYSCALL;
                }
                case XP_EMULATOR_SYSCALL_EXIT: {
                    XP_EMULATOR_LOGV_SYSCALL("SYSCALL EXIT (%i, %i, %i)", arg0, arg1, arg2);
                    int32_t exit_code = arg0;
                    printf("Exit code: %i\n", exit_code);
                    break;
                }
                case XP_EMULATOR_SYSCALL_FSTAT: {
                    XP_EMULATOR_LOGV_SYSCALL("SYSCALL FSTAT (%i, %i, %i)", arg0, arg1, arg2);
                    break;
                }
                case XP_EMULATOR_SYSCALL_WRITE: {
                    if (arg0 == STDOUT_FILENO || arg0 == STDERR_FILENO) {
                        XP_EMULATOR_LOGV_SYSCALL("SYSCALL WRITE (%i, %i, %i)", arg0, arg1, (int)((uint8_t)arg2));
                        uint8_t*       ptr    = &processor->bus.uart.data[arg1];
                        size_t         length = (int)((uint8_t)arg2);
                        unsigned char* buffer = (unsigned char*)malloc(sizeof(unsigned char) * length);
                        for (size_t i = 0; i < length; ++i) { buffer[i] = processor->bus.uart.data[arg1 + i]; }
                        size_t return_value               = fwrite(buffer, 1, length, stdout);
                        processor->regs[XPEmulatorEReg10] = return_value;
                        free(buffer);
                    }
                    break;
                }
                case XP_EMULATOR_SYSCALL_READ: {
                    XP_EMULATOR_LOGV_SYSCALL("SYSCALL READ (%i, %i, %i)", arg0, arg1, arg2);
                    int32_t fd = (int32_t)arg0;
                    if (fd == (int32_t)XP_EMULATOR_FD_FROM_FILE(file)) {
                        uint32_t bufferPtrAddr = (uint32_t)arg1;
                        uint32_t cnt           = (uint32_t)arg2;
                        uint8_t* buff = &processor->bus.memory.ram[bufferPtrAddr - XP_EMULATOR_CONFIG_MEMORY_RAM_BASE];
                        if (fread((void*)buff, cnt, 1, file) == 0) { processor->regs[XPEmulatorEReg10] = cnt; }
                    }
                    break;
                }
                case XP_EMULATOR_SYSCALL_LSEEK: {
                    printf("SYSCALL LSEEK (%i, %i, %i)\n", arg0, arg1, arg2);
                    int return_value                  = fseek(stdout, arg1, arg2);
                    processor->regs[XPEmulatorEReg10] = return_value;
                    break;
                }
                case XP_EMULATOR_SYSCALL_CLOSE: {
                    XP_EMULATOR_LOGV_SYSCALL("SYSCALL CLOSE (%i, %i, %i)", arg0, arg1, arg2);
                    if (file && ((uint32_t)XP_EMULATOR_FD_FROM_FILE(file)) == arg0) {
                        fclose(file);
                        file = NULL;
                    }
                    break;
                }

                default: {
                    XP_EMULATOR_LOGV_SYSCALL("UNHANDLED SYSCALL: (%i, %i, %i)", arg0, arg1, arg2);
                    return XP_EMULATOR_PROGRAM_EXIT_CODE_UNHANDLED_SYSCALL;
                }
            }
        } break;
        case XPEmulatorEInstructionType_EBREAK: {
            xp_emulator_print_op("EBREAK");
            return XP_EMULATOR_PROGRAM_EXIT_CODE_EBREAK;
        }

        default: {
            XP_EMULATOR_LOG("Unhandled EInstructionType");
            return XP_EMULATOR_PROGRAM_EXIT_CODE_UNKNOWN;
        }
    }

    processor->regs[XPEmulatorEReg0] = 0;
    processor->pc += pcIncr;

    return XP_EMULATOR_PROGRAM_EXIT_CODE_SUCCESS;
}

uint32_t
load(XPEmulatorProcessor* processor, uint32_t addr, uint32_t size)
{
    return xp_emulator_bus_load(&(processor->bus), addr, size);
}

void
store(XPEmulatorProcessor* processor, uint32_t addr, uint32_t size, uint32_t value)
{
    xp_emulator_bus_store(&(processor->bus), addr, size, value);
}

void
print_registers(XPEmulatorProcessor* processor)
{
#ifdef XP_EMULATOR_CONFIG_ENABLE_PRINT_REGS
    char* abi[] = {
        // Application Binary Interface registers
        "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2", "s0", "s1", "a0",  "a1",  "a2", "a3", "a4", "a5",
        "a6",   "a7", "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6",
    };

    /*for (int i=0; i<8; i++) {*/
    /*LOGV("%4s| x%02d: %#-8.2lx\t", abi[i],    i,    processor->registers[i]);*/
    /*LOGV("%4s| x%02d: %#-8.2lx\t", abi[i+8],  i+8,  processor->registers[i+8]);*/
    /*LOGV("%4s| x%02d: %#-8.2lx\t", abi[i+16], i+16, processor->registers[i+16]);*/
    /*LOGV("%4s| x%02d: %#-8.2lx\n", abi[i+24], i+24, processor->registers[i+24]);*/
    /*}*/

    for (int i = 0; i < 8; i++) {
        XP_EMULATOR_LOGV("%4s: %-10u %#-20.2x  %2s: %-10u %#-20.2x  %2s: %-10u %#-20.2x  %3s: %-10u %#-20.2x",
                         abi[i],
                         processor->regs[i],
                         processor->regs[i],
                         abi[i + 8],
                         processor->regs[i + 8],
                         processor->regs[i + 8],
                         abi[i + 16],
                         processor->regs[i + 16],
                         processor->regs[i + 16],
                         abi[i + 24],
                         processor->regs[i + 24],
                         processor->regs[i + 24]);
    }
    XP_EMULATOR_LOGV("%4s: %#-20.2x\n\n", "pc", processor->pc);
#endif
}