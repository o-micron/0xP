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

#include <DataPipeline/XPMaterialBuffer.h>

#include <string>

/// @brief Binds between material buffers streamed from disk and their gpu references
class XPMaterialAsset
{
  public:
    XPMaterialAsset(const std::string& name, uint32_t id)
      : _name(name)
      , _id(id)
      , _materialBuffer(nullptr)
      , _gpuRef(nullptr)
    {
    }
    ~XPMaterialAsset() {}
    const std::string& getName() const { return _name; }
    uint32_t           getId() const { return _id; }
    XPMaterialBuffer*  getMaterialBuffer() const { return _materialBuffer; }
    void*              getGPURef() const { return _gpuRef; }
    void               setMaterialBuffer(XPMaterialBuffer* materialBuffer) { _materialBuffer = materialBuffer; }
    void               setGPURef(void* gpuRef) { _gpuRef = gpuRef; }

  private:
    const std::string _name;
    const uint32_t    _id;
    XPMaterialBuffer* _materialBuffer;
    void*             _gpuRef;
};
