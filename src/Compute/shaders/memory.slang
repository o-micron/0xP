#include "shared/structures.h"

uint32_t
load_flash(XPXPUMemory* memory, uint32_t address, uint32_t size);
uint32_t
load_ram(XPXPUMemory* memory, uint32_t address, uint32_t size);
uint32_t
load_heap(XPXPUMemory* memory, uint32_t address, uint32_t size);

void
store_flash(XPXPUMemory* memory, uint32_t address, uint32_t size, uint32_t value);
void
store_ram(XPXPUMemory* memory, uint32_t address, uint32_t size, uint32_t value);
void
store_heap(XPXPUMemory* memory, uint32_t address, uint32_t size, uint32_t value);

uint32_t
xp_xpu_memory_load(XPXPUMemory* memory, uint32_t address, uint32_t size)
{
    if (address >= XP_XPU_CONFIG_MEMORY_FLASH_BASE &&
        address <= (XP_XPU_CONFIG_MEMORY_FLASH_BASE + XP_XPU_CONFIG_MEMORY_FLASH_SIZE)) {
        return load_flash(memory, address - XP_XPU_CONFIG_MEMORY_FLASH_BASE, size);
    } else if (address >= XP_XPU_CONFIG_MEMORY_RAM_BASE &&
               address <= (XP_XPU_CONFIG_MEMORY_RAM_BASE + XP_XPU_CONFIG_MEMORY_RAM_SIZE)) {
        return load_ram(memory, address - XP_XPU_CONFIG_MEMORY_RAM_BASE, size);
    } else if (address >= XP_XPU_CONFIG_MEMORY_HEAP_BASE &&
               address <= (XP_XPU_CONFIG_MEMORY_HEAP_BASE + XP_XPU_CONFIG_MEMORY_HEAP_SIZE)) {
        return load_heap(memory, address - XP_XPU_CONFIG_MEMORY_HEAP_BASE, size);
    } else {
        return -1;
    }
}

void
xp_xpu_memory_store(XPXPUMemory* memory, uint32_t address, uint32_t size, uint32_t value)
{
    if (address >= XP_XPU_CONFIG_MEMORY_FLASH_BASE &&
        address <= (XP_XPU_CONFIG_MEMORY_FLASH_BASE + XP_XPU_CONFIG_MEMORY_FLASH_SIZE)) {
        store_flash(memory, address - XP_XPU_CONFIG_MEMORY_FLASH_BASE, size, value);
    } else if (address >= XP_XPU_CONFIG_MEMORY_RAM_BASE &&
               address <= (XP_XPU_CONFIG_MEMORY_RAM_BASE + XP_XPU_CONFIG_MEMORY_RAM_SIZE)) {
        store_ram(memory, address - XP_XPU_CONFIG_MEMORY_RAM_BASE, size, value);
    } else if (address >= XP_XPU_CONFIG_MEMORY_HEAP_BASE &&
               address <= (XP_XPU_CONFIG_MEMORY_HEAP_BASE + XP_XPU_CONFIG_MEMORY_HEAP_SIZE)) {
        store_heap(memory, address - XP_XPU_CONFIG_MEMORY_HEAP_BASE, size, value);
    }
}

uint32_t
load_flash(XPXPUMemory* memory, uint32_t address, uint32_t size)
{
    switch (size) {
        case 8: {
            return (uint32_t)memory->flash[address];
        }

        case 16: {
            return (uint32_t)memory->flash[address] | (uint32_t)memory->flash[address + 1] << 8;
        }

        case 32: {
            return (uint32_t)memory->flash[address] | (uint32_t)memory->flash[address + 1] << 8 |
                   (uint32_t)memory->flash[address + 2] << 16 | (uint32_t)memory->flash[address + 3] << 24;
        }

        default: return 0;
    }
}

uint32_t
load_ram(XPXPUMemory* memory, uint32_t address, uint32_t size)
{
    switch (size) {
        case 8: {
            return (uint32_t)memory->ram[address];
        }

        case 16: {
            return (uint32_t)memory->ram[address] | (uint32_t)memory->ram[address + 1] << 8;
        }

        case 32: {
            return (uint32_t)memory->ram[address] | (uint32_t)memory->ram[address + 1] << 8 |
                   (uint32_t)memory->ram[address + 2] << 16 | (uint32_t)memory->ram[address + 3] << 24;
        }

        default: return 0;
    }
}

uint32_t
load_heap(XPXPUMemory* memory, uint32_t address, uint32_t size)
{
    switch (size) {
        case 8: {
            return (uint32_t)memory->heap[address];
        }

        case 16: {
            return (uint32_t)memory->heap[address] | (uint32_t)memory->heap[address + 1] << 8;
        }

        case 32: {
            return (uint32_t)memory->heap[address] | (uint32_t)memory->heap[address + 1] << 8 |
                   (uint32_t)memory->heap[address + 2] << 16 | (uint32_t)memory->heap[address + 3] << 24;
        }

        default: return 0;
    }
}

void
store_flash(XPXPUMemory* memory, uint32_t address, uint32_t size, uint32_t value)
{
    switch (size) {
        case 8: {
            memory->flash[address] = (uint8_t)(value & 0xff);
        } break;

        case 16: {
            memory->flash[address]     = (uint8_t)(value & 0xff);
            memory->flash[address + 1] = (uint8_t)((value >> 8) & 0xff);
        } break;

        case 32: {
            memory->flash[address]     = (uint8_t)(value & 0xff);
            memory->flash[address + 1] = (uint8_t)((value >> 8) & 0xff);
            memory->flash[address + 2] = (uint8_t)((value >> 16) & 0xff);
            memory->flash[address + 3] = (uint8_t)((value >> 24) & 0xff);
        } break;

        default: break;
    }
}

void
store_ram(XPXPUMemory* memory, uint32_t address, uint32_t size, uint32_t value)
{
    switch (size) {
        case 8: {
            memory->ram[address] = (uint8_t)(value & 0xff);
        } break;

        case 16: {
            memory->ram[address]     = (uint8_t)(value & 0xff);
            memory->ram[address + 1] = (uint8_t)((value >> 8) & 0xff);
        } break;

        case 32: {
            memory->ram[address]     = (uint8_t)(value & 0xff);
            memory->ram[address + 1] = (uint8_t)((value >> 8) & 0xff);
            memory->ram[address + 2] = (uint8_t)((value >> 16) & 0xff);
            memory->ram[address + 3] = (uint8_t)((value >> 24) & 0xff);
        } break;

        default: break;
    }
}

void
store_heap(XPXPUMemory* memory, uint32_t address, uint32_t size, uint32_t value)
{
    switch (size) {
        case 8: {
            memory->heap[address] = (uint8_t)(value & 0xff);
        } break;

        case 16: {
            memory->heap[address]     = (uint8_t)(value & 0xff);
            memory->heap[address + 1] = (uint8_t)((value >> 8) & 0xff);
        } break;

        case 32: {
            memory->heap[address]     = (uint8_t)(value & 0xff);
            memory->heap[address + 1] = (uint8_t)((value >> 8) & 0xff);
            memory->heap[address + 2] = (uint8_t)((value >> 16) & 0xff);
            memory->heap[address + 3] = (uint8_t)((value >> 24) & 0xff);
        } break;

        default: break;
    }
}
