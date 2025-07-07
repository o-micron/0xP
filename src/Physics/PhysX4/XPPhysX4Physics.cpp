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

#include <Physics/PhysX4/XPPhysX4Physics.h>

#include <SceneDescriptor/Attachments/XPCollider.h>
#include <SceneDescriptor/XPNode.h>

XPPhysX4Physics::XPPhysX4Physics(XPRegistry* const registry)
  : XPIPhysics(registry)
  , _registry(registry)
  , _isPlaying(false)
{
}

XPPhysX4Physics::~XPPhysX4Physics() {}

void
XPPhysX4Physics::initialize()
{
}

void
XPPhysX4Physics::finalize()
{
}

void
XPPhysX4Physics::update()
{
}

void
XPPhysX4Physics::onSceneTraitsChanged()
{
}

void
XPPhysX4Physics::play()
{
    _isPlaying = true;
}

void
XPPhysX4Physics::pause()
{
    _isPlaying = false;
}

void
XPPhysX4Physics::fetchModelMatrix(void* rigidbody, void* shape, XPMat4<float>& model)
{
}

void
XPPhysX4Physics::createColliders(XPNode* node)
{
}

void
XPPhysX4Physics::destroyColliders(XPNode* node)
{
}

void
XPPhysX4Physics::createPlaneCollider(XPNode* node, uint32_t index)
{
}

void
XPPhysX4Physics::createBoxCollider(XPNode* node, uint32_t index)
{
}

void
XPPhysX4Physics::createSphereCollider(XPNode* node, uint32_t index)
{
}

void
XPPhysX4Physics::createCapsuleCollider(XPNode* node, uint32_t index)
{
}

void
XPPhysX4Physics::createCylinderCollider(XPNode* node, uint32_t index)
{
}

void
XPPhysX4Physics::createConvexMeshCollider(XPNode* node, uint32_t index)
{
}

void
XPPhysX4Physics::createTriMeshCollider(XPNode* node, uint32_t index)
{
}

XPRegistry*
XPPhysX4Physics::getRegistry()
{
    return _registry;
}

void
XPPhysX4Physics::setRigidbodyPositionAndRotation(XPNode* node)
{
}