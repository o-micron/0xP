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

#include <Renderer/SW/XPSWLogger.h>

#include <stdint.h>

struct XPSWMemoryPool
{
    XPSWMemoryPool(const size_t numBytes = 0)
    {
        // allocate memory to be used during frame rasterization
        frameMemoryEnd   = numBytes == 0 ? 4 * 1024 * 1024 * sizeof(uint8_t) : numBytes;
        frameMemory      = static_cast<uint8_t*>(malloc(frameMemoryEnd));
        frameMemoryStart = 0;
        if (frameMemory == nullptr) { LOGV_CRITICAL("Could not allocate thread pool memory of {} bytes", numBytes); }
    }
    XPSWMemoryPool(const XPSWMemoryPool&) = delete;
    XPSWMemoryPool(XPSWMemoryPool&&)      = delete;
    ~XPSWMemoryPool()
    {
        if (frameMemory) {
            assert(frameMemoryStart == 0 && "Memory pool wasn't properly cleared out");
            free(frameMemory);
            frameMemory    = nullptr;
            frameMemoryEnd = 0;
        }
    }
    [[nodiscard]] uint8_t* pushFrameMemory(size_t numBytes)
    {
        if (frameMemoryStart + numBytes <= frameMemoryEnd) {
            uint8_t* ptr = &frameMemory[frameMemoryStart];
            // memset(ptr, 0, numBytes);
            frameMemoryStart += static_cast<int64_t>(numBytes);
            return ptr;
        }
        assert(false && "Memory full, you need to expand the maximum size of a frame memory");
        return nullptr;
    }
    void popFrameMemory(size_t numBytes)
    {
        assert((frameMemoryStart - static_cast<int64_t>(numBytes)) >= 0 &&
               "You need to push memory first then return it back via calling this function");
        frameMemoryStart -= static_cast<int64_t>(numBytes);
    }
    void checkClear()
    {
        if (frameMemoryStart != 0) {
            LOGV_CRITICAL("ThreadPoolMemory is not clear here, frameMemoryStart = {}, frameMemoryEnd = {}",
                          frameMemoryStart,
                          frameMemoryEnd);
        }
    }
    void popAllFrameMemory() { frameMemoryStart = 0; }
    void memsetZeros() { memset(frameMemory, 0, frameMemoryEnd); }

    uint8_t* frameMemory;
    int64_t  frameMemoryStart;
    int64_t  frameMemoryEnd;
};