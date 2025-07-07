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

#include <Physics/Empty/XPEmptyPhysics.h>

#include <SceneDescriptor/Attachments/XPCollider.h>
#include <SceneDescriptor/XPNode.h>

XPEmptyPhysics::XPEmptyPhysics(XPRegistry* const registry)
  : XPIPhysics(registry)
  , _registry(registry)
  , _isPlaying(false)
{
}

XPEmptyPhysics::~XPEmptyPhysics() {}

void
XPEmptyPhysics::initialize()
{
}

void
XPEmptyPhysics::finalize()
{
}

void
XPEmptyPhysics::update()
{
}

void
XPEmptyPhysics::onSceneTraitsChanged()
{
}

void
XPEmptyPhysics::play()
{
    _isPlaying = true;
}

void
XPEmptyPhysics::pause()
{
    _isPlaying = false;
}

void
XPEmptyPhysics::fetchModelMatrix(void* rigidbody, void* shape, XPMat4<float>& model)
{
}

void
XPEmptyPhysics::createColliders(XPNode* node)
{
}

void
XPEmptyPhysics::destroyColliders(XPNode* node)
{
}

void
XPEmptyPhysics::createPlaneCollider(XPNode* node)
{
}

void
XPEmptyPhysics::createBoxCollider(XPNode* node)
{
}

void
XPEmptyPhysics::createSphereCollider(XPNode* node)
{
}

void
XPEmptyPhysics::createCapsuleCollider(XPNode* node)
{
}

void
XPEmptyPhysics::createCylinderCollider(XPNode* node)
{
}

void
XPEmptyPhysics::createConvexMeshCollider(XPNode* node)
{
}

void
XPEmptyPhysics::createTriMeshCollider(XPNode* node)
{
}

void
XPEmptyPhysics::createCompoundCollider(XPNode* node)
{
}

void
XPEmptyPhysics::setRigidbodyPositionAndRotation(XPNode* node)
{
}

void
XPEmptyPhysics::setRigidbodyLinearVelocity(XPNode* node)
{
}

void
XPEmptyPhysics::setRigidbodyAngularVelocity(XPNode* node)
{
}

void
XPEmptyPhysics::setRigidbodyStatic(XPNode* node)
{
}

void
XPEmptyPhysics::updateColliderShape(XPNode* node)
{
}

XPRegistry*
XPEmptyPhysics::getRegistry()
{
    return _registry;
}

void
XPEmptyPhysics::beginUploadMeshAssets()
{
}

void
XPEmptyPhysics::endUploadMeshAssets()
{
}

void
XPEmptyPhysics::uploadMeshAsset(XPMeshAsset* meshAsset)
{
}

void
XPEmptyPhysics::beginReUploadMeshAssets()
{
}

void
XPEmptyPhysics::endReUploadMeshAssets()
{
}

void
XPEmptyPhysics::reUploadMeshAsset(XPMeshAsset* meshAsset)
{
}
