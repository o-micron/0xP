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

// SAME AS EMULATOR //
#define UART_BASE         0x10000000
#define UART_THR          *((volatile char*)(UART_BASE + 0x00)) // Transmit holding register
#define UART_RBR          *((volatile char*)(UART_BASE + 0x00)) // Receive buffer register
#define UART_LSR          *((volatile char*)(UART_BASE + 0x05)) // Line status register
#define UART_LSR_TX_IDLE  (1 << 5)                              // Transmitter idle
#define UART_LSR_RX_READY (1 << 0)                              // Receiver

// #define ENABLE_DBG_PRINT
#ifdef ENABLE_DBG_PRINT
    #define DBG_PRNT(A) uart_puts(A, sizeof(A));
#else
    #define DBG_PRNT(A)
#endif

void
uart_puts(const char* str, int cnt);

void
uart_putc(char c);

char
uart_getc();