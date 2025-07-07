#include "shared/structures.h"

uint32_t
xp_xpu_host_mapped_memory_load(XPXPUHostMappedMemory* memory, uint32_t address, uint32_t size)
{
    uint32_t mem_address = address - XP_XPU_CONFIG_HMM_BASE;
    switch (size) {
        case 8: {
            return (uint32_t)memory->data[mem_address];
        }
        case 16: {
            return (uint32_t)memory->data[mem_address] | (uint32_t)memory->data[mem_address + 1] << 8;
        }
        case 32: {
            return (uint32_t)memory->data[mem_address] | (uint32_t)memory->data[mem_address + 1] << 8 |
                   (uint32_t)memory->data[mem_address + 2] << 16 | (uint32_t)memory->data[mem_address + 3] << 24;
        }
        default: return 0;
    }
}

void
xp_xpu_host_mapped_memory_store(XPXPUHostMappedMemory* memory, uint32_t address, uint32_t size, uint32_t value)
{
    uint32_t mem_address = address - XP_XPU_CONFIG_HMM_BASE;
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

        default: break;
    }
    if (mem_address == 0) {
        uint32_t memVal = *((uint32_t*)&memory->data[mem_address]);
        if (memVal == 0x1) {
            memory->deviceBottomStack = xp_xpu_host_mapped_memory_load(memory, XP_XPU_CONFIG_HMM_BOTTOM_STACK_PTR, 32);
            memory->deviceTopStack    = xp_xpu_host_mapped_memory_load(memory, XP_XPU_CONFIG_HMM_TOP_STACK_PTR, 32);
            memory->deviceStartHeap   = xp_xpu_host_mapped_memory_load(memory, XP_XPU_CONFIG_HMM_START_HEAP_PTR, 32);
            memory->deviceEndHeap     = xp_xpu_host_mapped_memory_load(memory, XP_XPU_CONFIG_HMM_END_HEAP_PTR, 32);
        }
    }
}
