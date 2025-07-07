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
// #pragma once
//
// #include <Renderer/Metal/XPMetal.h>
// #include <Renderer/Metal/XPMetalGPUData.h>
// #include <Utilities/XPBitArray.h>
// #include <Utilities/XPPlatforms.h>
//
// #include <array>
// #include <unordered_map>
// #include <unordered_set>
//
// enum XPMetalCullingTestType_
//{
//    XPMetalCullingTestType_None       = 0,
//    XPMetalCullingTestType_Frustum    = 1,
//    XPMetalCullingTestType_Visibility = 2,
//};
//
// class XPMetalRenderer;
// class XPScene;
//
// class XPMetalCulling
//{
//  public:
//    XPMetalCulling(XPMetalRenderer* const renderer);
//    ~XPMetalCulling();
//    void initialize(MTL::Device* device);
//    void finalize();
//    void update(const XPFrustum&      frustum,
//                XPMetalGPUData&       gpuData,
//                XPFrameDataPerObject& frameDataPerObject,
//                size_t                frameDataIndex,
//                XPMetalOCBuffer&      ocBuffer,
//                MTL::CommandBuffer*   commandBuffer,
//                XPScene&              scene,
//                const packed_uint2&   dimensions,
//                const MTL::Viewport&  viewport);
//    void doFrustumCulling(const XPFrustum&     frustum,
//                          XPMetalGPUData&      gpuData,
//                          XPMetalOCBuffer&     ocBuffer,
//                          MTL::CommandBuffer*  commandBuffer,
//                          XPScene&             scene,
//                          const packed_uint2&  dimensions,
//                          const MTL::Viewport& viewport);
//    void doVisibilityCulling(XPMetalGPUData&      gpuData,
//                             XPMetalOCBuffer&     ocBuffer,
//                             MTL::CommandBuffer*  commandBuffer,
//                             XPScene&             scene,
//                             const packed_uint2&  dimensions,
//                             const MTL::Viewport& viewport);
//    void incrementVisibilityBufferWriteIndex();
//    void incrementVisibilityBufferReadIndex();
//    void copyVisibilityGPUBufferToCPUBuffer();
//
//    XPMetalRenderer*                                      getRenderer() const;
//    size_t                                                getVisibilityBufferWriteIndex() const;
//    size_t                                                getVisibilityBufferReadIndex() const;
//    const std::array<MTL::Buffer*, XPNumPerFrameBuffers>& getVisibilityBuffer() const;
//    bool                                                  isPassingFrustumCulling(size_t subMeshIndex) const;
//    bool                                                  isPassingVisibilityCulling(size_t sortedMeshIndex) const;
//
//  private:
//    XPMetalRenderer* const                                 _renderer;
//    std::array<std::vector<int64_t>, XPNumPerFrameBuffers> _indirectVisibilityIndices;
//    XPBitArray                                             _frustumBuffer;
//    std::array<MTL::Buffer*, XPNumPerFrameBuffers>         _visibilityBuffer;
//    std::array<XPFrameDataPerObject, XPNumPerFrameBuffers> _frameDataPerObject;
//    MTL::Buffer*                                           _readFromVisibilityResultBuffer;
//    MTL::Buffer*                                           _writeToVisibilityResultBuffer;
//    size_t                                                 _frameDataIndex;
//    size_t                                                 _visibilityBufferReadIndex;
//    size_t                                                 _visibilityBufferWriteIndex;
//};
