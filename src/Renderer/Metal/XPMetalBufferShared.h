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

#include <Renderer/Metal/XPMetalBuffer.h>

class XPMetalSharedBuffer final : public XPMetalBuffer
{
  public:
    XPMetalSharedBuffer(XPIRenderer* const renderer, size_t numBytes);
    ~XPMetalSharedBuffer() final;
    void fill(char value) final;
    void fill(char value, size_t offset, size_t numBytes) final;
    void write(const void* source) final;
    void write(const void* source, size_t sourceOffset, size_t destinationOffset, size_t numBytes) final;
    void read(void* destination) final;
    void read(void* destination, size_t sourceOffset, size_t destinationOffset, size_t numBytes) final;
};
