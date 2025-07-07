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

#include <Emulator/XPEmulatorSyscalls.h>
#include <Emulator/XPEmulatorUART.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

XP_EMULATOR_EXTERN void
xp_emulator_uart_initialize(XPEmulatorUART* uart)
{
    memset(uart->data, 0, XP_EMULATOR_CONFIG_UART_BUFFER_SIZE);
}

XP_EMULATOR_EXTERN uint32_t
xp_emulator_uart_load(XPEmulatorUART* uart, uint32_t address, uint32_t size)
{
    uint32_t uart_addr = address - XP_EMULATOR_CONFIG_UART_BASE;
    switch (size) {
        case 8: {
            return (uint32_t)uart->data[uart_addr];
        } break;

        case 16: {
            return (uint32_t)uart->data[uart_addr] | (uint32_t)uart->data[uart_addr + 1] << 8;
        } break;

        case 32: {
            return (uint32_t)uart->data[uart_addr] | (uint32_t)uart->data[uart_addr + 1] << 8 |
                   (uint32_t)uart->data[uart_addr + 2] << 16 | (uint32_t)uart->data[uart_addr + 3] << 24;
        } break;
    }

    exit(-1);
    return 0;
}

XP_EMULATOR_EXTERN void
xp_emulator_uart_store(XPEmulatorUART* uart, uint32_t address, uint32_t size, uint32_t value)
{
    uint32_t uart_addr = address - XP_EMULATOR_CONFIG_UART_BASE;
    switch (size) {
        case 8: {
            char c                = value;
            uart->data[uart_addr] = (uint8_t)c;
            printf("%c", c);
            fflush(stdout);
        } break;

        case 16: {
            uart->data[uart_addr]     = (uint8_t)(value & 0xff);
            uart->data[uart_addr + 1] = (uint8_t)((value >> 8) & 0xff);
            printf("%c%c", (char)(value & 0x000000FF), (char)((value >> 8) & 0xff));
            fflush(stdout);
        } break;

        case 32: {
            uart->data[uart_addr]     = (uint8_t)(value & 0xff);
            uart->data[uart_addr + 1] = (uint8_t)((value >> 8) & 0xff);
            uart->data[uart_addr + 2] = (uint8_t)((value >> 16) & 0xff);
            uart->data[uart_addr + 3] = (uint8_t)((value >> 24) & 0xff);
            printf("%c%c%c%c",
                   (char)(value & 0x000000FF),
                   (char)((value >> 8) & 0xff),
                   (char)((value >> 16) & 0xff),
                   (char)((value >> 24) & 0xff));
            fflush(stdout);
        } break;

        default: exit(-1); break;
    }
}

XP_EMULATOR_EXTERN void
xp_emulator_uart_finalize(XPEmulatorUART* uart)
{
    
}