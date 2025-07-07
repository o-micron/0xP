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

#include <stdint.h>

#define SYSCALL_OPEN         1024
#define SYSCALL_CLOCKGETTIME 403
#define SYSCALL_BRK          214
#define SYSCALL_GETTIMEOFDAY 169
#define SYSCALL_EXIT         93
#define SYSCALL_FSTAT        80
#define SYSCALL_WRITE        64
#define SYSCALL_READ         63
#define SYSCALL_LSEEK        62
#define SYSCALL_CLOSE        57

#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

#ifdef __cplusplus
    #define XPEXTERN extern "C"
#else
    #define XPEXTERN extern
#endif

XPEXTERN uint32_t
riscv_syscall1(uint32_t num, uint32_t arg0);

XPEXTERN uint32_t
riscv_syscall2(uint32_t num, uint32_t arg0, uint32_t arg1);

XPEXTERN uint32_t
riscv_syscall3(uint32_t num, uint32_t arg0, uint32_t arg1, uint32_t arg2);

XPEXTERN uint32_t
riscv_syscall4(uint32_t num, uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3);