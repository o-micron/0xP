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

#include "uart.h"
#include "syscalls.h"

void
uart_puts(const char* str, int cnt)
{
    for (int i = 0; i < cnt; ++i) { uart_putc(str[i]); }
    uart_putc('\n');
}

void
uart_putc(char c)
{
    // Wait until transmitter is idle
    // while ((UART_LSR & UART_LSR_TX_IDLE) != 0);

    // Special handling for newline (send CR+LF)
    if (c == '\r' || c == '\0' || c == '\n') {
        UART_THR = '\n';
    } else {
        UART_THR = c;
    }

    // riscv_syscall2(SYSCALL_WRITE, STDOUT_FILENO, 1);
}

char
uart_getc()
{
    // Wait for data
    // while ((UART_LSR & UART_LSR_RX_READY) != 0);
    return UART_RBR;
}