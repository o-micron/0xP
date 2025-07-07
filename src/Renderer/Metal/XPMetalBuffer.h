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

#include <Renderer/Interface/XPIRendererBuffer.h>

#include <Renderer/Metal/metal_cpp.h>

class XPMetalRenderer;

class XPMetalBuffer : public XPIRendererBuffer
{
  public:
    XPMetalBuffer(XPIRenderer* const renderer, size_t numBytes, XPERendererBufferStorageMode storageMode);
    ~XPMetalBuffer();
    void                         allocate() override;
    void                         deallocate() override;
    void                         synchronizeFromGPU(size_t offset, size_t numBytes) override;
    void                         synchronizeToGPU(size_t offset, size_t numBytes) override;
    void*                        getGPUBufferContents() final;
    void                         resize(size_t numBytes) final;
    size_t                       getNumBytes() final;
    XPERendererBufferStorageMode getStorageMode() final;

  protected:
    XPMetalRenderer*             _renderer;
    MTL::Buffer*                 _buffer;
    size_t                       _numBytes;
    XPERendererBufferStorageMode _storageMode;
};
