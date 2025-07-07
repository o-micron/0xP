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

#include "XPRendererGraph.h"

#include <SceneDescriptor/XPScene.h>

void
XPRendererGraphCommandBuffer::beginRecording()
{
    drawMeshes.clear();
}
void
XPRendererGraphCommandBuffer::submitMeshAssetForDraw(XPMeshAsset* meshAsset)
{
    drawMeshes.push_back(meshAsset);
}
void
XPRendererGraphCommandBuffer::endRecording()
{
}

void
XPRendererGraph::loadScene(XPScene& scene)
{
    auto& meshNodes = scene.getNodes(MeshRendererAttachmentDescriptor | TransformAttachmentDescriptor);

    // TODO: begin render pass

    // get graphics queue command buffer
    auto& graphicsQueue     = commandQueues[EXPRendererGraphCommandQueueType_Graphics];
    auto& graphicsCmdBuffer = graphicsQueue.commandBuffer;
    graphicsCmdBuffer.beginRecording();

    // for (const auto& meshNode : meshNodes) {
    //     Transform* transform  = meshNode->getTransform();
    //     auto       operations = static_cast<XPMat4<float>::ModelMatrixOperations>(
    //       XPMat4<float>::ModelMatrixOperation_Translation | XPMat4<float>::ModelMatrixOperation_Rotation |
    //       XPMat4<float>::ModelMatrixOperation_Scale);
    //     XPMat4<float>::buildModelMatrix(
    //       transform->modelMatrix, transform->location, transform->euler, transform->scale, operations);
    //     _gpuData->modelMatrices[_gpuData->numMeshNodes] = transform->modelMatrix;
    //     _gpuData->meshNodesIds[_gpuData->numMeshNodes]  = meshNode->getId();
    //     MeshRenderer* meshRenderer                      = meshNode->getMeshRenderer();
    //     for (const auto& subMesh : meshRenderer->info) {
    //         XPVulkanMeshObject* meshObject                       = _meshObjectMap[subMesh.mesh.text].get();
    //         _gpuData->meshObjects[_gpuData->numSubMeshes]        = meshObject;
    //         _gpuData->boundingBoxes[_gpuData->numSubMeshes]      = meshObject->boundingBox;
    //         _gpuData->perMeshNodeIndices[_gpuData->numSubMeshes] = _gpuData->numMeshNodes;
    //         ++_gpuData->numSubMeshes;
    //         ++_gpuData->numTotalDrawCalls;
    //         _gpuData->numTotalDrawCallsVertices += meshObject->numIndices;
    //     }
    //     ++_gpuData->numMeshNodes;
    // }

    // submit graphics queue command buffer
    graphicsCmdBuffer.endRecording();

    // end render pass
}