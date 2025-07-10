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

#include <Emulator/XPEmulatorHostMappedMemory.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>

XP_EMULATOR_EXTERN void
xp_emulator_host_mapped_memory_initialize(XPEmulatorHostMappedMemory* memory)
{
    memset(memory->data, 0, XP_EMULATOR_CONFIG_HMM_SIZE);
}

XP_EMULATOR_EXTERN uint32_t
xp_emulator_host_mapped_memory_load(XPEmulatorHostMappedMemory* memory, uint32_t address, uint32_t size)
{
    uint32_t mem_address = address - XP_EMULATOR_CONFIG_HMM_BASE;
    switch (size) {
        case 8: {
            return (uint32_t)memory->data[mem_address];
        } break;

        case 16: {
            return (uint32_t)memory->data[mem_address] | (uint32_t)memory->data[mem_address + 1] << 8;
        } break;

        case 32: {
            return (uint32_t)memory->data[mem_address] | (uint32_t)memory->data[mem_address + 1] << 8 |
                   (uint32_t)memory->data[mem_address + 2] << 16 | (uint32_t)memory->data[mem_address + 3] << 24;
        } break;
    }

    assert(0 && "Unreachable");
    return 0;
}

XP_EMULATOR_EXTERN void
xp_emulator_host_mapped_memory_store(XPEmulatorHostMappedMemory* memory,
                                     uint32_t                    address,
                                     uint32_t                    size,
                                     uint32_t                    value)
{
    uint32_t mem_address = address - XP_EMULATOR_CONFIG_HMM_BASE;
    switch (size) {
        case 8: {
            memory->data[mem_address] = (uint8_t)(value & 0xff);
        } break;

        case 16: {
            memory->data[mem_address]     = (uint8_t)(value & 0xff);
            memory->data[mem_address + 1] = (uint8_t)((value >> 8) & 0xff);
        } break;

        case 32: {
            memory->data[mem_address]     = (uint8_t)(value & 0xff);
            memory->data[mem_address + 1] = (uint8_t)((value >> 8) & 0xff);
            memory->data[mem_address + 2] = (uint8_t)((value >> 16) & 0xff);
            memory->data[mem_address + 3] = (uint8_t)((value >> 24) & 0xff);
        } break;

        default: assert(0 && "Unreachable"); break;
    }
    if (mem_address == 0) {
        uint32_t memVal = *((uint32_t*)&memory->data[mem_address]);
        // we have requested a read here, lets do that now
        if (memVal == 0x1) {
            memory->deviceBottomStack =
              xp_emulator_host_mapped_memory_load(memory, XP_EMULATOR_CONFIG_HMM_BOTTOM_STACK_PTR, 32);
            memory->deviceTopStack =
              xp_emulator_host_mapped_memory_load(memory, XP_EMULATOR_CONFIG_HMM_TOP_STACK_PTR, 32);
            memory->deviceStartHeap =
              xp_emulator_host_mapped_memory_load(memory, XP_EMULATOR_CONFIG_HMM_START_HEAP_PTR, 32);
            memory->deviceEndHeap =
              xp_emulator_host_mapped_memory_load(memory, XP_EMULATOR_CONFIG_HMM_END_HEAP_PTR, 32);
            memory->deviceFramebuffer =
              xp_emulator_host_mapped_memory_load(memory, XP_EMULATOR_CONFIG_HMM_FRAMEBUFFER_PTR, 32);
            memory->deviceFrameMemoryPool =
              xp_emulator_host_mapped_memory_load(memory, XP_EMULATOR_CONFIG_HMM_FRAMEMEMPOOL_PTR, 32);
        }
    }
}

XP_EMULATOR_EXTERN void
xp_emulator_host_mapped_memory_finalize(XPEmulatorHostMappedMemory* memory)
{
}