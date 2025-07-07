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

#include <DataPipeline/XPMeshBuffer.h>

#include <iostream>
#include <stdint.h>
#include <string>

class XPFile;

static int64_t MeshAssetRefCounter = 0;

/// @brief Binds between mesh buffers streamed from disk and their gpu references
class XPMeshAsset
{
  public:
    XPMeshAsset(XPFile* const file, uint32_t id)
      : _file(file)
      , _id(id)
      , _meshBuffer(nullptr)
      , _gpuRef(nullptr)
    {
        ++MeshAssetRefCounter;
    }
    ~XPMeshAsset()
    {
        --MeshAssetRefCounter;
        std::cout << "MeshAssetRefCounter: " << MeshAssetRefCounter << "\n";
    }
    XPFile*       getFile() const { return _file; }
    uint32_t      getId() const { return _id; }
    XPMeshBuffer* getMeshBuffer() const { return _meshBuffer; }
    void*         getGPURef() const { return _gpuRef; }
    void          setMeshBuffer(XPMeshBuffer* meshBuffer) { _meshBuffer = meshBuffer; }
    void          setGPURef(void* gpuRef) { _gpuRef = gpuRef; }

  private:
    XPFile* const  _file;
    const uint32_t _id;
    XPMeshBuffer*  _meshBuffer;
    void*          _gpuRef;
};
