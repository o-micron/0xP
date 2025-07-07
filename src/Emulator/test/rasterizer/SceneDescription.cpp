#include "SceneDescription.h"

// TRIANGLE -----------------------------------------------------------------------------------------------------------
template<typename T>
[[nodiscard]] T
Triangle<T>::edgeFunction(const Vec2<T>& v0, const Vec2<T>& v1, const Vec2<T>& v2)
{
    return (v2.x - v0.x) * (v1.y - v0.y) - (v2.y - v0.y) * (v1.x - v0.x);
}
template<typename T>
[[nodiscard]] T
Triangle<T>::area(const Vec2<T>& v0, const Vec2<T>& v1, const Vec2<T>& v2)
{
    return Triangle::edgeFunction(v0, v1, v2);
}

template<typename T>
[[nodiscard]] Vec3<T>
Triangle<T>::normal() const
{
    const Vec3<T> edge1 = { v1.location.x - v0.location.x,
                            v1.location.y - v0.location.y,
                            v1.location.z - v0.location.z };
    const Vec3<T> edge2 = { v2.location.x - v0.location.x,
                            v2.location.y - v0.location.y,
                            v2.location.z - v0.location.z };
    return Vec3<T>{ edge1.y * edge2.z - edge1.z * edge2.y,
                    edge1.z * edge2.x - edge1.x * edge2.z,
                    edge1.x * edge2.y - edge1.y * edge2.x };
}
// --------------------------------------------------------------------------------------------------------------------

// PLANE --------------------------------------------------------------------------------------------------------------
template<typename T>
void
Plane<T>::normalize()
{
    double length = std::sqrt(static_cast<double>(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z));
    normal.x /= length;
    normal.y /= length;
    normal.z /= length;
    normal.w /= length;
}

template<typename T>
T
Plane<T>::distanceFromPoint(Vec3<T> point) const
{
    return normal.x * point.x + normal.y * point.y + normal.z * point.z + distance;
}

template<typename T>
[[nodiscard]] std::array<Plane<T>, 6>
Plane<T>::extractFrustumPlanes(const Mat4<T>& viewProjectionMatrix)
{
    std::array<Plane, 6> frustumPlanes = {};
    // Left plane
    frustumPlanes[0].normal.x = viewProjectionMatrix[3] + viewProjectionMatrix[0];
    frustumPlanes[0].normal.y = viewProjectionMatrix[7] + viewProjectionMatrix[4];
    frustumPlanes[0].normal.z = viewProjectionMatrix[11] + viewProjectionMatrix[8];
    frustumPlanes[0].normal.w = viewProjectionMatrix[15] + viewProjectionMatrix[12];
    frustumPlanes[0].normalize();

    // Right plane
    frustumPlanes[1].normal.x = viewProjectionMatrix[3] - viewProjectionMatrix[0];
    frustumPlanes[1].normal.y = viewProjectionMatrix[7] - viewProjectionMatrix[4];
    frustumPlanes[1].normal.z = viewProjectionMatrix[11] - viewProjectionMatrix[8];
    frustumPlanes[1].normal.w = viewProjectionMatrix[15] - viewProjectionMatrix[12];
    frustumPlanes[1].normalize();

    // Bottom plane
    frustumPlanes[2].normal.x = viewProjectionMatrix[3] + viewProjectionMatrix[1];
    frustumPlanes[2].normal.y = viewProjectionMatrix[7] + viewProjectionMatrix[5];
    frustumPlanes[2].normal.z = viewProjectionMatrix[11] + viewProjectionMatrix[9];
    frustumPlanes[2].normal.w = viewProjectionMatrix[15] + viewProjectionMatrix[13];
    frustumPlanes[2].normalize();

    // Top plane
    frustumPlanes[3].normal.x = viewProjectionMatrix[3] - viewProjectionMatrix[1];
    frustumPlanes[3].normal.y = viewProjectionMatrix[7] - viewProjectionMatrix[5];
    frustumPlanes[3].normal.z = viewProjectionMatrix[11] - viewProjectionMatrix[9];
    frustumPlanes[3].normal.w = viewProjectionMatrix[15] - viewProjectionMatrix[13];
    frustumPlanes[3].normalize();

    // Near plane
    frustumPlanes[4].normal.x = viewProjectionMatrix[3] + viewProjectionMatrix[2];
    frustumPlanes[4].normal.y = viewProjectionMatrix[7] + viewProjectionMatrix[6];
    frustumPlanes[4].normal.z = viewProjectionMatrix[11] + viewProjectionMatrix[10];
    frustumPlanes[4].normal.w = viewProjectionMatrix[15] + viewProjectionMatrix[14];
    frustumPlanes[4].normalize();

    // Far plane
    frustumPlanes[5].normal.x = viewProjectionMatrix[3] - viewProjectionMatrix[2];
    frustumPlanes[5].normal.y = viewProjectionMatrix[7] - viewProjectionMatrix[6];
    frustumPlanes[5].normal.z = viewProjectionMatrix[11] - viewProjectionMatrix[10];
    frustumPlanes[5].normal.w = viewProjectionMatrix[15] - viewProjectionMatrix[14];
    frustumPlanes[5].normalize();

    return frustumPlanes;
}
// --------------------------------------------------------------------------------------------------------------------

// BOUNDING BOX -------------------------------------------------------------------------------------------------------
template<typename T>
EBoundingBoxFrustumTest
BoundingBox<T>::testFrustum(const std::array<Plane<T>, 6>& frustum) const
{
    bool isFullyInside = true;

    for (int i = 0; i < 6; i++) {
        Plane plane = frustum[i];

        // Find the AABB corner that is farthest in the direction of the plane normal
        Vec3<T> positiveCorner;
        positiveCorner.x = (plane.normal.x > 0) ? max.x : min.x;
        positiveCorner.y = (plane.normal.y > 0) ? max.y : min.y;
        positiveCorner.z = (plane.normal.z > 0) ? max.z : min.z;

        // Find the AABB corner that is closest in the direction of the plane normal
        Vec3<T> negativeCorner;
        negativeCorner.x = (plane.normal.x > 0) ? min.x : max.x;
        negativeCorner.y = (plane.normal.y > 0) ? min.y : max.y;
        negativeCorner.z = (plane.normal.z > 0) ? min.z : max.z;

        // If the farthest corner is outside the plane, the AABB is outside the frustum
        if (plane.distanceFromPoint(positiveCorner) < 0) { return EBoundingBoxFrustumTest_FullyOutside; }

        // If the closest corner is outside the plane, the AABB is intersecting the frustum
        if (plane.distanceFromPoint(negativeCorner) < 0) {
            isFullyInside = false; // AABB is intersecting the frustum
        }
    }

    // If the AABB is not fully inside and not fully outside, it is intersecting
    if (!isFullyInside) { return EBoundingBoxFrustumTest_Intersecting; }

    return EBoundingBoxFrustumTest_FullyInside;
}
// --------------------------------------------------------------------------------------------------------------------

// CAMERA -------------------------------------------------------------------------------------------------------------
template<typename T>
bool
Camera<T>::isBackFace(const Vec4<T>& projectedVertex0,
                      const Vec4<T>& projectedVertex1,
                      const Vec4<T>& projectedVertex2) const
{
    // Perform back-face culling
    Vec3<T> cameraDirection = { -viewMatrix.glm[0][2], -viewMatrix.glm[1][2], -viewMatrix.glm[2][2] };
    cameraDirection.normalize();

    glm::mat<4, 4, T, glm::defaultp> inverseViewMatrix       = glm::inverse(viewMatrix.glm);
    glm::mat<4, 4, T, glm::defaultp> inverseProjectionMatrix = glm::inverse(projectionMatrix.glm);

    glm::vec<4, T, glm::defaultp> wv0     = inverseViewMatrix * inverseProjectionMatrix * projectedVertex0.glm;
    Vec3<T>                       worldV0 = { wv0.x, wv0.y, wv0.z };

    glm::vec<4, T, glm::defaultp> wv1     = inverseViewMatrix * inverseProjectionMatrix * projectedVertex1.glm;
    Vec3<T>                       worldV1 = { wv1.x, wv1.y, wv1.z };

    glm::vec<4, T, glm::defaultp> wv2     = inverseViewMatrix * inverseProjectionMatrix * projectedVertex2.glm;
    Vec3<T>                       worldV2 = { wv2.x, wv2.y, wv2.z };

    Vec3<T> edge1  = worldV1 - worldV0;
    Vec3<T> edge2  = worldV2 - worldV0;
    Vec3<T> normal = edge1.cross(edge2);
    normal.normalize();

    return normal.dot(cameraDirection) < 0;
}

template<typename T>
bool
Camera<T>::isBackFace(const Vec3<T>& worldVertex0, const Vec3<T>& worldVertex1, const Vec3<T>& worldVertex2) const
{
    // Perform back-face culling
    Vec3<T> cameraDirection = { -viewMatrix.glm[0][2], -viewMatrix.glm[1][2], -viewMatrix.glm[2][2] };
    cameraDirection.normalize();

    Vec3<T> edge1  = worldVertex1 - worldVertex0;
    Vec3<T> edge2  = worldVertex2 - worldVertex0;
    Vec3<T> normal = edge1.cross(edge2);
    normal.normalize();

    return normal.dot(cameraDirection) < 0;
}

template<typename T>
Ray<T>
Camera<T>::getProjectedRay(const Vec2<int>& pixel)
{
    // NDC coordinates (-1 to 1)
    const float x_ndc = (2.0f * (static_cast<float>(pixel.x) + 0.5f) / resolution.x) - 1.0f;
    const float y_ndc = 1.0f - (2.0f * (static_cast<float>(pixel.y) + 0.5f) / resolution.y);

    // Clip-space (z=-1 for OpenGL, z=0 for DirectX)
    const Vec4<T> clip_coords = { x_ndc, y_ndc, 0.0f, 1.0f };

    // Transform to world-space
    auto world_pos = inverseViewMatrix * inverseProjectionMatrix * clip_coords;
    world_pos.x    = world_pos.x / world_pos.w;
    world_pos.y    = world_pos.y / world_pos.w;
    world_pos.z    = world_pos.z / world_pos.w;

    // Ray origin (camera position)
    auto ray_origin = inverseViewMatrix.row3.xyz;

    // Ray direction
    auto ray_dir = world_pos.xyz - ray_origin;
    ray_dir.normalize();

    Ray<T> ray = {};
    ray.start  = Vec3<T>{ ray_origin.x + ray_dir.x * zNearPlane,
                          ray_origin.y + ray_dir.y * zNearPlane,
                          ray_origin.z + ray_dir.z * zNearPlane };
    ray.end    = Vec3<T>{ ray_origin.x + ray_dir.x * zFarPlane,
                          ray_origin.y + ray_dir.y * zFarPlane,
                          ray_origin.z + ray_dir.z * zFarPlane };
    return ray;
}

template<typename T>
float
Camera<T>::extractNear() const
{
    return projectionMatrix.glm[3][2] / (projectionMatrix.glm[2][2] - 1.0f);
}

template<typename T>
float
Camera<T>::extractFar() const
{
    return projectionMatrix.glm[3][2] / (projectionMatrix.glm[2][2] + 1.0f);
}
// --------------------------------------------------------------------------------------------------------------------

// FRAME BUFFER REF ---------------------------------------------------------------------------------------------------
FramebufferRef::FramebufferRef(float* preAllocatedColorAttachmentPtr, float* preAllocatedDepthAttachmentPtr, int width, int height)
  : colorAttachmentPtr(preAllocatedColorAttachmentPtr)
  , depthAttachmentPtr(preAllocatedDepthAttachmentPtr)
  , width(width)
  , height(height)
{
}
// --------------------------------------------------------------------------------------------------------------------

// FRAME MEMORY POOL --------------------------------------------------------------------------------------------------
FrameMemoryPool::FrameMemoryPool(uint8_t* preAllocatedMemPtr, size_t preAllocatedNumBytes)
  : memPtr(preAllocatedMemPtr)
  , memStartIndex(0)
  , memEndIndex(preAllocatedNumBytes)
{
}

uint8_t*
FrameMemoryPool::pushMemory(size_t numBytes)
{
    if (memStartIndex + numBytes <= memEndIndex) {
        uint8_t* ptr = &memPtr[memStartIndex];
        memStartIndex += static_cast<int64_t>(numBytes);
        return ptr;
    }
    assert(false && "Memory full, you need to expand the maximum size of a frame memory");
    return nullptr;
}

void
FrameMemoryPool::popMemory(size_t numBytes)
{
    assert((memStartIndex - static_cast<int64_t>(numBytes)) >= 0 &&
           "You need to push memory first then return it back via calling this function");
    memStartIndex -= static_cast<int64_t>(numBytes);
}

void
FrameMemoryPool::popAllMemory()
{
    memStartIndex = 0;
}

void
FrameMemoryPool::memsetZeros()
{
    memset(memPtr, 0, memEndIndex);
}
// --------------------------------------------------------------------------------------------------------------------

template struct Triangle<float>;
template struct Plane<float>;
template struct BoundingBox<float>;
template struct Ray<float>;
template struct Camera<float>;
template struct Mesh<float>;
template struct Material<float>;
template struct Light<float>;
template struct Scene<float>;
