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

#include <Utilities/XPMacros.h>
#include <Utilities/XPPlatforms.h>

#include <stddef.h>

class XPIRenderer;

// For Metal, check the docs:
// https://developer.apple.com/documentation/metal/resource_fundamentals/choosing_a_resource_storage_mode_for_apple_gpus
enum class XPERendererBufferStorageMode
{
    // Accessed from both CPU and GPU, frequently updated from CPU side.
    XPERendererBufferStorageMode_Shared,
    // Accessed exclusively from GPU, would need temporary shared buffer to copy data from CPU.
    XPERendererBufferStorageMode_Private,
    // Not frequently changed from CPU side, and mostly changed in ranges, requires explicit synchronizing after changes
    XPERendererBufferStorageMode_Managed,
    // Accessed exclusively from GPU and temporary, tile memory, faster and higher bandwidth (for textures only)
    XPERendererBufferStorageMode_Memoryless
};

class XP_PURE_ABSTRACT XPIRendererBuffer
{
  public:
    explicit XPIRendererBuffer(XPIRenderer* renderer) { XP_UNUSED(renderer) }
    virtual ~XPIRendererBuffer() = default;
    // allocates memory for the buffer
    virtual void allocate() = 0;
    // deallocates memory used by the buffer
    virtual void deallocate() = 0;
    // used to sync CPU from GPU memory in case the storage mode is managed
    virtual void synchronizeFromGPU(size_t offset, size_t numBytes) = 0;
    // used to sync GPU from CPU memory in case the storage mode is managed
    virtual void synchronizeToGPU(size_t offset, size_t numBytes) = 0;
    // returns a valid pointer to the buffer in the gpu memory that can be read from and written to from CPU side
    virtual void* getGPUBufferContents() = 0;
    // fill the whole buffer with some value
    virtual void fill(char value) = 0;
    // fill part of the buffer with some value
    virtual void fill(char value, size_t offset, size_t numBytes) = 0;
    // copies the whole buffer from RAM to VRAM
    virtual void write(const void* source) = 0;
    // copies part of the buffer from RAM to VRAM with the given offsets and numBytes options
    virtual void write(const void* source, size_t sourceOffset, size_t destinationOffset, size_t numBytes) = 0;
    // copies the whole buffer from VRAM to RAM
    virtual void read(void* destination) = 0;
    // copies part of the buffer from VRAM to RAM with the given offsets and numBytes options
    virtual void read(void* destination, size_t sourceOffset, size_t destinationOffset, size_t numBytes) = 0;
    // resizes the buffer and potentially deallocates and allocates new buffer with new size
    virtual void resize(size_t numBytes) = 0;
    // returns the size of the buffer in bytes
    virtual size_t getNumBytes() = 0;
    // returns the storage mode
    virtual XPERendererBufferStorageMode getStorageMode() = 0;
};
