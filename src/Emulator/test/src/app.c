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

#include "../common/syscalls.h"
#include "../common/uart.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define ITERATIONS 10000

int
dosum()
{
    int out = 0;
    for (int i = 0; i <= ITERATIONS; i++) { out++; }
    return out;
}

int
dodiff()
{
    int out = 0;
    for (int i = 0; i <= ITERATIONS; i++) { out--; }
    return out;
}

float
domul()
{
    float out = 1.0f;
    for (int i = 0; i <= 10; i++) { out *= 2.0f; }
    return out;
}

int
main()
{
#if defined(__riscv)
    #if __riscv_xlen == 32
    printf("RUNNING ON RV32\n");
    #elif __riscv_xlen == 64
    printf("RUNNING ON RV64\n");
    #elif __riscv_xlen == 128
    printf("RUNNING ON RV128\n");
    #endif
#endif

    int   sum  = 0;
    int   diff = 0;
    float fmul = 0.f;
    printf("sum: %i, diff: %i, fmul: %f\n", sum, diff, fmul);

    sum  = dosum();
    diff = dodiff();
    fmul = domul();
    printf("sum: %i, diff: %i, fmul: %f\n", sum, diff, fmul);

    // test putc
    uart_putc('h');
    uart_putc('e');
    uart_putc('l');
    uart_putc('l');
    uart_putc('o');
    uart_putc('\n');

    // test puts
    puts("program can use puts.");
    // test fwrite stdout
    fwrite("program can use fwrite", sizeof("program can use fwrite"), 1, stdout);
    // test fwrite stderr
    fwrite("program can use fwrite with stderr", sizeof("program can use fwrite with stderr"), 1, stderr);

    // test dynamic allocation
    int* a = (int*)malloc(sizeof(int));
    *a     = 11;
    int* b = (int*)malloc(sizeof(int));
    *b     = 22;
    int c  = *a + *b;

    return c;
}