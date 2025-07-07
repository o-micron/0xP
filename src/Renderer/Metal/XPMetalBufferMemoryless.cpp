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

#include <Renderer/Metal/XPMetalBufferMemoryless.h>

XPMetalMemorylessBuffer::XPMetalMemorylessBuffer(XPIRenderer* const renderer, size_t numBytes)
  : XPMetalBuffer(renderer, numBytes, XPERendererBufferStorageMode::XPERendererBufferStorageMode_Memoryless)
{
}

XPMetalMemorylessBuffer::~XPMetalMemorylessBuffer() {}

void
XPMetalMemorylessBuffer::fill(char value)
{
    XP_UNUSED(value)

    XP_UNIMPLEMENTED("Memoryless buffers are used only for textures");
}

void
XPMetalMemorylessBuffer::fill(char value, size_t offset, size_t numBytes)
{
    XP_UNUSED(value)
    XP_UNUSED(offset)
    XP_UNUSED(numBytes)

    XP_UNIMPLEMENTED("Memoryless buffers are used only for textures");
}

void
XPMetalMemorylessBuffer::write(const void* source)
{
    XP_UNUSED(source)

    XP_UNIMPLEMENTED("Memoryless buffers are used only for textures");
}

void
XPMetalMemorylessBuffer::write(const void* source, size_t sourceOffset, size_t destinationOffset, size_t numBytes)
{
    XP_UNUSED(source)
    XP_UNUSED(sourceOffset)
    XP_UNUSED(destinationOffset)
    XP_UNUSED(numBytes)

    XP_UNIMPLEMENTED("Memoryless buffers are used only for textures");
}

void
XPMetalMemorylessBuffer::read(void* destination)
{
    XP_UNUSED(destination)

    XP_UNIMPLEMENTED("Memoryless buffers are used only for textures");
}

void
XPMetalMemorylessBuffer::read(void* destination, size_t sourceOffset, size_t destinationOffset, size_t numBytes)
{
    XP_UNUSED(destination)
    XP_UNUSED(sourceOffset)
    XP_UNUSED(destinationOffset)
    XP_UNUSED(numBytes)

    XP_UNIMPLEMENTED("Memoryless buffers are used only for textures");
}
