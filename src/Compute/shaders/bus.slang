#include "shared/structures.h"

#include "hostMappedMemory.slang"
#include "memory.slang"

uint32_t
xp_xpu_bus_load(XPXPUBus* bus, uint32_t address, uint32_t size)
{
    if (address >= XP_XPU_CONFIG_HMM_BASE && address <= (XP_XPU_CONFIG_HMM_BASE + XP_XPU_CONFIG_HMM_SIZE)) {
        return xp_xpu_host_mapped_memory_load(&bus->hostMappedMemory, address, size);
    } else {
        if (address >= XP_XPU_CONFIG_MAIN_MEMORY_RANGE_START && address <= XP_XPU_CONFIG_MAIN_MEMORY_RANGE_END) {
            return xp_xpu_memory_load(&bus->memory, address, size);
        } else {
            return 0;
        }
    }
}

void
xp_xpu_bus_store(XPXPUBus* bus, uint32_t address, uint32_t size, uint32_t value)
{
    if (address >= XP_XPU_CONFIG_HMM_BASE && address <= (XP_XPU_CONFIG_HMM_BASE + XP_XPU_CONFIG_HMM_SIZE)) {
        xp_xpu_host_mapped_memory_store(&bus->hostMappedMemory, address, size, value);
    } else {
        if (address >= XP_XPU_CONFIG_MAIN_MEMORY_RANGE_START && address <= XP_XPU_CONFIG_MAIN_MEMORY_RANGE_END) {
            xp_xpu_memory_store(&bus->memory, address, size, value);
        }
    }
}
