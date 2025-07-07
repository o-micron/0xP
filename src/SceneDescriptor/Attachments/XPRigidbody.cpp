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

#include "XPRigidbody.h"

#include <Engine/XPRegistry.h>
#include <Physics/Interface/XPIPhysics.h>
#include <SceneDescriptor/XPNode.h>
#include <SceneDescriptor/XPScene.h>

void
Rigidbody::onChanged_linearVelocity()
{
    XPRegistry* registry = owner->getAbsoluteScene()->getRegistry();
    registry->getPhysics()->setRigidbodyLinearVelocity(owner);
}

void
Rigidbody::onChanged_linearDamping()
{
}

void
Rigidbody::onChanged_angularVelocity()
{
    XPRegistry* registry = owner->getAbsoluteScene()->getRegistry();
    registry->getPhysics()->setRigidbodyAngularVelocity(owner);
}

void
Rigidbody::onChanged_angularDamping()
{
}

void
Rigidbody::onChanged_mass()
{
}

void
Rigidbody::onChanged_isLinearXLocked()
{
}

void
Rigidbody::onChanged_isLinearYLocked()
{
}

void
Rigidbody::onChanged_isLinearZLocked()
{
}

void
Rigidbody::onChanged_isAngularXLocked()
{
}

void
Rigidbody::onChanged_isAngularYLocked()
{
}

void
Rigidbody::onChanged_isAngularZLocked()
{
}

void
Rigidbody::onChanged_isStatic()
{
    XPRegistry* registry = owner->getAbsoluteScene()->getRegistry();
    registry->getPhysics()->updateColliderShape(owner);
}

void
Rigidbody::onChanged_isTrigger()
{
}

static void
createPhysicsHandles(XPNode* node)
{
    XPRegistry* registry = node->getAbsoluteScene()->getRegistry();
    registry->getPhysics()->createColliders(node);
}

static void
destroyPhysicsHandles(XPNode* node)
{
    XPRegistry* registry = node->getAbsoluteScene()->getRegistry();
    registry->getPhysics()->destroyColliders(node);
}

void
onTraitAttached(Rigidbody* rigidbody)
{
    if (rigidbody->owner->hasMeshRendererAttachment()) {
        MeshRenderer* mr    = rigidbody->owner->getMeshRenderer();
        rigidbody->isStatic = mr->isStatic;
    }
    if (rigidbody->owner->hasColliderAttachment()) { createPhysicsHandles(rigidbody->owner); }
}

void
onTraitDettached(Rigidbody* rigidbody)
{
    if (rigidbody->owner->hasColliderAttachment()) { destroyPhysicsHandles(rigidbody->owner); }
}

void
onRenderUI(Rigidbody* rigidbody, XPIUI* ui)
{
}