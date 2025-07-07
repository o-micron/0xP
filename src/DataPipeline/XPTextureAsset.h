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

#include <Utilities/XPPlatforms.h>

#include <DataPipeline/XPTextureBuffer.h>

#include <string>

class XPFile;

/// @brief Binds between texture buffers streamed from disk and their gpu references
class XPTextureAsset
{
  public:
    XPTextureAsset(XPFile* const file, uint32_t id)
      : _file(file)
      , _id(id)
      , _textureBuffer(nullptr)
      , _gpuRef(nullptr)
    {
    }
    ~XPTextureAsset() {}
    XPFile*          getFile() const { return _file; }
    uint32_t         getId() const { return _id; }
    XPTextureBuffer* getTextureBuffer() const { return _textureBuffer; }
    void*            getGPURef() const { return _gpuRef; }
    void             setTextureBuffer(XPTextureBuffer* textureBuffer) { _textureBuffer = textureBuffer; }
    void             setGPURef(void* gpuRef) { _gpuRef = gpuRef; }

  private:
    XPFile* const    _file;
    const uint32_t   _id;
    XPTextureBuffer* _textureBuffer;
    void*            _gpuRef;
};
