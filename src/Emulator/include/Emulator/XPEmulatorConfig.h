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

// #define XP_EMULATOR_CONFIG_ENABLE_PRINT_OP
// #define XP_EMULATOR_CONFIG_ENABLE_PRINT_REGS
// #define XP_EMULATOR_CONFIG_ENABLE_LOGS
// #define XP_EMULATOR_CONFIG_ENABLE_SYSCALL_LOGS
// #define XP_EMULATOR_CONFIG_ENABLE_BUS_LOGS

// assets are stored here
// - (num meshes, num cameras, num lights, num materials)
//      - [(numVertices, numNormals, numTexcoords, numColors, numTangents, numBiTangents, numIndices) for each mesh]
//          - transform, vertices, normals, texcoords, colors, tangents, bitangents, indices, boundingBox, materialIndex
//
// 1920 * 1080 * 4 bytes per pixel = 8,294,400 (~8MB)
// #define XP_EMULATOR_CONFIG_HMM_SIZE  (128 * 1024U)                         // 128 KB
// #define XP_EMULATOR_CONFIG_HMM_BASE  (0x8000000U)                          // MOSTLY UNUSED
// #define XP_EMULATOR_CONFIG_HMM_SMD   (XP_EMULATOR_CONFIG_HMM_BASE + 0x04U) // STATE MACHINE POINTER
// #define XP_EMULATOR_CONFIG_HMM_FETCH (XP_EMULATOR_CONFIG_HMM_SMD + 0x04U)  // FETCH DATA POINTER

// // SMD
// #define XP_EMULATOR_CONFIG_HMM_SMD_VALUE_CAMERA                  1
// #define XP_EMULATOR_CONFIG_HMM_SMD_VALUE_NUM_TEXTURES            2
// #define XP_EMULATOR_CONFIG_HMM_SMD_VALUE_NUM_MESH_BUFFER_OBJECTS 3
// #define XP_EMULATOR_CONFIG_HMM_SMD_VALUE_NEXT_TRIANGLE           4

// //
// --------------------------------------------------------------------------------------------------------------------
// // TEXTURES
// //
// --------------------------------------------------------------------------------------------------------------------
// // In emulator we typically don't want to have textures occupying lots of memory, we keep it on host side and we just
// // deal with host side as if it's a texture unit or something like that.
// // We issue requests to the host in order to read/write pixels from any texture we want.
// //
// // In order to read or write from textures, we need to provide key info like the pixel row/col location.
// // In case of write we need the pixel value we want to write
// // In case of read we need the address ((RAM) Risc-V program address space) to assign the pixel value to it from
// host.

// // clang-format off
// #define XP_EMULATOR_CONFIG_HMM_TEXTURE_NUM_TEXTURES_REG     (XP_EMULATOR_CONFIG_HMM_BASE + 4)
// #define XP_EMULATOR_CONFIG_HMM_TEXTURE_INDEX_READ_REG       (XP_EMULATOR_CONFIG_HMM_BASE + 8)
// #define XP_EMULATOR_CONFIG_HMM_TEXTURE_INDEX_WRITE_REG      (XP_EMULATOR_CONFIG_HMM_BASE + 8)
// #define XP_EMULATOR_CONFIG_HMM_TEXTURE_READ_VAL_REG         (XP_EMULATOR_CONFIG_HMM_BASE + 12)  // (4 bytes) 32 bit
// #define XP_EMULATOR_CONFIG_HMM_TEXTURE_WRITE_TO_ADDRESS_REG (XP_EMULATOR_CONFIG_HMM_BASE + 16) // (4 bytes) 32 bit
// #define XP_EMULATOR_CONFIG_HMM_TEXTURE_OP_PIXEL_ROW_REG     (XP_EMULATOR_CONFIG_HMM_BASE + 20) // (4 bytes) 32 bit
// #define XP_EMULATOR_CONFIG_HMM_TEXTURE_OP_PIXEL_COL_REG     (XP_EMULATOR_CONFIG_HMM_BASE + 24) // (4 bytes) 32 bit
// // clang-format on
// //
// --------------------------------------------------------------------------------------------------------------------

// #define XP_EMULATOR_CONFIG_MEMORY_SIZE (6 * 1024U * 1024U) // 6 MB
// #define XP_EMULATOR_CONFIG_MEMORY_BASE (0x80000000U)       // 2 GB

// // SAME AS EMULATOR //
// #define XP_EMULATOR_UART_BUFFER_SIZE         (4 * 1024U)                            // 4 KB
// #define XP_EMULATOR_CONFIG_UART_BASE         (0x10000000U)                          // 256 MB
// #define XP_EMULATOR_CONFIG_UART_THR          (XP_EMULATOR_CONFIG_UART_BASE + 0x00U) // Transmit holding register
// #define XP_EMULATOR_CONFIG_UART_RBR          (XP_EMULATOR_CONFIG_UART_BASE + 0x00U) // Receive buffer register
// #define XP_EMULATOR_CONFIG_UART_LSR          (XP_EMULATOR_CONFIG_UART_BASE + 0x05U) // Line status register
// #define XP_EMULATOR_CONFIG_UART_LSR_TX_IDLE  (1 << 5)                               // Transmitter idle
// #define XP_EMULATOR_CONFIG_UART_LSR_RX_READY (1 << 0)                               // Receiver

// // define it for using riscv M extension
// #define XP_EMULATOR_USE_M_EXTENSION

// // define it for riscv64
// #if defined(__riscv)
//     #if __riscv_xlen == 64
//         #define XP_EMULATOR_USE_XLEN_64
//     #endif
// #endif

// #ifdef __cplusplus
//     #define XP_EMULATOR_EXTERN extern "C"
// #else
//     #define XP_EMULATOR_EXTERN
// #endif

// #define XP_EMULATOR_CONFIG_ENABLE_PRINT_OP
// #define XP_EMULATOR_CONFIG_ENABLE_PRINT_REGS
// #define XP_EMULATOR_CONFIG_ENABLE_LOGS
// #define XP_EMULATOR_CONFIG_ENABLE_SYSCALL_LOGS
// #define XP_EMULATOR_CONFIG_ENABLE_BUS_LOGS

// clang-format off

#define XP_EMULATOR_CONFIG_HMM_SIZE             (128 * 1024U)                                       // 128 KB
#define XP_EMULATOR_CONFIG_HMM_BASE             (0x8000000U)                                        // 128 MB
#define XP_EMULATOR_CONFIG_HMM_BOTTOM_STACK_PTR (XP_EMULATOR_CONFIG_HMM_BASE + 0x05U)               // READ STACK BOTTOM (0x1)
#define XP_EMULATOR_CONFIG_HMM_TOP_STACK_PTR    (XP_EMULATOR_CONFIG_HMM_BOTTOM_STACK_PTR + 0x05U)   // READ STACK TOP (0x1)
#define XP_EMULATOR_CONFIG_HMM_START_HEAP_PTR   (XP_EMULATOR_CONFIG_HMM_TOP_STACK_PTR + 0x05U)      // READ HEAP START (0x1)
#define XP_EMULATOR_CONFIG_HMM_END_HEAP_PTR     (XP_EMULATOR_CONFIG_HMM_START_HEAP_PTR + 0x05U)     // READ HEAP END (0x1)
#define XP_EMULATOR_CONFIG_HMM_FRAMEBUFFER_PTR  (XP_EMULATOR_CONFIG_HMM_END_HEAP_PTR + 0x05U)       // READ FRAMEBUFFER PTR START (0x1)
#define XP_EMULATOR_CONFIG_HMM_FRAMEMEMPOOL_PTR (XP_EMULATOR_CONFIG_HMM_FRAMEBUFFER_PTR + 0x05U)    // READ MEMORY POOL PTR (0x1)


#define XP_EMULATOR_CONFIG_HMM_MESH_COMM_BASE_PTR               (XP_EMULATOR_CONFIG_HMM_FRAMEMEMPOOL_PTR + 0x05U)    // BASE PTR FOR COMM WRITE
#define XP_EMULATOR_CONFIG_HMM_WRITE_NUM_SUBMESH_PTR            (XP_EMULATOR_CONFIG_HMM_MESH_COMM_BASE_PTR)     // WRITE NUM SUBMESHES (0x2)
#define XP_EMULATOR_CONFIG_HMM_WRITE_BEGIN_SUBMESH_STREAM_PTR   (XP_EMULATOR_CONFIG_HMM_MESH_COMM_BASE_PTR)     // WRITE STATE SUBMESH BEGIN (0x3)
#define XP_EMULATOR_CONFIG_HMM_WRITE_MODEL_MATRIX_PTR           (XP_EMULATOR_CONFIG_HMM_MESH_COMM_BASE_PTR)     // WRITE MODEL MATRIX (0x4)
#define XP_EMULATOR_CONFIG_HMM_WRITE_OBJECT_ID_PTR              (XP_EMULATOR_CONFIG_HMM_MESH_COMM_BASE_PTR)     // WRITE OBJECT ID (0x5)
#define XP_EMULATOR_CONFIG_HMM_WRITE_BEGIN_TRIANGLE_STREAM_PTR  (XP_EMULATOR_CONFIG_HMM_MESH_COMM_BASE_PTR)     // WRITE STATE BEGIN (0x6)
#define XP_EMULATOR_CONFIG_HMM_WRITE_NEXT_MESH_TRIANGLE_PTR     (XP_EMULATOR_CONFIG_HMM_MESH_COMM_BASE_PTR)     // WRITE NEXT TRIANGLE DATA (0x7)
#define XP_EMULATOR_CONFIG_HMM_WRITE_END_TRIANGLE_STREAM_PTR    (XP_EMULATOR_CONFIG_HMM_MESH_COMM_BASE_PTR)     // WRITE STATE END (0x8)
#define XP_EMULATOR_CONFIG_HMM_WRITE_END_SUBMESH_STREAM_PTR     (XP_EMULATOR_CONFIG_HMM_MESH_COMM_BASE_PTR)     // WRITE STATE SUBMESH BEGIN (0x9)

// clang-format on

// SMD
#define XP_EMULATOR_CONFIG_HMM_SMD_VALUE_CAMERA                  1
#define XP_EMULATOR_CONFIG_HMM_SMD_VALUE_NUM_TEXTURES            2
#define XP_EMULATOR_CONFIG_HMM_SMD_VALUE_NUM_MESH_BUFFER_OBJECTS 3
#define XP_EMULATOR_CONFIG_HMM_SMD_VALUE_NEXT_TRIANGLE           4

// #define XP_EMULATOR_CONFIG_MEMORY_SIZE       (7 * 1024U * 1024U) // 7 MB
#define XP_EMULATOR_CONFIG_MEMORY_FLASH_BASE (0x4000000U)  // 64 MB
#define XP_EMULATOR_CONFIG_MEMORY_RAM_BASE   (0x80000000U) // 2 GB
#define XP_EMULATOR_CONFIG_MEMORY_HEAP_BASE  (0x80400040U) // 2 GB + 4 MB

#define XP_EMULATOR_CONFIG_MEMORY_FLASH_SIZE (1 * 1024U * 1024U) // 1 MB
#define XP_EMULATOR_CONFIG_MEMORY_RAM_SIZE   (4 * 1024U * 1024U) // 4 MB
#define XP_EMULATOR_CONFIG_MEMORY_HEAP_SIZE  (2 * 1024U * 1024U) // 2 MB

#define XP_EMULATOR_CONFIG_MAIN_MEMORY_RANGE_START (XP_EMULATOR_CONFIG_MEMORY_FLASH_BASE)
#define XP_EMULATOR_CONFIG_MAIN_MEMORY_RANGE_END                                                                       \
    (XP_EMULATOR_CONFIG_MEMORY_HEAP_BASE + XP_EMULATOR_CONFIG_MEMORY_HEAP_SIZE)

// define it for using riscv M extension
#define XP_EMULATOR_USE_M_EXTENSION

// define it for riscv64
#if defined(__riscv)
    #if __riscv_xlen == 64
        #define XP_EMULATOR_USE_XLEN_64
    #endif
#endif

#ifdef __cplusplus
    #define XP_EMULATOR_EXTERN extern "C"
#else
    #define XP_EMULATOR_EXTERN
#endif

// SAME AS EMULATOR //
#define XP_EMULATOR_CONFIG_UART_BUFFER_SIZE  (4 * 1024U)                            // 4 KB
#define XP_EMULATOR_CONFIG_UART_BASE         (0x10000000U)                          // 256 MB
#define XP_EMULATOR_CONFIG_UART_THR          (XP_EMULATOR_CONFIG_UART_BASE + 0x00U) // Transmit holding register
#define XP_EMULATOR_CONFIG_UART_RBR          (XP_EMULATOR_CONFIG_UART_BASE + 0x00U) // Receive buffer register
#define XP_EMULATOR_CONFIG_UART_LSR          (XP_EMULATOR_CONFIG_UART_BASE + 0x05U) // Line status register
#define XP_EMULATOR_CONFIG_UART_LSR_TX_IDLE  (1 << 5)                               // Transmitter idle
#define XP_EMULATOR_CONFIG_UART_LSR_RX_READY (1 << 0)                               // Receiver
