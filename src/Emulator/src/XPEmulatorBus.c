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

#include <Emulator/XPEmulatorBus.h>
#include <Emulator/XPEmulatorLogger.h>

#include <assert.h>
#include <stdio.h>
#include <string.h>

XP_EMULATOR_EXTERN void
xp_emulator_bus_initialize(XPEmulatorBus* bus)
{
    xp_emulator_memory_initialize(&bus->memory);
    xp_emulator_uart_initialize(&bus->uart);
    xp_emulator_host_mapped_memory_initialize(&bus->hostMappedMemory);
}

XP_EMULATOR_EXTERN uint32_t
xp_emulator_bus_load(XPEmulatorBus* bus, uint32_t address, uint32_t size)
{
    if (address >= XP_EMULATOR_CONFIG_UART_BASE &&
        address <= (XP_EMULATOR_CONFIG_UART_BASE + XP_EMULATOR_CONFIG_UART_BUFFER_SIZE)) {
        // uart
        XP_EMULATOR_LOG_BUS("load uart");
        return xp_emulator_uart_load(&bus->uart, address, size);
    } else if (address >= XP_EMULATOR_CONFIG_HMM_BASE &&
               address <= (XP_EMULATOR_CONFIG_HMM_BASE + XP_EMULATOR_CONFIG_HMM_SIZE)) {
        XP_EMULATOR_LOG_BUS("load host mapped memory");
        return xp_emulator_host_mapped_memory_load(&bus->hostMappedMemory, address, size);
    } else {
        if (address >= XP_EMULATOR_CONFIG_MAIN_MEMORY_RANGE_START &&
            address <= XP_EMULATOR_CONFIG_MAIN_MEMORY_RANGE_END) {
            XP_EMULATOR_LOG_BUS("load ram");
            return xp_emulator_memory_load(&bus->memory, address, size);
        } else {
            XP_EMULATOR_LOG_BUS("load unkown");
            assert(0 && "Unreachable");
        }
    }

    return 0;
}

XP_EMULATOR_EXTERN void
xp_emulator_bus_store(XPEmulatorBus* bus, uint32_t address, uint32_t size, uint32_t value)
{
    if (address >= XP_EMULATOR_CONFIG_UART_BASE &&
        address <= (XP_EMULATOR_CONFIG_UART_BASE + XP_EMULATOR_CONFIG_UART_BUFFER_SIZE)) {
        // uart
        XP_EMULATOR_LOG_BUS("store uart");
        xp_emulator_uart_store(&bus->uart, address, size, value);
    } else if (address >= XP_EMULATOR_CONFIG_HMM_BASE &&
               address <= (XP_EMULATOR_CONFIG_HMM_BASE + XP_EMULATOR_CONFIG_HMM_SIZE)) {
        XP_EMULATOR_LOG_BUS("store host mapped memory");
        xp_emulator_host_mapped_memory_store(&bus->hostMappedMemory, address, size, value);
    } else {
        if (address >= XP_EMULATOR_CONFIG_MAIN_MEMORY_RANGE_START &&
            address <= XP_EMULATOR_CONFIG_MAIN_MEMORY_RANGE_END) {
            XP_EMULATOR_LOG_BUS("store ram");
            xp_emulator_memory_store(&bus->memory, address, size, value);
        } else {
            XP_EMULATOR_LOG_BUS("store unkown");
            assert(0 && "Unreachable");
            // xp_emulator_uart_store(&bus->uart, address + 0x10000000, size, value);
        }
    }
}

XP_EMULATOR_EXTERN void
xp_emulator_bus_finalize(XPEmulatorBus* bus)
{
    xp_emulator_host_mapped_memory_finalize(&bus->hostMappedMemory);
    xp_emulator_uart_finalize(&bus->uart);
    xp_emulator_memory_finalize(&bus->memory);
}

XP_EMULATOR_EXTERN uint32_t
xp_emulator_bus_load_str(XPEmulatorBus* bus, uint32_t address, uint32_t size, char* buffer, uint32_t bufferNumBytes)
{
    memset(buffer, '\0', bufferNumBytes);
    uint32_t pathAddressIndex = address;
    uint32_t hostBufferIndex  = 0;
    while ((char)xp_emulator_bus_load(bus, pathAddressIndex, 8) != '\0' && hostBufferIndex < bufferNumBytes) {
        buffer[hostBufferIndex] = (char)xp_emulator_bus_load(bus, pathAddressIndex, 8);
        ++hostBufferIndex;
        ++pathAddressIndex;
    }
    return hostBufferIndex;
}