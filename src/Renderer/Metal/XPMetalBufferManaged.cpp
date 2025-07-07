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

#include <Renderer/Metal/XPMetalBufferManaged.h>
#include <Renderer/Metal/XPMetalRenderer.h>
#include <Utilities/XPMemory.h>

XPMetalManagedBuffer::XPMetalManagedBuffer(XPIRenderer* const renderer, size_t numBytes)
  : XPMetalBuffer(renderer, numBytes, XPERendererBufferStorageMode::XPERendererBufferStorageMode_Managed)
  , _alignedAllocatedMemory(nullptr)
  , _alignedMemorySize(0)
{
}

XPMetalManagedBuffer::~XPMetalManagedBuffer() {}

void
XPMetalManagedBuffer::allocate()
{
    XPMetalBuffer::allocate();

    _alignedMemorySize = _numBytes;
    size_t pageSize    = XPGetMemoryPageSize();
    while (_alignedMemorySize % pageSize != 0) { ++_alignedMemorySize; }
    _alignedAllocatedMemory = XPAllocateAlignedVirtualMemory(_alignedMemorySize);
}

void
XPMetalManagedBuffer::deallocate()
{
    XPDeallocateAlignedVirtualMemory(_alignedAllocatedMemory, _alignedMemorySize);
    XPMetalBuffer::deallocate();
}

void
XPMetalManagedBuffer::synchronizeFromGPU(size_t offset, size_t numBytes)
{
    memcpy(static_cast<char*>(_alignedAllocatedMemory) + offset,
           static_cast<char*>(getGPUBufferContents()) + offset,
           numBytes);
}

void
XPMetalManagedBuffer::synchronizeToGPU(size_t offset, size_t numBytes)
{
    memcpy(static_cast<char*>(getGPUBufferContents()) + offset,
           static_cast<char*>(_alignedAllocatedMemory) + offset,
           numBytes);
    _buffer->didModifyRange(NS::Range(offset, numBytes));
}

void
XPMetalManagedBuffer::fill(char value)
{
    memset(_alignedAllocatedMemory, value, _numBytes);
}

void
XPMetalManagedBuffer::fill(char value, size_t offset, size_t numBytes)
{
    memset(static_cast<char*>(_alignedAllocatedMemory) + offset, value, numBytes);
}

void
XPMetalManagedBuffer::write(const void* source)
{
    memcpy(_alignedAllocatedMemory, source, _numBytes);
}

void
XPMetalManagedBuffer::write(const void* source, size_t sourceOffset, size_t destinationOffset, size_t numBytes)
{
    memcpy(static_cast<char*>(_alignedAllocatedMemory) + destinationOffset,
           static_cast<const char*>(source) + sourceOffset,
           numBytes);
}

void
XPMetalManagedBuffer::read(void* destination)
{
    memcpy(destination, _alignedAllocatedMemory, _numBytes);
}

void
XPMetalManagedBuffer::read(void* destination, size_t sourceOffset, size_t destinationOffset, size_t numBytes)
{
    memcpy(static_cast<char*>(destination) + destinationOffset,
           static_cast<char*>(_alignedAllocatedMemory) + sourceOffset,
           numBytes);
}
