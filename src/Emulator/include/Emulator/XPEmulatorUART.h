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

typedef struct XPEmulatorUART
{
    uint8_t data[XP_EMULATOR_CONFIG_UART_BUFFER_SIZE];
} XPEmulatorUART;

XP_EMULATOR_EXTERN void
xp_emulator_uart_initialize(XPEmulatorUART* uart);

XP_EMULATOR_EXTERN uint32_t
xp_emulator_uart_load(XPEmulatorUART* uart, uint32_t address, uint32_t size);

XP_EMULATOR_EXTERN void
xp_emulator_uart_store(XPEmulatorUART* uart, uint32_t address, uint32_t size, uint32_t value);

XP_EMULATOR_EXTERN void
xp_emulator_uart_finalize(XPEmulatorUART* uart);