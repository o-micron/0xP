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

#include <Renderer/Metal/XPMetalBufferPrivate.h>
#include <Renderer/Metal/XPMetalRenderer.h>

XPMetalPrivateBuffer::XPMetalPrivateBuffer(XPIRenderer* const renderer, size_t numBytes)
  : XPMetalBuffer(renderer, numBytes, XPERendererBufferStorageMode::XPERendererBufferStorageMode_Private)
{
}

XPMetalPrivateBuffer::~XPMetalPrivateBuffer() {}

void
XPMetalPrivateBuffer::fill(char value)
{
    MTL::Buffer* temporaryBuffer = createTemporarySharedBuffer();
    memset(temporaryBuffer->contents(), value, _numBytes);
    copyTemporarySharedBufferToPrivateBuffer(temporaryBuffer, 0, _numBytes);
    temporaryBuffer->release();
}

void
XPMetalPrivateBuffer::fill(char value, size_t offset, size_t numBytes)
{
    MTL::Buffer* temporaryBuffer = createTemporarySharedBuffer();
    memset(static_cast<char*>(temporaryBuffer->contents()) + offset, value, numBytes);
    copyTemporarySharedBufferToPrivateBuffer(temporaryBuffer, offset, numBytes);
    temporaryBuffer->release();
}

void
XPMetalPrivateBuffer::write(const void* source)
{
    MTL::Buffer* temporaryBuffer = createTemporarySharedBuffer();
    memcpy(temporaryBuffer->contents(), source, _numBytes);
    copyTemporarySharedBufferToPrivateBuffer(temporaryBuffer, 0, _numBytes);
    temporaryBuffer->release();
}

void
XPMetalPrivateBuffer::write(const void* source, size_t sourceOffset, size_t destinationOffset, size_t numBytes)
{
    MTL::Buffer* temporaryBuffer = createTemporarySharedBuffer();
    memcpy(static_cast<char*>(temporaryBuffer->contents()) + destinationOffset,
           static_cast<const char*>(source) + sourceOffset,
           numBytes);
    copyTemporarySharedBufferToPrivateBuffer(temporaryBuffer, destinationOffset, numBytes);
    temporaryBuffer->release();
}

void
XPMetalPrivateBuffer::read(void* destination)
{
    memcpy(destination, getGPUBufferContents(), _numBytes);
}

void
XPMetalPrivateBuffer::read(void* destination, size_t sourceOffset, size_t destinationOffset, size_t numBytes)
{
    memcpy(static_cast<char*>(destination) + destinationOffset,
           static_cast<char*>(getGPUBufferContents()) + sourceOffset,
           numBytes);
}

MTL::Buffer*
XPMetalPrivateBuffer::createTemporarySharedBuffer()
{
    return _renderer->getDevice()->newBuffer(NS::UInteger(_numBytes), MTL::ResourceStorageModeShared);
}

void
XPMetalPrivateBuffer::copyTemporarySharedBufferToPrivateBuffer(MTL::Buffer* temporaryShadowBuffer,
                                                               size_t       offset,
                                                               size_t       numBytes)
{
    assert(temporaryShadowBuffer->length() == _buffer->length());

    MTL::CommandBuffer*      commandBuffer      = _renderer->getIOCommandQueue()->commandBuffer();
    MTL::BlitCommandEncoder* blitCommandEncoder = commandBuffer->blitCommandEncoder();
    blitCommandEncoder->copyFromBuffer(
      temporaryShadowBuffer, NS::UInteger(offset), _buffer, NS::UInteger(offset), numBytes);
    blitCommandEncoder->endEncoding();
    commandBuffer->commit();
    commandBuffer->waitUntilCompleted();
}

void
XPMetalPrivateBuffer::copyPrivateToTemporarySharedBuffer(MTL::Buffer* temporaryShadowBuffer,
                                                         size_t       offset,
                                                         size_t       numBytes)
{
    assert(temporaryShadowBuffer->length() == _buffer->length());

    MTL::CommandBuffer*      commandBuffer      = _renderer->getIOCommandQueue()->commandBuffer();
    MTL::BlitCommandEncoder* blitCommandEncoder = commandBuffer->blitCommandEncoder();
    blitCommandEncoder->copyFromBuffer(
      _buffer, NS::UInteger(offset), temporaryShadowBuffer, NS::UInteger(offset), numBytes);
    blitCommandEncoder->endEncoding();
    commandBuffer->commit();
    commandBuffer->waitUntilCompleted();
}
