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

#include <Physics/Interface/XPIPhysics.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Weverything"
// #include <PxPhysicsAPI.h>
#pragma clang diagnostic pop

#include <unordered_map>

class XPRegistry;

class XPPhysX4Physics final : public XPIPhysics
{
  public:
    XPPhysX4Physics(XPRegistry* const registry);
    ~XPPhysX4Physics() final;
    void        initialize() final;
    void        finalize() final;
    void        update() final;
    void        onSceneTraitsChanged() final;
    void        play() final;
    void        pause() final;
    void        fetchModelMatrix(void* rigidbody, void* shape, XPMat4<float>& model) final;
    void        createColliders(XPNode* node) final;
    void        destroyColliders(XPNode* node) final;
    void        createPlaneCollider(XPNode* node, uint32_t index) final;
    void        createBoxCollider(XPNode* node, uint32_t index) final;
    void        createSphereCollider(XPNode* node, uint32_t index) final;
    void        createCapsuleCollider(XPNode* node, uint32_t index) final;
    void        createCylinderCollider(XPNode* node, uint32_t index) final;
    void        createConvexMeshCollider(XPNode* node, uint32_t index) final;
    void        createTriMeshCollider(XPNode* node, uint32_t index) final;
    XPRegistry* getRegistry() final;
    void        setRigidbodyPositionAndRotation(XPNode* node) final;
    void        beginUploadMeshAssets() final;
    void        endUploadMeshAssets() final;
    void        uploadMeshAsset(XPMeshAsset* meshAsset) final;
    void        reUploadMeshAsset(XPMeshAsset* meshAsset) final;

  private:
    XPRegistry* const _registry = nullptr;
    bool              _isPlaying;
};
