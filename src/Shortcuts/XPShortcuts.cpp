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

#include <Shortcuts/XPShortcuts.h>

#include <DataPipeline/XPDataPipelineStore.h>
#include <DataPipeline/XPFile.h>
#include <DataPipeline/XPMeshAsset.h>
#include <DataPipeline/XPMeshBuffer.h>
#include <Engine/XPEngine.h>
#include <Engine/XPRegistry.h>
#include <SceneDescriptor/XPLayer.h>
#include <SceneDescriptor/XPNode.h>
#include <SceneDescriptor/XPScene.h>
#include <SceneDescriptor/XPSceneDescriptorStore.h>

XP_EXTERN void
spawnCube(XPScene* scene, FreeCamera* camera, float speed)
{
    XPRegistry* registry = scene->getRegistry();
    registry->getEngine()->scheduleUITask([scene, camera, speed, registry]() {
        // attempt to load the constant mesh "CUBE" once per program
        const XPMeshBuffer* cubeMeshBuffer = [&]() -> const XPMeshBuffer* {
            auto optFile = registry->getDataPipelineStore()->getFile("CUBE", XPEFileResourceType::PreloadedMesh);
            if (optFile.has_value()) {
                XPFile* file = optFile.value();
                assert(file->getMeshAssets().size() == 1);
                auto it = file->getMeshAssets().begin();
                return (*it)->getMeshBuffer();
            }
            return nullptr;
        }();
        // ----------------------------------------------------------------------------

        XPVec3<float> ray;
        XPMat4<float>::projectRayFromClipSpaceCenterPoint(ray, camera->activeProperties.inverseViewMatrix.glm);
        ray.x *= speed;
        ray.y *= speed;
        ray.z *= speed;

        auto optNode = scene->getLayer("layer").value()->createNode();
        if (optNode.has_value()) {
            XPNode* newNode = optNode.value();

            newNode->attachTransform();
            Transform* tr = newNode->getTransform();
            tr->location  = camera->activeProperties.location;
            tr->euler     = camera->activeProperties.euler;
            tr->euler.z   = 0;
            tr->scale     = { 1.0f, 1.0f, 1.0f };
            XPMat4<float>::buildModelMatrix(tr->modelMatrix,
                                            tr->location,
                                            tr->euler,
                                            tr->scale,
                                            static_cast<XPMat4<float>::ModelMatrixOperations>(
                                              XPMat4<float>::ModelMatrixOperations::ModelMatrixOperation_Translation |
                                              XPMat4<float>::ModelMatrixOperations::ModelMatrixOperation_Rotation |
                                              XPMat4<float>::ModelMatrixOperations::ModelMatrixOperation_Scale));

            newNode->attachMeshRenderer();
            MeshRenderer* mr = newNode->getMeshRenderer();
            mr->isStatic     = false;
            mr->info.resize(1);
            mr->info[0].mesh.text        = "CUBE";
            mr->info[0].mesh.inputBuffer = mr->info[0].mesh.text;
            mr->info[0].meshBuffer       = cubeMeshBuffer;
            mr->info[0].polygonMode      = XPEMeshRendererPolygonModeFill;

            newNode->attachCollider();
            Collider* cl = newNode->getCollider();
            cl->info.resize(1);
            cl->info[0].shapeName.text        = "CUBE";
            cl->info[0].shapeName.inputBuffer = cl->info[0].shapeName.text;
            cl->info[0].shape                 = XPEColliderShapeBox;
            cl->info[0].meshBufferObjectIndex = 0;
            cl->info[0].parameters.boxWidth   = 1;
            cl->info[0].parameters.boxHeight  = 1;
            cl->info[0].parameters.boxDepth   = 1;
            cl->info[0].meshBuffer            = cubeMeshBuffer;

            newNode->attachRigidbody();
            Rigidbody* rb      = newNode->getRigidbody();
            rb->isStatic       = false;
            rb->linearVelocity = ray;
            rb->onChanged_linearVelocity();

            scene->addAttachmentChanges(XPEInteractionHasMeshRendererChanges | XPEInteractionHasTransformChanges |
                                          XPEInteractionHasColliderChanges | XPEInteractionHasRigidbodyChanges,
                                        false);
        }
    });
}