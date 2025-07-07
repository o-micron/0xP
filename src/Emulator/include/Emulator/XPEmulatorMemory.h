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

#pragma once

#include <Emulator/XPEmulatorCommon.h>
#include <Emulator/XPEmulatorConfig.h>

#include <stdint.h>

typedef struct XPEmulatorMemory
{
    uint8_t flash[XP_EMULATOR_CONFIG_MEMORY_FLASH_SIZE];
    uint8_t ram[XP_EMULATOR_CONFIG_MEMORY_RAM_SIZE];
    uint8_t heap[XP_EMULATOR_CONFIG_MEMORY_HEAP_SIZE];
} XPEmulatorMemory;

XP_EMULATOR_EXTERN void
xp_emulator_memory_initialize(XPEmulatorMemory* memory);

XP_EMULATOR_EXTERN uint32_t
xp_emulator_memory_load(XPEmulatorMemory* memory, uint32_t address, uint32_t size);

XP_EMULATOR_EXTERN void
xp_emulator_memory_store(XPEmulatorMemory* memory, uint32_t address, uint32_t size, uint32_t value);

XP_EMULATOR_EXTERN void
xp_emulator_memory_finalize(XPEmulatorMemory* memory);
