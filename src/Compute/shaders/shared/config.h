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

// #define XP_XPU_CONFIG_ENABLE_PRINT_OP
// #define XP_XPU_CONFIG_ENABLE_PRINT_REGS
// #define XP_XPU_CONFIG_ENABLE_LOGS
// #define XP_XPU_CONFIG_ENABLE_SYSCALL_LOGS
// #define XP_XPU_CONFIG_ENABLE_BUS_LOGS

#define XP_XPU_CONFIG_HMM_SIZE             (128 * 1024U)                                // 128 KB
#define XP_XPU_CONFIG_HMM_BASE             (0x8000000U)                                 // 128 MB
#define XP_XPU_CONFIG_HMM_BOTTOM_STACK_PTR (XP_XPU_CONFIG_HMM_BASE + 0x05U)             // STACK BOTTOM (0x1)
#define XP_XPU_CONFIG_HMM_TOP_STACK_PTR    (XP_XPU_CONFIG_HMM_BOTTOM_STACK_PTR + 0x05U) // STACK TOP (0x1)
#define XP_XPU_CONFIG_HMM_START_HEAP_PTR   (XP_XPU_CONFIG_HMM_TOP_STACK_PTR + 0x05U)    // HEAP START (0x1)
#define XP_XPU_CONFIG_HMM_END_HEAP_PTR     (XP_XPU_CONFIG_HMM_START_HEAP_PTR + 0x05U)   // HEAP END (0x1)
#define XP_XPU_CONFIG_HMM_SMPTR            (XP_XPU_CONFIG_HMM_END_HEAP_PTR + 0x05U)     // STATE MACHINE POINTER (0x2)
#define XP_XPU_CONFIG_HMM_FETCHPTR         (XP_XPU_CONFIG_HMM_SMPTR + 0x05U)            // FETCH DATA POINTER (0x3)

// SMD
#define XP_XPU_CONFIG_HMM_SMD_VALUE_CAMERA                  1
#define XP_XPU_CONFIG_HMM_SMD_VALUE_NUM_TEXTURES            2
#define XP_XPU_CONFIG_HMM_SMD_VALUE_NUM_MESH_BUFFER_OBJECTS 3
#define XP_XPU_CONFIG_HMM_SMD_VALUE_NEXT_TRIANGLE           4

// #define XP_XPU_CONFIG_MEMORY_SIZE       (7 * 1024U * 1024U) // 7 MB
#define XP_XPU_CONFIG_MEMORY_FLASH_BASE (0x4000000U)  // 64 MB
#define XP_XPU_CONFIG_MEMORY_RAM_BASE   (0x80000000U) // 2 GB
#define XP_XPU_CONFIG_MEMORY_HEAP_BASE  (0x80400040U) // 2 GB + 4 MB

#define XP_XPU_CONFIG_MEMORY_FLASH_SIZE (1 * 1024U * 1024U) // 1 MB
#define XP_XPU_CONFIG_MEMORY_RAM_SIZE   (4 * 1024U * 1024U) // 4 MB
#define XP_XPU_CONFIG_MEMORY_HEAP_SIZE  (2 * 1024U * 1024U) // 2 MB

#define XP_XPU_CONFIG_MAIN_MEMORY_RANGE_START (XP_XPU_CONFIG_MEMORY_FLASH_BASE)
#define XP_XPU_CONFIG_MAIN_MEMORY_RANGE_END   (XP_XPU_CONFIG_MEMORY_HEAP_BASE + XP_XPU_CONFIG_MEMORY_HEAP_SIZE)

// define it for using riscv M extension
#define XP_XPU_USE_M_EXTENSION

// define it for riscv64
#if defined(__riscv)
    #if __riscv_xlen == 64
        #define XP_XPU_USE_XLEN_64
    #endif
#endif

#ifdef __cplusplus
    #define XP_XPU_EXTERN extern "C"
#else
    #define XP_XPU_EXTERN
#endif

// SAME AS EMULATOR //
#define XP_XPU_CONFIG_UART_BUFFER_SIZE  (4 * 1024U)                       // 4 KB
#define XP_XPU_CONFIG_UART_BASE         (0x10000000U)                     // 256 MB
#define XP_XPU_CONFIG_UART_THR          (XP_XPU_CONFIG_UART_BASE + 0x00U) // Transmit holding register
#define XP_XPU_CONFIG_UART_RBR          (XP_XPU_CONFIG_UART_BASE + 0x00U) // Receive buffer register
#define XP_XPU_CONFIG_UART_LSR          (XP_XPU_CONFIG_UART_BASE + 0x05U) // Line status register
#define XP_XPU_CONFIG_UART_LSR_TX_IDLE  (1 << 5)                          // Transmitter idle
#define XP_XPU_CONFIG_UART_LSR_RX_READY (1 << 0)                          // Receiver
