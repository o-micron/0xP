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

class XPEmptyPhysics final : public XPIPhysics
{
  public:
    XPEmptyPhysics(XPRegistry* const registry);
    ~XPEmptyPhysics() final;
    void        initialize() final;
    void        finalize() final;
    void        update() final;
    void        onSceneTraitsChanged() final;
    void        play() final;
    void        pause() final;
    void        fetchModelMatrix(void* rigidbody, void* shape, XPMat4<float>& model) final;
    void        createColliders(XPNode* node) final;
    void        destroyColliders(XPNode* node) final;
    void        createPlaneCollider(XPNode* node) final;
    void        createBoxCollider(XPNode* node) final;
    void        createSphereCollider(XPNode* node) final;
    void        createCapsuleCollider(XPNode* node) final;
    void        createCylinderCollider(XPNode* node) final;
    void        createConvexMeshCollider(XPNode* node) final;
    void        createTriMeshCollider(XPNode* node) final;
    void        createCompoundCollider(XPNode* node) final;
    void        setRigidbodyPositionAndRotation(XPNode* node) final;
    void        setRigidbodyLinearVelocity(XPNode* node) final;
    void        setRigidbodyAngularVelocity(XPNode* node) final;
    void        setRigidbodyStatic(XPNode* node) final;
    void        updateColliderShape(XPNode* node) final;
    XPRegistry* getRegistry() final;
    void        beginUploadMeshAssets() final;
    void        endUploadMeshAssets() final;
    void        uploadMeshAsset(XPMeshAsset* meshAsset) final;
    void        beginReUploadMeshAssets() final;
    void        endReUploadMeshAssets() final;
    void        reUploadMeshAsset(XPMeshAsset* meshAsset) final;

  private:
    XPRegistry* const _registry = nullptr;
    bool              _isPlaying;
};