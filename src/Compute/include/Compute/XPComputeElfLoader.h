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

#include <Compute/shaders/shared/config.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MAX_SEGMENTS 3

typedef struct
{
    uint64_t vaddr;
    uint64_t size;
    uint32_t flags;
    uint8_t* data;
} MemorySegment;

typedef struct
{
    char*    filename;
    uint8_t* file_data;
    size_t   file_size;

    MemorySegment segments[MAX_SEGMENTS];
    int           num_segments;

    bool     is_64bit;
    bool     is_little_endian;
    uint64_t entry_point;
} RiscvElfLoader;

XP_XPU_EXTERN
RiscvElfLoader*
xp_xpu_elf_loader_load(const char* program_binary);

XP_XPU_EXTERN void
xp_xpu_elf_loader_unload(RiscvElfLoader* loader);