///// --------------------------------------------------------------------------------------
///// Copyright 2025 Omar Sherif Fathy
/////
///// Licensed under the Apache License, Version 2.0 (the "License");
///// you may not use this file except in compliance with the License.
///// You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
/////
///// Unless required by applicable law or agreed to in writing, software
///// distributed under the License is distributed on an "AS IS" BASIS,
///// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
///// See the License for the specific language governing permissions and
///// limitations under the License.
///// --------------------------------------------------------------------------------------
//
// #include <DataPipeline/XPMeshBuffer.h>
// #include <DataPipeline/XPShaderBuffer.h>
// #include <DataPipeline/XPTextureBuffer.h>
// #include <Engine/XPConsole.h>
// #include <Engine/XPEngine.h>
// #include <Renderer/Metal/XPMetalCulling.h>
// #include <Renderer/Metal/XPMetalRenderer.h>
// #include <SceneDescriptor/XPScene.h>
// #include <Utilities/XPMemory.h>
//
// #include <cstring>
//
// void
// memset64(uint64_t* ptr, size_t count)
//{
//    uint64_t* cptr = ptr;
//    for (size_t i = 0; i < count; ++i) {
//        *cptr = 1;
//        ++cptr;
//    }
//}
//
// XPMetalCulling::XPMetalCulling(XPMetalRenderer* const renderer)
//  : _renderer(renderer)
//  , _frustumBuffer(XPBitArray(XPMaxNumObjectsPerScene, false))
//  , _visibilityBufferReadIndex(0)
//  , _visibilityBufferWriteIndex(0)
//{
//}
//
// XPMetalCulling::~XPMetalCulling() {}
//
// void
// XPMetalCulling::initialize(MTL::Device* device)
//{
//    _frustumBuffer.resize(XPMaxNumObjectsPerScene, false);
//    for (size_t i = 0; i < XPNumPerFrameBuffers; ++i) {
//        _indirectVisibilityIndices[i].resize(XPMaxNumMeshesPerScene, -1);
//        _visibilityBuffer[i] =
//          device->newBuffer(XPMaxNumVisibilityBufferGeometries * sizeof(uint64_t), MTL::ResourceStorageModeShared);
//        _visibilityBuffer[i]->setLabel(NS_STRING_FROM_CSTRING("visibilitybuffer"));
//    }
//    _readFromVisibilityResultBuffer = _visibilityBuffer[_visibilityBufferReadIndex];
//    _writeToVisibilityResultBuffer  = _visibilityBuffer[_visibilityBufferWriteIndex];
//}
//
// void
// XPMetalCulling::finalize()
//{
//    for (size_t i = 0; i < XPNumPerFrameBuffers; ++i) { _visibilityBuffer[i]->release(); }
//}
//
//// Doing frustum and visibility culling only for now.
////
//// Debug using console variables:
////  - "r.bb <1|0>"
////  - "r.freeze <1|0>"
////  - "r.frustumCulling <1|0>"
////  - "r.visibilityCulling <1|0>"
////
//// Frustum culling is mainly depending on the frustum built from camera.
//// TODO: Visibility buffer works best if we do sort the objects from nearest to farthest from the camera.
// void
// XPMetalCulling::update(const XPFrustum&      frustum,
//                        XPMetalGPUData&       gpuData,
//                        XPFrameDataPerObject& frameDataPerObject,
//                        size_t                frameDataIndex,
//                        XPMetalOCBuffer&      ocBuffer,
//                        MTL::CommandBuffer*   commandBuffer,
//                        XPScene&              scene,
//                        const packed_uint2&   dimensions,
//                        const MTL::Viewport&  viewport)
//{
//     auto console = _renderer->getRegistry()->getEngine()->getConsole();
//
//     _frameDataIndex = frameDataIndex;
//     memcpy(&_frameDataPerObject[_frameDataIndex], &frameDataPerObject, sizeof(XPFrameDataPerObject));
//
//     if (console->getVariableValue<bool>("r.frustumCulling")) {
//         doFrustumCulling(frustum, gpuData, ocBuffer, commandBuffer, scene, dimensions, viewport);
//     } else {
//         //        _frustumBuffer.setAll();
//     }
//
//     if (console->getVariableValue<bool>("r.visibilityCulling")) {
//         doVisibilityCulling(gpuData, ocBuffer, commandBuffer, scene, dimensions, viewport);
//     } else {
//         // memset(_writeToVisibilityResultBuffer->contents(), 1, _writeToVisibilityResultBuffer->length());
//         //        uint64_t* ptr = reinterpret_cast<uint64_t*>(_writeToVisibilityResultBuffer->contents());
//         //        memset64(ptr, XPMaxNumObjectsPerScene);
//     }
// }
//
// void
// XPMetalCulling::doFrustumCulling(const XPFrustum&     frustum,
//                                  XPMetalGPUData&      gpuData,
//                                  XPMetalOCBuffer&     ocBuffer,
//                                  MTL::CommandBuffer*  commandBuffer,
//                                  XPScene&             scene,
//                                  const packed_uint2&  dimensions,
//                                  const MTL::Viewport& viewport)
//{
//     XP_UNUSED(ocBuffer)
//     XP_UNUSED(commandBuffer)
//     XP_UNUSED(scene)
//     XP_UNUSED(dimensions)
//     XP_UNUSED(viewport)
//
//     _frustumBuffer.clearAll();
//
//     // start frustum culling followed by visibility culling pass if it passed
//     for (size_t subMeshIndex = 0; subMeshIndex < gpuData.numSubMeshes; ++subMeshIndex) {
//         //        if (frustum.isBoundingBoxInside(gpuData.meshObjects[subMeshIndex]->boundingBox)) {
//         //            _frustumBuffer.setBit(subMeshIndex);
//         //        }
//         if (gpuData.meshObjects[subMeshIndex]->boundingBox.worldCorners[23].x == 1 &&
//             gpuData.meshObjects[subMeshIndex]->boundingBox.worldCorners[23].y == 1 &&
//             gpuData.meshObjects[subMeshIndex]->boundingBox.worldCorners[23].z == 1 &&
//             gpuData.meshObjects[subMeshIndex]->boundingBox.worldCorners[23].w == 1) {
//             _frustumBuffer.setBit(subMeshIndex);
//         }
//     }
// }
//
// void
// XPMetalCulling::doVisibilityCulling(XPMetalGPUData&      gpuData,
//                                     XPMetalOCBuffer&     ocBuffer,
//                                     MTL::CommandBuffer*  commandBuffer,
//                                     XPScene&             scene,
//                                     const packed_uint2&  dimensions,
//                                     const MTL::Viewport& viewport)
//{
//     XP_UNUSED(scene)
//
//     {
//         //        uint64_t* ptr = reinterpret_cast<uint64_t*>(_writeToVisibilityResultBuffer->contents());
//         //        memset64(ptr, XPMaxNumObjectsPerScene);
//     }
//
//     ocBuffer.frameBuffer->passDescriptor->colorAttachments()->object(0)->setClearColor(
//       ocBuffer.frameBuffer->clearColor);
//     ocBuffer.frameBuffer->passDescriptor->colorAttachments()->object(0)->setLoadAction(MTL::LoadActionClear);
//     ocBuffer.frameBuffer->passDescriptor->colorAttachments()->object(0)->setStoreAction(MTL::StoreActionStore);
//     ocBuffer.frameBuffer->passDescriptor->colorAttachments()->object(0)->setTexture(
//       ocBuffer.frameBuffer->colorAttachments[0].texture.get());
//     ocBuffer.frameBuffer->passDescriptor->depthAttachment()->setLoadAction(MTL::LoadActionDontCare);
//     ocBuffer.frameBuffer->passDescriptor->depthAttachment()->setStoreAction(MTL::StoreActionDontCare);
//     ocBuffer.frameBuffer->passDescriptor->depthAttachment()->setTexture(
//       ocBuffer.frameBuffer->depthAttachment.texture.get());
//     ocBuffer.frameBuffer->passDescriptor->setVisibilityResultBuffer(_writeToVisibilityResultBuffer);
//
//     auto encoder = commandBuffer->renderCommandEncoder(ocBuffer.frameBuffer->passDescriptor);
//     auto label   = NS_STRING_FROM_CSTRING("OCBuffer");
//     encoder->setLabel(label);
//     encoder->pushDebugGroup(label);
//     encoder->setViewport(viewport);
//     encoder->setFrontFacingWinding(MTL::WindingCounterClockwise);
//     encoder->setRenderPipelineState(ocBuffer.renderPipeline->renderPipelineState);
//     encoder->setDepthStencilState(ocBuffer.renderPipeline->depthStencilState);
//     encoder->setVertexBytes(&dimensions, sizeof(packed_uint2), XPVertexInputIndexViewportSize);
//
//     auto console               = _renderer->getRegistry()->getEngine()->getConsole();
//     bool frustumCullingEnabled = console->getVariableValue<bool>("r.frustumCulling");
//
//     size_t subMeshIndex         = 0;
//     size_t numTrackedGeometries = 0;
//     for (; subMeshIndex < gpuData.numSubMeshes && numTrackedGeometries < XPMaxNumVisibilityBufferGeometries;
//          ++subMeshIndex) {
//         size_t sortedMeshIndex = gpuData.sortedNodesIndices[subMeshIndex];
//         memcpy(&_frameDataPerObject[_frameDataIndex].modelMatrix._00,
//                &gpuData.modelMatrices[gpuData.perMeshNodeIndices[sortedMeshIndex]]._00,
//                sizeof(XPMat4<float>));
//         encoder->setVertexBytes(
//           &_frameDataPerObject[_frameDataIndex], sizeof(XPFrameDataPerObject), XPVertexInputFrameData);
//         if (!frustumCullingEnabled || (frustumCullingEnabled && isPassingFrustumCulling(subMeshIndex))) {
//             // do visibility culling test here ..
//             _indirectVisibilityIndices[_visibilityBufferWriteIndex][sortedMeshIndex] = numTrackedGeometries;
//             encoder->setVisibilityResultMode(MTL::VisibilityResultModeBoolean, numTrackedGeometries *
//             sizeof(uint64_t));
//             encoder->setVertexBuffer(gpuData.meshObjects[sortedMeshIndex]->mesh.vertexBuffer.get(),
//                                      XPMeshBuffer::sizeofPositions() *
//                                        gpuData.meshObjects[sortedMeshIndex]->vertexOffset,
//                                      XPVertexInputIndexPositions);
//             encoder->drawIndexedPrimitives(
//               MTL::PrimitiveTypeTriangle,
//               NS::UInteger(gpuData.meshObjects[sortedMeshIndex]->numIndices),
//               MTL::IndexTypeUInt32,
//               gpuData.meshObjects[sortedMeshIndex]->mesh.indexBuffer.get(),
//               NS::UInteger(XPMeshBuffer::sizeofIndices() * gpuData.meshObjects[sortedMeshIndex]->indexOffset));
//             ++numTrackedGeometries;
//         } else {
//             _indirectVisibilityIndices[_visibilityBufferWriteIndex][sortedMeshIndex] = -1;
//         }
//     }
//     while (subMeshIndex < gpuData.numSubMeshes) {
//         size_t sortedMeshIndex = gpuData.sortedNodesIndices[subMeshIndex];
//         _indirectVisibilityIndices[_visibilityBufferWriteIndex][sortedMeshIndex] = -1;
//         ++subMeshIndex;
//     }
//
//     encoder->popDebugGroup();
//     encoder->endEncoding();
// }
//
// void
// XPMetalCulling::incrementVisibilityBufferWriteIndex()
//{
//     _visibilityBufferWriteIndex    = (_visibilityBufferWriteIndex + 1) % XPNumPerFrameBuffers;
//     _writeToVisibilityResultBuffer = _visibilityBuffer[_visibilityBufferWriteIndex];
// }
//
// void
// XPMetalCulling::incrementVisibilityBufferReadIndex()
//{
//     _visibilityBufferReadIndex      = (_visibilityBufferReadIndex + 1) % XPNumPerFrameBuffers;
//     _readFromVisibilityResultBuffer = _visibilityBuffer[_visibilityBufferReadIndex];
// }
//
// void
// XPMetalCulling::copyVisibilityGPUBufferToCPUBuffer()
//{
//     //    MTL::CommandBuffer*      commandBuffer = _renderer->getIOCommandQueue()->commandBuffer();
//     //    MTL::BlitCommandEncoder* blitEncoder   = commandBuffer->blitCommandEncoder();
//     //    blitEncoder->copyFromBuffer(_readFromVisibilityResultBuffer,
//     //                                NS::UInteger(0),
//     //                                _visibilityCopyBuffer,
//     //                                NS::UInteger(0),
//     //                                _readFromVisibilityResultBuffer->length());
//     //    blitEncoder->endEncoding();
//     //    commandBuffer->addCompletedHandler([this](MTL::CommandBuffer*) {
//     //        uint64_t* buffer = static_cast<uint64_t*>(_visibilityCopyBuffer->contents());
//     //        for (size_t i = 0; i < XPMaxNumObjectsPerScene; ++i) {
//     //            if (buffer[i] > 0) {
//     //                _visibilityBitArray[_visibilityBufferReadIndex].setBit(i);
//     //            } else {
//     //                _visibilityBitArray[_visibilityBufferReadIndex].clearBit(i);
//     //            }
//     //        }
//     //    });
//     //    commandBuffer->commit();
// }
//
// XPMetalRenderer*
// XPMetalCulling::getRenderer() const
//{
//     return _renderer;
// }
//
// size_t
// XPMetalCulling::getVisibilityBufferWriteIndex() const
//{
//     return _visibilityBufferWriteIndex;
// }
//
// size_t
// XPMetalCulling::getVisibilityBufferReadIndex() const
//{
//     return _visibilityBufferReadIndex;
// }
//
// const std::array<MTL::Buffer*, XPNumPerFrameBuffers>&
// XPMetalCulling::getVisibilityBuffer() const
//{
//     return _visibilityBuffer;
// }
//
// bool
// XPMetalCulling::isPassingFrustumCulling(size_t subMeshIndex) const
//{
//     return _frustumBuffer.isSet(subMeshIndex);
// }
//
// bool
// XPMetalCulling::isPassingVisibilityCulling(size_t sortedMeshIndex) const
//{
//     int64_t efficientIndex = _indirectVisibilityIndices[_visibilityBufferReadIndex][sortedMeshIndex];
//     if (efficientIndex < 0) { return true; }
//     return static_cast<uint64_t*>(_readFromVisibilityResultBuffer->contents())[efficientIndex] > 0;
// }
