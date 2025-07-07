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

#include <Physics/PhysX5/XPPhysX5Physics.h>

#include <Engine/XPRegistry.h>
#include <Renderer/Interface/XPIRenderer.h>
#include <SceneDescriptor/Attachments/XPCollider.h>
#include <SceneDescriptor/Attachments/XPRigidbody.h>
#include <SceneDescriptor/Attachments/XPTransform.h>
#include <SceneDescriptor/XPAttachments.h>
#include <SceneDescriptor/XPLayer.h>
#include <SceneDescriptor/XPScene.h>
#include <Utilities/XPLogger.h>

#include <thread>

XPPhysX5Physics::XPPhysX5Physics(XPRegistry* const registry)
  : XPIPhysics(registry)
  , _registry(registry)
{
}

XPPhysX5Physics::~XPPhysX5Physics() {}

void
XPPhysX5Physics::initialize()
{
}

void
XPPhysX5Physics::finalize()
{
}

void
XPPhysX5Physics::update()
{
}

void
XPPhysX5Physics::onSceneTraitsChanged()
{
}

void
XPPhysX5Physics::play()
{
}

void
XPPhysX5Physics::pause()
{
}

void
XPPhysX5Physics::fetchModelMatrix(void* rigidbody, void* shape, XPMat4<float>& model)
{
}

void
XPPhysX5Physics::createColliders(XPNode* node)
{
}

void
XPPhysX5Physics::destroyColliders(XPNode* node)
{
}

void
XPPhysX5Physics::createPlaneCollider(XPNode* node, uint32_t index)
{
}

void
XPPhysX5Physics::createBoxCollider(XPNode* node, uint32_t index)
{
}

void
XPPhysX5Physics::createSphereCollider(XPNode* node, uint32_t index)
{
}

void
XPPhysX5Physics::createCapsuleCollider(XPNode* node, uint32_t index)
{
}

void
XPPhysX5Physics::createCylinderCollider(XPNode* node, uint32_t index)
{
}

void
XPPhysX5Physics::createConvexMeshCollider(XPNode* node, uint32_t index)
{
}

void
XPPhysX5Physics::createTriMeshCollider(XPNode* node, uint32_t index)
{
}

XPRegistry*
XPPhysX5Physics::getRegistry()
{
    return _registry;
}

void
XPPhysX5Physics::setRigidbodyPositionAndRotation(XPNode* node)
{
}