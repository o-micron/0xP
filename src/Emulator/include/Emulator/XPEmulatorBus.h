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

#include <Emulator/XPEmulatorConfig.h>
#include <Emulator/XPEmulatorHostMappedMemory.h>
#include <Emulator/XPEmulatorMemory.h>
#include <Emulator/XPEmulatorUART.h>

#include <stdint.h>

typedef struct XPEmulatorBus
{
    struct XPEmulatorMemory           memory;
    struct XPEmulatorUART             uart;
    struct XPEmulatorHostMappedMemory hostMappedMemory;
} XPEmulatorBus;

XP_EMULATOR_EXTERN void
xp_emulator_bus_initialize(XPEmulatorBus* bus);

XP_EMULATOR_EXTERN uint32_t
xp_emulator_bus_load(XPEmulatorBus* bus, uint32_t address, uint32_t size);

XP_EMULATOR_EXTERN void
xp_emulator_bus_store(XPEmulatorBus* bus, uint32_t address, uint32_t size, uint32_t value);

XP_EMULATOR_EXTERN void
xp_emulator_bus_finalize(XPEmulatorBus* bus);

XP_EMULATOR_EXTERN uint32_t
xp_emulator_bus_load_str(XPEmulatorBus* bus, uint32_t address, uint32_t size, char* buffer, uint32_t bufferNumBytes);