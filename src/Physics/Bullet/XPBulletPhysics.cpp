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

#include <Physics/Bullet/XPBulletPhysics.h>

#include <SceneDescriptor/Attachments/XPCollider.h>
#include <SceneDescriptor/XPNode.h>

XPBulletPhysics::XPBulletPhysics(XPRegistry* const registry)
  : XPIPhysics(registry)
  , _registry(registry)
  , _isPlaying(false)
{
}

XPBulletPhysics::~XPBulletPhysics() {}

void
XPBulletPhysics::initialize()
{
}

void
XPBulletPhysics::finalize()
{
}

void
XPBulletPhysics::update()
{
}

void
XPBulletPhysics::onSceneTraitsChanged()
{
}

void
XPBulletPhysics::play()
{
    _isPlaying = true;
}

void
XPBulletPhysics::pause()
{
    _isPlaying = false;
}

void
XPBulletPhysics::fetchModelMatrix(void* rigidbody, void* shape, XPMat4<float>& model)
{
}

void
XPBulletPhysics::createColliders(XPNode* node)
{
}

void
XPBulletPhysics::destroyColliders(XPNode* node)
{
}

void
XPBulletPhysics::createPlaneCollider(XPNode* node, uint32_t index)
{
}

void
XPBulletPhysics::createBoxCollider(XPNode* node, uint32_t index)
{
}

void
XPBulletPhysics::createSphereCollider(XPNode* node, uint32_t index)
{
}

void
XPBulletPhysics::createCapsuleCollider(XPNode* node, uint32_t index)
{
}

void
XPBulletPhysics::createCylinderCollider(XPNode* node, uint32_t index)
{
}

void
XPBulletPhysics::createConvexMeshCollider(XPNode* node, uint32_t index)
{
}

void
XPBulletPhysics::createTriMeshCollider(XPNode* node, uint32_t index)
{
}

XPRegistry*
XPBulletPhysics::getRegistry()
{
    return _registry;
}

void
XPBulletPhysics::setRigidbodyPositionAndRotation(XPNode* node)
{
}