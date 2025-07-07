#pragma once

#include "config.h"
#include "enums.h"

#ifndef __SLANG__
    #include <stdint.h>
#endif

struct XPXPUHostMappedMemory
{
    uint8_t  data[XP_XPU_CONFIG_HMM_SIZE];
    uint32_t deviceBottomStack;
    uint32_t deviceTopStack;
    uint32_t deviceStartHeap;
    uint32_t deviceEndHeap;
};

struct XPXPUMemory
{
    uint8_t flash[XP_XPU_CONFIG_MEMORY_FLASH_SIZE];
    uint8_t ram[XP_XPU_CONFIG_MEMORY_RAM_SIZE];
    uint8_t heap[XP_XPU_CONFIG_MEMORY_HEAP_SIZE];
};

struct XPXPUBus
{
    XPXPUMemory           memory;
    XPXPUHostMappedMemory hostMappedMemory;
};

struct XPXPUProcessor
{
    uint32_t regs[int(XPXPUEReg::RCount)];
    uint32_t pc;
    XPXPUBus bus;
};