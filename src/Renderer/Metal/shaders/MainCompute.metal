#include <metal_stdlib>
#include <metal_common>
#include <metal_math>
#include <metal_matrix>
using namespace metal;

#ifdef XP_RUNTIME_COMPILER
    #include "shaders/XPShaderTypes.h"
#else
    #include "../XPShaderTypes.h"
#endif

// http://iquilezles.org/www/articles/frustumcorrect/frustumcorrect.htm
bool IsBoxVisible(const float4 minp, const float4 maxp, device XPFrameDataPerObject& frameDataPerObject)
{
    // check box outside/inside of frustum
    for (int i = 0; i < Count; i++) {
        if ((dot(frameDataPerObject.frustumPlanes[i], float4(minp.x, minp.y, minp.z, 1.0f)) < 0.0) != 0 &&
            (dot(frameDataPerObject.frustumPlanes[i], float4(maxp.x, minp.y, minp.z, 1.0f)) < 0.0) != 0 &&
            (dot(frameDataPerObject.frustumPlanes[i], float4(minp.x, maxp.y, minp.z, 1.0f)) < 0.0) != 0 &&
            (dot(frameDataPerObject.frustumPlanes[i], float4(maxp.x, maxp.y, minp.z, 1.0f)) < 0.0) != 0 &&
            (dot(frameDataPerObject.frustumPlanes[i], float4(minp.x, minp.y, maxp.z, 1.0f)) < 0.0) != 0 &&
            (dot(frameDataPerObject.frustumPlanes[i], float4(maxp.x, minp.y, maxp.z, 1.0f)) < 0.0) != 0 &&
            (dot(frameDataPerObject.frustumPlanes[i], float4(minp.x, maxp.y, maxp.z, 1.0f)) < 0.0) != 0 &&
            (dot(frameDataPerObject.frustumPlanes[i], float4(maxp.x, maxp.y, maxp.z, 1.0f)) < 0.0) != 0) {
            return false;
        }
    }

    // check frustum outside/inside box
    int out;
    out = 0;
    for (int i = 0; i < 8; i++) out += ((frameDataPerObject.frustumPoints[i].x > maxp.x) ? 1 : 0);
    if (out == 8) return false;
    out = 0;
    for (int i = 0; i < 8; i++) out += ((frameDataPerObject.frustumPoints[i].x < minp.x) ? 1 : 0);
    if (out == 8) return false;
    out = 0;
    for (int i = 0; i < 8; i++) out += ((frameDataPerObject.frustumPoints[i].y > maxp.y) ? 1 : 0);
    if (out == 8) return false;
    out = 0;
    for (int i = 0; i < 8; i++) out += ((frameDataPerObject.frustumPoints[i].y < minp.y) ? 1 : 0);
    if (out == 8) return false;
    out = 0;
    for (int i = 0; i < 8; i++) out += ((frameDataPerObject.frustumPoints[i].z > maxp.z) ? 1 : 0);
    if (out == 8) return false;
    out = 0;
    for (int i = 0; i < 8; i++) out += ((frameDataPerObject.frustumPoints[i].z < minp.z) ? 1 : 0);
    if (out == 8) return false;

    return true;
}

// points [24]
kernel void computeShader(device const float4x4* modelMatrices [[buffer(0)]],
                          device XPGPUBoundingBox* boundingBoxes [[buffer(1)]],
                          constant uint* modelMatricesIndices [[buffer(2)]],
                          device XPFrameDataPerObject& frameDataPerObject [[buffer(3)]],
                          uint index [[thread_position_in_grid]])
{
    boundingBoxes[index].minPoint.w = 1;

    float4 minWorld = modelMatrices[modelMatricesIndices[index]] * boundingBoxes[index].minPoint;
    float4 maxWorld = modelMatrices[modelMatricesIndices[index]] * boundingBoxes[index].maxPoint;

    boundingBoxes[index].points[0]  = minWorld;
    boundingBoxes[index].points[7]  = minWorld;
    boundingBoxes[index].points[16] = minWorld;
    boundingBoxes[index].points[11] = maxWorld;
    boundingBoxes[index].points[12] = maxWorld;
    boundingBoxes[index].points[21] = maxWorld;

    float4 a = float4(boundingBoxes[index].points[11].x, boundingBoxes[index].points[0].y, boundingBoxes[index].points[0].z, 1.0);
    float4 b = float4(boundingBoxes[index].points[0].x, boundingBoxes[index].points[11].y, boundingBoxes[index].points[0].z, 1.0);
    float4 c = float4(boundingBoxes[index].points[11].x, boundingBoxes[index].points[11].y, boundingBoxes[index].points[0].z, 1.0);
    float4 d = float4(boundingBoxes[index].points[0].x, boundingBoxes[index].points[0].y, boundingBoxes[index].points[11].z, 1.0);
    float4 e = float4(boundingBoxes[index].points[11].x, boundingBoxes[index].points[0].y, boundingBoxes[index].points[11].z, 1.0);
    float4 f = float4(boundingBoxes[index].points[0].x, boundingBoxes[index].points[11].y, boundingBoxes[index].points[11].z, 1.0);

    boundingBoxes[index].points[5]  = a;
    boundingBoxes[index].points[6]  = a;
    boundingBoxes[index].points[22] = a;
    boundingBoxes[index].points[1]  = b;
    boundingBoxes[index].points[2]  = b;
    boundingBoxes[index].points[18] = b;
    boundingBoxes[index].points[3]  = c;
    boundingBoxes[index].points[4]  = c;
    boundingBoxes[index].points[20] = c;
    boundingBoxes[index].points[8]  = d;
    boundingBoxes[index].points[15] = d;
    boundingBoxes[index].points[17] = d;
    boundingBoxes[index].points[13] = e;
    boundingBoxes[index].points[14] = e;
    boundingBoxes[index].points[23] = e;
    boundingBoxes[index].points[9]  = f;
    boundingBoxes[index].points[10] = f;
    boundingBoxes[index].points[19] = f;
    
    // for(uint i = 0; i < 24; ++i) {
    //     boundingBoxes[index].points[i] = modelMatrices[modelMatricesIndices[index]] * boundingBoxes[index].points[i];
    // }
    
//    frameDataPerObject.frustumCorners[0] = frustumPoints[1];
//    frameDataPerObject.frustumCorners[1] = frustumPoints[0];
//    frameDataPerObject.frustumCorners[2] = frustumPoints[0];
//    frameDataPerObject.frustumCorners[3] = frustumPoints[3];
//    frameDataPerObject.frustumCorners[4] = frustumPoints[3];
//    frameDataPerObject.frustumCorners[5] = frustumPoints[2];
//    frameDataPerObject.frustumCorners[6] = frustumPoints[2];
//    frameDataPerObject.frustumCorners[7] = frustumPoints[1];
//    frameDataPerObject.frustumCorners[8] = frustumPoints[5];
//    frameDataPerObject.frustumCorners[9] = frustumPoints[4];
//    frameDataPerObject.frustumCorners[10] = frustumPoints[4];
//    frameDataPerObject.frustumCorners[11] = frustumPoints[7];
//    frameDataPerObject.frustumCorners[12] = frustumPoints[7];
//    frameDataPerObject.frustumCorners[13] = frustumPoints[6];
//    frameDataPerObject.frustumCorners[14] = frustumPoints[6];
//    frameDataPerObject.frustumCorners[15] = frustumPoints[5];
//    frameDataPerObject.frustumCorners[16] = frustumPoints[1];
//    frameDataPerObject.frustumCorners[17] = frustumPoints[5];
//    frameDataPerObject.frustumCorners[18] = frustumPoints[0];
//    frameDataPerObject.frustumCorners[19] = frustumPoints[4];
//    frameDataPerObject.frustumCorners[20] = frustumPoints[2];
//    frameDataPerObject.frustumCorners[21] = frustumPoints[6];
//    frameDataPerObject.frustumCorners[22] = frustumPoints[3];
//    frameDataPerObject.frustumCorners[23] = frustumPoints[7];
    
    bool isVisible = IsBoxVisible(boundingBoxes[index].points[0], boundingBoxes[index].points[11], frameDataPerObject);
    if(isVisible) { boundingBoxes[index].minPoint.w = 1;
    } else { boundingBoxes[index].minPoint.w = 0; }
}
