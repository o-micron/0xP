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

#include <Utilities/XPMacros.h>
#include <Utilities/XPMaths.h>

#include <Utilities/XPLogger.h>

#include <stdint.h>
#include <vector>

#if defined(__EMSCRIPTEN__)
typedef uint16_t MeshIndexType;
#else
typedef uint32_t MeshIndexType;
#endif

class XPMeshAsset;
class XPRegistry;
class XPNode;
class XP_PURE_ABSTRACT XPIPhysics
{
  public:
    XPIPhysics(XPRegistry* const registry) { XP_UNUSED(registry) }
    virtual ~XPIPhysics() {}
    virtual void        initialize()                                                         = 0;
    virtual void        finalize()                                                           = 0;
    virtual void        update()                                                             = 0;
    virtual void        onSceneTraitsChanged()                                               = 0;
    virtual void        play()                                                               = 0;
    virtual void        pause()                                                              = 0;
    virtual void        fetchModelMatrix(void* rigidbody, void* shape, XPMat4<float>& model) = 0;
    virtual void        createColliders(XPNode* node)                                        = 0;
    virtual void        destroyColliders(XPNode* node)                                       = 0;
    virtual void        createPlaneCollider(XPNode* node)                                    = 0;
    virtual void        createBoxCollider(XPNode* node)                                      = 0;
    virtual void        createSphereCollider(XPNode* node)                                   = 0;
    virtual void        createCapsuleCollider(XPNode* node)                                  = 0;
    virtual void        createCylinderCollider(XPNode* node)                                 = 0;
    virtual void        createConvexMeshCollider(XPNode* node)                               = 0;
    virtual void        createTriMeshCollider(XPNode* node)                                  = 0;
    virtual void        createCompoundCollider(XPNode* node)                                 = 0;
    virtual void        setRigidbodyPositionAndRotation(XPNode* node)                        = 0;
    virtual void        setRigidbodyLinearVelocity(XPNode* node)                             = 0;
    virtual void        setRigidbodyAngularVelocity(XPNode* node)                            = 0;
    virtual void        setRigidbodyStatic(XPNode* node)                                     = 0;
    virtual void        updateColliderShape(XPNode* node)                                    = 0;
    virtual XPRegistry* getRegistry()                                                        = 0;
    virtual void        beginUploadMeshAssets()                                              = 0;
    virtual void        endUploadMeshAssets()                                                = 0;
    virtual void        uploadMeshAsset(XPMeshAsset* meshAsset)                              = 0;
    virtual void        beginReUploadMeshAssets()                                            = 0;
    virtual void        endReUploadMeshAssets()                                              = 0;
    virtual void        reUploadMeshAsset(XPMeshAsset* meshAsset)                            = 0;
};
