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

#include "XPCollider.h"

#include <DataPipeline/XPMeshBuffer.h>
#include <Engine/XPRegistry.h>
#include <Physics/Interface/XPIPhysics.h>
#include <SceneDescriptor/Attachments/XPMeshRenderer.h>
#include <SceneDescriptor/XPNode.h>
#include <SceneDescriptor/XPScene.h>

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
onTraitAttached(Collider* collider)
{
    if (collider->owner->hasMeshRendererAttachment()) {
        // by default we assume that we're going for a box shape
        // in case you wanted some other shape (i.e TriMeshShape)
        // then after you call node->attachCollider() you set the colliderInfos
        MeshRenderer* mr = collider->owner->getMeshRenderer();
        collider->info.resize(mr->info.size());
        for (size_t i = 0; i < mr->info.size(); ++i) {
            if (mr->info[i].mesh.text == "Cube.001") {
                // here we assume that we need to overwrite shape to box for efficiency
                collider->info[i].shape                = XPEColliderShapeBox;
                const XPBoundingBox& boundingBox       = mr->info[i].meshBuffer->getObjects()[i].boundingBox;
                collider->info[i].parameters.boxWidth  = abs(boundingBox.maxPoint.x - boundingBox.minPoint.x) / 2.0f;
                collider->info[i].parameters.boxHeight = abs(boundingBox.maxPoint.y - boundingBox.minPoint.y) / 2.0f;
                collider->info[i].parameters.boxDepth  = abs(boundingBox.maxPoint.z - boundingBox.minPoint.z) / 2.0f;
            }
        }
    }
    if (collider->owner->hasRigidbodyAttachment()) { createPhysicsHandles(collider->owner); }
}

void
onTraitDettached(Collider* collider)
{
    if (collider->owner->hasRigidbodyAttachment()) { destroyPhysicsHandles(collider->owner); }
}

void
onRenderUI(Collider* collider, XPIUI* ui)
{
}

void
Collider::onChanged_info()
{
    if (owner->hasRigidbodyAttachment()) {
        XPRegistry* registry = owner->getAbsoluteScene()->getRegistry();
        registry->getPhysics()->updateColliderShape(owner);
    }
}