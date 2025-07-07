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

#include <Emulator/XPEmulatorMemory.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>

uint32_t
load_flash(XPEmulatorMemory* memory, uint32_t address, uint32_t size);
uint32_t
load_ram(XPEmulatorMemory* memory, uint32_t address, uint32_t size);
uint32_t
load_heap(XPEmulatorMemory* memory, uint32_t address, uint32_t size);

void
store_flash(XPEmulatorMemory* memory, uint32_t address, uint32_t size, uint32_t value);
void
store_ram(XPEmulatorMemory* memory, uint32_t address, uint32_t size, uint32_t value);
void
store_heap(XPEmulatorMemory* memory, uint32_t address, uint32_t size, uint32_t value);

void
xp_emulator_memory_initialize(XPEmulatorMemory* memory)
{
    memset(memory->flash, 0, XP_EMULATOR_CONFIG_MEMORY_FLASH_SIZE);
    memset(memory->ram, 0, XP_EMULATOR_CONFIG_MEMORY_RAM_SIZE);
    memset(memory->heap, 0, XP_EMULATOR_CONFIG_MEMORY_HEAP_SIZE);
}

uint32_t
xp_emulator_memory_load(XPEmulatorMemory* memory, uint32_t address, uint32_t size)
{
    if (address >= XP_EMULATOR_CONFIG_MEMORY_FLASH_BASE &&
        address <= (XP_EMULATOR_CONFIG_MEMORY_FLASH_BASE + XP_EMULATOR_CONFIG_MEMORY_FLASH_SIZE)) {
        return load_flash(memory, address - XP_EMULATOR_CONFIG_MEMORY_FLASH_BASE, size);
    } else if (address >= XP_EMULATOR_CONFIG_MEMORY_RAM_BASE &&
               address <= (XP_EMULATOR_CONFIG_MEMORY_RAM_BASE + XP_EMULATOR_CONFIG_MEMORY_RAM_SIZE)) {
        return load_ram(memory, address - XP_EMULATOR_CONFIG_MEMORY_RAM_BASE, size);
    } else if (address >= XP_EMULATOR_CONFIG_MEMORY_HEAP_BASE &&
               address <= (XP_EMULATOR_CONFIG_MEMORY_HEAP_BASE + XP_EMULATOR_CONFIG_MEMORY_HEAP_SIZE)) {
        return load_heap(memory, address - XP_EMULATOR_CONFIG_MEMORY_HEAP_BASE, size);
    } else {
        assert(0 && "Unreachable");
        return -1;
    }
}

void
xp_emulator_memory_store(XPEmulatorMemory* memory, uint32_t address, uint32_t size, uint32_t value)
{
    if (address >= XP_EMULATOR_CONFIG_MEMORY_FLASH_BASE &&
        address <= (XP_EMULATOR_CONFIG_MEMORY_FLASH_BASE + XP_EMULATOR_CONFIG_MEMORY_FLASH_SIZE)) {
        store_flash(memory, address - XP_EMULATOR_CONFIG_MEMORY_FLASH_BASE, size, value);
    } else if (address >= XP_EMULATOR_CONFIG_MEMORY_RAM_BASE &&
               address <= (XP_EMULATOR_CONFIG_MEMORY_RAM_BASE + XP_EMULATOR_CONFIG_MEMORY_RAM_SIZE)) {
        store_ram(memory, address - XP_EMULATOR_CONFIG_MEMORY_RAM_BASE, size, value);
    } else if (address >= XP_EMULATOR_CONFIG_MEMORY_HEAP_BASE &&
               address <= (XP_EMULATOR_CONFIG_MEMORY_HEAP_BASE + XP_EMULATOR_CONFIG_MEMORY_HEAP_SIZE)) {
        store_heap(memory, address - XP_EMULATOR_CONFIG_MEMORY_HEAP_BASE, size, value);
    } else {
        assert(0 && "Unreachable");
    }
}

void
xp_emulator_memory_finalize(XPEmulatorMemory* memory)
{
}

uint32_t
load_flash(XPEmulatorMemory* memory, uint32_t address, uint32_t size)
{
    switch (size) {
        case 8: {
            return (uint32_t)memory->flash[address];
        } break;

        case 16: {
            return (uint32_t)memory->flash[address] | (uint32_t)memory->flash[address + 1] << 8;
        } break;

        case 32: {
            return (uint32_t)memory->flash[address] | (uint32_t)memory->flash[address + 1] << 8 |
                   (uint32_t)memory->flash[address + 2] << 16 | (uint32_t)memory->flash[address + 3] << 24;
        } break;

        default: assert(0 && "Unreachable"); return 0;
    }
}

uint32_t
load_ram(XPEmulatorMemory* memory, uint32_t address, uint32_t size)
{
    switch (size) {
        case 8: {
            return (uint32_t)memory->ram[address];
        } break;

        case 16: {
            return (uint32_t)memory->ram[address] | (uint32_t)memory->ram[address + 1] << 8;
        } break;

        case 32: {
            return (uint32_t)memory->ram[address] | (uint32_t)memory->ram[address + 1] << 8 |
                   (uint32_t)memory->ram[address + 2] << 16 | (uint32_t)memory->ram[address + 3] << 24;
        } break;

        default: assert(0 && "Unreachable"); return 0;
    }
}

uint32_t
load_heap(XPEmulatorMemory* memory, uint32_t address, uint32_t size)
{
    switch (size) {
        case 8: {
            return (uint32_t)memory->heap[address];
        } break;

        case 16: {
            return (uint32_t)memory->heap[address] | (uint32_t)memory->heap[address + 1] << 8;
        } break;

        case 32: {
            return (uint32_t)memory->heap[address] | (uint32_t)memory->heap[address + 1] << 8 |
                   (uint32_t)memory->heap[address + 2] << 16 | (uint32_t)memory->heap[address + 3] << 24;
        } break;

        default: assert(0 && "Unreachable"); return 0;
    }
}

void
store_flash(XPEmulatorMemory* memory, uint32_t address, uint32_t size, uint32_t value)
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

        default: assert(0 && "Unreachable"); break;
    }
}

void
store_ram(XPEmulatorMemory* memory, uint32_t address, uint32_t size, uint32_t value)
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

        default: assert(0 && "Unreachable"); break;
    }
}

void
store_heap(XPEmulatorMemory* memory, uint32_t address, uint32_t size, uint32_t value)
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

        default: assert(0 && "Unreachable"); break;
    }
}