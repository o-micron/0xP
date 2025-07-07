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

#include <iostream>
#include <list>

class XPFile;
class XPMeshBuffer;
class XPTextureBuffer;
class XPMaterialBuffer;

static int64_t SceneAssetRefCounter = 0;

class XPSceneAsset
{
  public:
    XPSceneAsset(XPFile* const file, uint32_t id)
      : _file(file)
      , _id(id)
    {
        ++SceneAssetRefCounter;
    }
    ~XPSceneAsset()
    {
        --SceneAssetRefCounter;
        std::cout << "SceneAssetRefCounter: " << SceneAssetRefCounter << "\n";
    }
    XPFile*                             getFile() const { return _file; }
    uint32_t                            getId() const { return _id; }
    const std::list<XPMeshBuffer*>&     getMeshBuffers() const { return _meshBuffers; }
    const std::list<XPTextureBuffer*>&  getTextureBuffers() const { return _textureBuffers; }
    const std::list<XPMaterialBuffer*>& getMaterialBuffers() const { return _materialBuffers; }

  private:
    XPFile* const                _file;
    const uint32_t               _id;
    std::list<XPMeshBuffer*>     _meshBuffers;
    std::list<XPTextureBuffer*>  _textureBuffers;
    std::list<XPMaterialBuffer*> _materialBuffers;
};