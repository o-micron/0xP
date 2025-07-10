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

typedef struct XPEmulatorHostMappedMemory
{
    uint8_t  data[XP_EMULATOR_CONFIG_HMM_SIZE];
    uint32_t deviceBottomStack;
    uint32_t deviceTopStack;
    uint32_t deviceStartHeap;
    uint32_t deviceEndHeap;
    uint32_t deviceFramebuffer;
    uint32_t deviceFrameMemoryPool;
} XPEmulatorHostMappedMemory;

XP_EMULATOR_EXTERN void
xp_emulator_host_mapped_memory_initialize(XPEmulatorHostMappedMemory* memory);

XP_EMULATOR_EXTERN uint32_t
xp_emulator_host_mapped_memory_load(XPEmulatorHostMappedMemory* memory, uint32_t address, uint32_t size);

XP_EMULATOR_EXTERN void
xp_emulator_host_mapped_memory_store(XPEmulatorHostMappedMemory* memory,
                                     uint32_t                    address,
                                     uint32_t                    size,
                                     uint32_t                    value);

XP_EMULATOR_EXTERN void
xp_emulator_host_mapped_memory_finalize(XPEmulatorHostMappedMemory* memory);

// uint32_t numVertices
// uint32_t offsetVertices
// uint32_t offsetNormals
// uint32_t offsetTexcoords

// for all objects renderable in scene:
//      do frustum culling testing on each object in objects
//      if pass:
//          for each triangle in object mesh triangles, find which tile index it belong to
//          fill all tiles with triangle indices (not vertex indices, this is different)
//          for each tile:
//              clear buffer (or set flag as cleared)
//              if number of triangles >= 1:
//                  assign it to a new riscv core
//                  launch compute workloads on gpu to do rasterize (early depth buffer)
//                  for each framebuffer in color attachments and using early depth pre-calculated run through pixel
//                  shader copy back each tile to the main framebuffer texture done

// Nice things to add:
//      Counter of number of fragments in each triangle (if 0 means it was totally culled)
//      Time taken for each tile to finish executing (GPU Time in case host is GPU)
//      Visualize each step and each triangle rasterized in order if one can !