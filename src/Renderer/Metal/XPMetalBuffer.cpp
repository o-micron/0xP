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

#include <Renderer/Metal/XPMetalBuffer.h>
#include <Renderer/Metal/XPMetalRenderer.h>

XPMetalBuffer::XPMetalBuffer(XPIRenderer* const renderer, size_t numBytes, XPERendererBufferStorageMode storageMode)
  : XPIRendererBuffer(renderer)
  , _renderer(static_cast<XPMetalRenderer*>(renderer))
  , _buffer(nullptr)
  , _numBytes(numBytes)
  , _storageMode(storageMode)
{
}

XPMetalBuffer::~XPMetalBuffer() { assert(_buffer == nullptr); }

void
XPMetalBuffer::allocate()
{
    MTL::ResourceOptions resourceOptions;
    switch (_storageMode) {
        case XPERendererBufferStorageMode::XPERendererBufferStorageMode_Shared:
            resourceOptions = MTL::ResourceStorageModeShared;
            break;
        case XPERendererBufferStorageMode::XPERendererBufferStorageMode_Private:
            resourceOptions = MTL::ResourceStorageModePrivate;
            break;
        case XPERendererBufferStorageMode::XPERendererBufferStorageMode_Managed:
            resourceOptions = MTL::ResourceStorageModeManaged;
            break;
        case XPERendererBufferStorageMode::XPERendererBufferStorageMode_Memoryless:
            resourceOptions = MTL::ResourceStorageModeMemoryless;
            break;
    };

    _buffer = _renderer->getDevice()->newBuffer(NS::UInteger(_numBytes), resourceOptions);
}

void
XPMetalBuffer::deallocate()
{
    _buffer->release();
    _buffer = nullptr;
}

void
XPMetalBuffer::synchronizeFromGPU(size_t offset, size_t numBytes)
{
}

void
XPMetalBuffer::synchronizeToGPU(size_t offset, size_t numBytes)
{
}

void*
XPMetalBuffer::getGPUBufferContents()
{
    return _buffer->contents();
}

void
XPMetalBuffer::resize(size_t numBytes)
{
    if (numBytes != _numBytes) {
        deallocate();
        _numBytes = numBytes;
        allocate();
    }
}

size_t
XPMetalBuffer::getNumBytes()
{
    return _numBytes;
}

XPERendererBufferStorageMode
XPMetalBuffer::getStorageMode()
{
    return _storageMode;
}
