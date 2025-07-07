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

#include <Renderer/Metal/XPMetalBufferShared.h>

XPMetalSharedBuffer::XPMetalSharedBuffer(XPIRenderer* const renderer, size_t numBytes)
  : XPMetalBuffer(renderer, numBytes, XPERendererBufferStorageMode::XPERendererBufferStorageMode_Shared)
{
}

XPMetalSharedBuffer::~XPMetalSharedBuffer() {}

void
XPMetalSharedBuffer::fill(char value)
{
    memset(getGPUBufferContents(), value, _numBytes);
}

void
XPMetalSharedBuffer::fill(char value, size_t offset, size_t numBytes)
{
    memset(static_cast<char*>(getGPUBufferContents()) + offset, value, numBytes);
}

void
XPMetalSharedBuffer::write(const void* source)
{
    memcpy(getGPUBufferContents(), source, _numBytes);
}

void
XPMetalSharedBuffer::write(const void* source, size_t sourceOffset, size_t destinationOffset, size_t numBytes)
{
    memcpy(static_cast<char*>(getGPUBufferContents()) + destinationOffset,
           static_cast<const char*>(source) + sourceOffset,
           numBytes);
}

void
XPMetalSharedBuffer::read(void* destination)
{
    memcpy(destination, getGPUBufferContents(), _numBytes);
}

void
XPMetalSharedBuffer::read(void* destination, size_t sourceOffset, size_t destinationOffset, size_t numBytes)
{
    memcpy(static_cast<char*>(destination) + destinationOffset,
           static_cast<char*>(getGPUBufferContents()) + sourceOffset,
           numBytes);
}
