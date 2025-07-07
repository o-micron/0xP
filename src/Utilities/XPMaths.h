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

#pragma once

#include <Utilities/XPPlatforms.h>

#include <array>
#include <cmath>
#include <cstring>
#include <iomanip>
#include <ostream>

#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wall"
    #pragma clang diagnostic ignored "-Weverything"
#endif
// #define GLM_FORCE_ALIGNED
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>
#ifdef __clang__
    #pragma clang diagnostic pop
#endif

/// A general purpose 2-components vector
template<typename T>
struct XPVec2
{
    XPVec2()
      : x{}
      , y{}
    {
    }
    XPVec2(T defaultValue)
      : x(defaultValue)
      , y(defaultValue)
    {
    }
    XPVec2(T x, T y)
      : x(x)
      , y(y)
    {
    }
    XPVec2(const glm::vec2& glm)
      : glm(glm)
    {
    }
    XPVec2(const XPVec2<T>& other) = default;
    XPVec2(XPVec2<T>&& other)      = default;
    union
    {
        struct
        {
            T x;
            T y;
        };
        struct
        {
            T a;
            T b;
        };
        struct
        {
            T _0;
            T _1;
        };
        struct
        {
            std::array<T, 2> arr;
        };
        struct
        {
            glm::vec2 glm;
        };
    };

    [[nodiscard]] T&       operator[](int i) { return arr[i]; }
    [[nodiscard]] const T& operator[](int i) const { return arr[i]; }
    [[nodiscard]] bool     operator==(const XPVec2<T>& other) { return x == other.x && y == other.y; }
    XPVec2&                operator=(const XPVec2& rhs)
    {
        if (this == &rhs) { return *this; }
        x = rhs.x;
        y = rhs.y;
        return *this;
    }
};
static_assert(sizeof(XPVec2<float>) == sizeof(float) * 2);

/// A general purpose 3-components vector
template<typename T>
struct XPVec3
{
    XPVec3()
      : x{}
      , y{}
      , z{}
    {
    }
    XPVec3(T defaultValue)
      : x(defaultValue)
      , y(defaultValue)
      , z(defaultValue)
    {
    }
    XPVec3(T x, T y, T z)
      : x(x)
      , y(y)
      , z(z)
    {
    }
    XPVec3(const glm::vec3& glm)
      : glm(glm)
    {
    }
    XPVec3(const XPVec3<T>& other) = default;
    XPVec3(XPVec3<T>&& other)      = default;

    union
    {
        struct
        {
            T x;
            T y;
            T z;
        };
        struct
        {
            T a;
            T b;
            T c;
        };
        struct
        {
            T _0;
            T _1;
            T _2;
        };
        struct
        {
            std::array<T, 3> arr;
        };
        struct
        {
            glm::vec3 glm;
        };
        struct
        {
            XPVec2<T> xy;
            T         __z;
        };
        struct
        {
            T         __x;
            XPVec2<T> yz;
        };
    };

    [[nodiscard]] T&       operator[](int i) { return arr[i]; }
    [[nodiscard]] const T& operator[](int i) const { return arr[i]; }
    [[nodiscard]] bool     operator==(const XPVec3<T>& other) { return x == other.x && y == other.y && z == other.z; }
    XPVec3&                operator=(const XPVec3& rhs)
    {
        if (this == &rhs) { return *this; }
        x = rhs.x;
        y = rhs.y;
        z = rhs.z;
        return *this;
    }
    void                    normalize() { this->glm = glm::normalize<3, T, glm::defaultp>(this->glm); }
    [[nodiscard]] XPVec3<T> cross(const XPVec3<T>& other) const
    {
        XPVec3<T> result;
        result.glm = glm::cross<T, glm::defaultp>(this->glm, other.glm);
        return result;
    }
    [[nodiscard]] T dot(const XPVec3<T>& other) const { return glm::dot<3, T, glm::defaultp>(glm, other.glm); }
    [[nodiscard]] T distanceTo(const XPVec3<T>& other) const
    {
        return glm::distance<3, T, glm::defaultp>(this->glm, other.glm);
    }
};
static_assert(sizeof(XPVec3<float>) == sizeof(float) * 3);

/// A general purpose 4-components vector
template<typename T>
struct XPVec4
{
    XPVec4()
      : x{}
      , y{}
      , z{}
      , w{}
    {
    }
    XPVec4(T defaultValue)
      : x(defaultValue)
      , y(defaultValue)
      , z(defaultValue)
      , w(defaultValue)
    {
    }
    XPVec4(T x, T y, T z, T w)
      : x(x)
      , y(y)
      , z(z)
      , w(w)
    {
    }
    XPVec4(const glm::vec4& glm)
      : glm(glm)
    {
    }
    XPVec4(const XPVec4<T>& other) = default;
    XPVec4(XPVec4<T>&& other)      = default;

    union
    {
        struct
        {
            T x;
            T y;
            T z;
            T w;
        };
        struct
        {
            T a;
            T b;
            T c;
            T d;
        };
        struct
        {
            T _0;
            T _1;
            T _2;
            T _3;
        };
        struct
        {
            std::array<T, 4> arr;
        };
        struct
        {
            glm::vec4 glm;
        };
        struct
        {
            XPVec3<T> xyz;
            T         __w;
        };
        struct
        {
            T         __x;
            XPVec3<T> yzw;
        };
        struct
        {
            XPVec2<T> xy;
            XPVec2<T> zw;
        };
    };

    [[nodiscard]] T&       operator[](int i) { return arr[i]; }
    [[nodiscard]] const T& operator[](int i) const { return arr[i]; }
    [[nodiscard]] bool     operator==(const XPVec4<T>& other)
    {
        return x == other.x && y == other.y && z == other.z && w == other.w;
    }
    XPVec4& operator=(const XPVec4& rhs)
    {
        if (this == &rhs) { return *this; }
        x = rhs.x;
        y = rhs.y;
        z = rhs.z;
        w = rhs.w;
        return *this;
    }
};
static_assert(sizeof(XPVec4<float>) == sizeof(float) * 4);

/// A general purpose 3x3 matrix
template<typename T>
struct XPMat3
{
    XPMat3()
      : _00(1.0)
      , _01(0.0)
      , _02(0.0)
      , _10(0.0)
      , _11(1.0)
      , _12(0.0)
      , _20(0.0)
      , _21(0.0)
      , _22(1.0)
    {
    }
    XPMat3(T _00, T _01, T _02, T _10, T _11, T _12, T _20, T _21, T _22)
      : _00(_00)
      , _01(_01)
      , _02(_02)
      , _10(_10)
      , _11(_11)
      , _12(_12)
      , _20(_20)
      , _21(_21)
      , _22(_22)
    {
    }
    XPMat3(const glm::mat3& glm)
      : glm(glm)
    {
    }
    XPMat3(const XPMat3<T>& other) = default;
    XPMat3(XPMat3<T>&& other)      = default;

    static XPMat3<T> identity() { return XPMat3<T>{ 1, 0, 0, 0, 1, 0, 0, 0, 1 }; }

    union
    {
        struct
        {
            T _00;
            T _01;
            T _02;

            T _10;
            T _11;
            T _12;

            T _20;
            T _21;
            T _22;
        };
        struct
        {
            XPVec3<T> row0;
            XPVec3<T> row1;
            XPVec3<T> row2;
        };
        struct
        {
            std::array<T, 9> arr;
        };
        struct
        {
            glm::mat3 glm;
        };
    };

    [[nodiscard]] T&       operator[](int i) { return arr[i]; }
    [[nodiscard]] const T& operator[](int i) const { return arr[i]; }
    [[nodiscard]] bool     operator==(const XPMat3<T>& other)
    {
        return row0 == other.row0 && row1 == other.row1 && row2 == other.row2;
    }
    XPMat3& operator=(const XPMat3& rhs)
    {
        if (this == &rhs) { return *this; }
        _00 = rhs._00;
        _01 = rhs._01;
        _02 = rhs._02;
        _10 = rhs._10;
        _11 = rhs._11;
        _12 = rhs._12;
        _20 = rhs._20;
        _21 = rhs._21;
        _22 = rhs._22;
        return *this;
    }
};
static_assert(sizeof(XPMat3<float>) == sizeof(float) * 3 * 3);

/// A general purpose 4x4 matrix
template<typename T>
struct XPMat4
{
    XPMat4()
      : _00(1.0)
      , _01(0.0)
      , _02(0.0)
      , _03(0.0)
      , _10(0.0)
      , _11(1.0)
      , _12(0.0)
      , _13(0.0)
      , _20(0.0)
      , _21(0.0)
      , _22(1.0)
      , _23(0.0)
      , _30(0.0)
      , _31(0.0)
      , _32(0.0)
      , _33(1.0)
    {
    }
    XPMat4(T _00,
           T _01,
           T _02,
           T _03,
           T _10,
           T _11,
           T _12,
           T _13,
           T _20,
           T _21,
           T _22,
           T _23,
           T _30,
           T _31,
           T _32,
           T _33)
      : _00(_00)
      , _01(_01)
      , _02(_02)
      , _03(_03)
      , _10(_10)
      , _11(_11)
      , _12(_12)
      , _13(_13)
      , _20(_20)
      , _21(_21)
      , _22(_22)
      , _23(_23)
      , _30(_30)
      , _31(_31)
      , _32(_32)
      , _33(_33)
    {
    }
    XPMat4(const glm::mat4& glm)
      : glm(glm)
    {
    }
    XPMat4(const XPMat4<T>& other) = default;
    XPMat4(XPMat4<T>&& other)      = default;

    static XPMat4<T> identity() { return XPMat4<T>{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 }; }

    union
    {
        struct
        {
            T _00;
            T _01;
            T _02;
            T _03;

            T _10;
            T _11;
            T _12;
            T _13;

            T _20;
            T _21;
            T _22;
            T _23;

            T _30;
            T _31;
            T _32;
            T _33;
        };
        struct
        {
            XPVec4<T> row0;
            XPVec4<T> row1;
            XPVec4<T> row2;
            XPVec4<T> row3;
        };
        struct
        {
            std::array<T, 16> arr;
        };
        struct
        {
            glm::mat4 glm;
        };
    };

    [[nodiscard]] T&       operator[](int i) { return arr[i]; }
    [[nodiscard]] const T& operator[](int i) const { return arr[i]; }
    [[nodiscard]] bool     operator==(const XPMat4<T>& other)
    {
        return row0 == other.row0 && row1 == other.row1 && row2 == other.row2 && row3 == other.row3;
    }
    XPMat4& operator=(const XPMat4& rhs)
    {
        if (this == &rhs) { return *this; }
        _00 = rhs._00;
        _01 = rhs._01;
        _02 = rhs._02;
        _03 = rhs._03;
        _10 = rhs._10;
        _11 = rhs._11;
        _12 = rhs._12;
        _13 = rhs._13;
        _20 = rhs._20;
        _21 = rhs._21;
        _22 = rhs._22;
        _23 = rhs._23;
        _30 = rhs._30;
        _31 = rhs._31;
        _32 = rhs._32;
        _33 = rhs._33;
        return *this;
    }

    typedef uint8_t ModelMatrixOperation_;
    enum ModelMatrixOperations : uint8_t
    {
        ModelMatrixOperation_None        = 0,
        ModelMatrixOperation_Translation = 1,
        ModelMatrixOperation_Rotation    = 2,
        ModelMatrixOperation_Scale       = 4,
    };

    /// Decomposes the model matrix into position, eulerRotation and scale components
    static void decomposeModelMatrix(const XPMat4<float>& model,
                                     XPVec3<float>&       position,
                                     XPVec3<float>&       eulerRotation,
                                     XPVec3<float>&       scale)
    {
        glm::quat glm_orientation;
        glm::vec3 glm_position;
        glm::vec3 glm_scale;
        glm::vec3 glm_skew;
        glm::vec4 glm_perspective;
        glm::decompose(model.glm, glm_scale, glm_orientation, glm_position, glm_skew, glm_perspective);
        glm::vec3 glm_rotation = glm::degrees(glm::eulerAngles(glm_orientation));
        memcpy(&position.x, &glm_position.x, sizeof(glm::vec3));
        memcpy(&eulerRotation.x, &glm_rotation.x, sizeof(glm::vec3));
        memcpy(&scale.x, &glm_scale.x, sizeof(glm::vec3));
    }

    /// Builds a model matrix by applying the selected transformation operations
    static void buildModelMatrix(XPMat4<float>&              model,
                                 const XPVec3<float>&        position,
                                 const XPVec3<float>&        eulerRotation,
                                 const XPVec3<float>&        scale,
                                 const ModelMatrixOperations operations)
    {
        model.glm = glm::mat4(1.0f);
        if ((operations & ModelMatrixOperation_Translation) == ModelMatrixOperation_Translation) {
            model.glm = glm::translate(model.glm, position.glm);
        }
        if ((operations & ModelMatrixOperation_Rotation) == ModelMatrixOperation_Rotation) {
            model.glm = model.glm * glm::eulerAngleZ(glm::radians(eulerRotation.z)) *
                        glm::eulerAngleY(glm::radians(eulerRotation.y)) *
                        glm::eulerAngleX(glm::radians(eulerRotation.x));
        }
        if ((operations & ModelMatrixOperation_Scale) == ModelMatrixOperation_Scale) {
            model.glm = glm::scale(model.glm, scale.glm);
        }
    }

    /// Builds all camera matrices
    static void buildViewProjectionMatrices(XPMat4<float>&       viewProjection,
                                            XPMat4<float>&       inverseViewProjection,
                                            XPMat4<float>&       view,
                                            XPMat4<float>&       inverseView,
                                            XPMat4<float>&       projection,
                                            const XPVec3<float>& position,
                                            const XPVec3<float>& eulerRotation,
                                            const float          fov,
                                            const float          width,
                                            const float          height,
                                            const float          znear,
                                            const float          zfar)
    {
        buildFreeViewMatrix(view, position, eulerRotation);
        buildPerspectiveProjection(projection, fov, width, height, znear, zfar);
        viewProjection.glm        = projection.glm * view.glm;
        inverseView.glm           = glm::inverse(view.glm);
        inverseViewProjection.glm = glm::inverse(viewProjection.glm);
    }

    /// Builds a target view matrix (camera look at a target)
    static void buildTargetViewMatrix(XPMat4<float>& view, const XPVec3<float>& position, const XPVec3<float>& point)
    {
        static const glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        view.glm                  = glm::lookAt(position.glm, point.glm, up);
    }

    /// Builds an orbital view matrix (Camera rotates around a point in 3D)
    static void buildOrbitalViewMatrix(XPMat4<float>&       view,
                                       const XPVec3<float>& position,
                                       const XPVec3<float>& eulerRotation)
    {
        glm::mat4 positionMatrix = glm::translate(position.glm);
        glm::mat4 rotationX      = glm::eulerAngleX(eulerRotation.x);
        glm::mat4 rotationY      = glm::eulerAngleY(eulerRotation.y);
        glm::mat4 rotationZ      = glm::eulerAngleZ(eulerRotation.z);
        glm::mat4 rotationMatrix = glm::transpose(rotationX * rotationY * rotationZ);
        view.glm                 = positionMatrix * rotationMatrix;
    }

  private:
    /// Builds a free (flying) view matrix
    static void buildFreeViewMatrix(XPMat4<float>&       view,
                                    const XPVec3<float>& position,
                                    const XPVec3<float>& eulerRotation)
    {
        glm::quat rotation       = glm::eulerAngleYX(glm::radians(eulerRotation.y), glm::radians(eulerRotation.x));
        glm::mat4 positionMatrix = glm::translate(-position.glm);
        glm::mat4 rotationMatrix = glm::transpose(glm::toMat4(rotation));
        view.glm                 = rotationMatrix * positionMatrix;
    }

    /// Builds a perspective projection matrix
    static void buildPerspectiveProjection(XPMat4<float>& projection,
                                           const float    fov,
                                           const float    width,
                                           const float    height,
                                           const float    znear,
                                           const float    zfar)
    {
        projection.glm = glm::perspective(glm::radians(fov), width / height, znear, zfar);
#if defined(XP_RENDERER_VULKAN)
        projection.glm[1][1] *= -1.0f;
#endif
    }

  public:
    /// Builds an orthographic projection matrix
    static void buildOrthographicProjection(XPMat4<float>& projection,
                                            const float    left,
                                            const float    right,
                                            const float    bottom,
                                            const float    top,
                                            const float    znear,
                                            const float    zfar)
    {
        projection.glm = glm::ortho(left, right, bottom, top, znear, zfar);
    }

    /// Calculates a normalized vec3 ray direction from cursor coordinates on viewport
    static void projectRayFromClipSpacePoint(XPVec3<float>&   ray,
                                             const glm::mat4& inverseViewMatrix,
                                             const glm::mat4& inverseProjectionMatrix,
                                             float            u,
                                             float            v,
                                             float            width,
                                             float            height)
    {
        float     x        = (2.0f * u) / width - 1.0f;
        float     y        = 1.0f - (2.0f * v) / height;
        float     z        = 1.0f;
        glm::vec3 rayNds   = glm::vec3(x, y, z);
        glm::vec4 rayClip  = glm::vec4(rayNds.x, rayNds.y, -1.0f, 1.0f);
        glm::vec4 rayEye   = inverseProjectionMatrix * rayClip;
        rayEye             = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
        glm::vec4 worldEye = inverseViewMatrix * rayEye;
        glm::vec3 rayWorld = glm::vec3(worldEye.x, worldEye.y, worldEye.z);
        ray.glm            = glm::normalize(rayWorld);
    }

    /// Calculates a normalized vec3 ray direction projected from the center of the viewport
    static void projectRayFromClipSpaceCenterPoint(XPVec3<float>& ray, const glm::mat4& inverseViewMatrix)
    {
        ray.glm = -glm::normalize(glm::vec3(inverseViewMatrix[2][0], inverseViewMatrix[2][1], inverseViewMatrix[2][2]));
    }
};
static_assert(sizeof(XPMat4<float>) == sizeof(float) * 4 * 4);

// ----------------------------------------------------------------------------------------------------------------------------------------
// OPERATOR OVERLOADING
// ----------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
std::ostream&
operator<<(std::ostream& out, const XPVec2<T>& v)
{
    out << "[" << v.x << " " << v.y << "]";
    return out;
}
template<typename T>
std::ostream&
operator<<(std::ostream& out, const XPVec3<T>& v)
{
    out << "[" << v.x << " " << v.y << " " << v.z << "]";
    return out;
}
template<typename T>
std::ostream&
operator<<(std::ostream& out, const XPVec4<T>& v)
{
    out << "[" << v.x << " " << v.y << " " << v.z << " " << v.w << "]";
    return out;
}
template<typename T>
std::ostream&
operator<<(std::ostream& out, const XPMat4<T>& m)
{
    out << "[" << std::setw(9) << m._00 << std::setw(9) << m._01 << std::setw(9) << m._02 << std::setw(9) << m._03
        << "]\n";
    out << "[" << std::setw(9) << m._10 << std::setw(9) << m._11 << std::setw(9) << m._12 << std::setw(9) << m._23
        << "]\n";
    out << "[" << std::setw(9) << m._20 << std::setw(9) << m._21 << std::setw(9) << m._22 << std::setw(9) << m._23
        << "]\n";
    out << "[" << std::setw(9) << m._30 << std::setw(9) << m._31 << std::setw(9) << m._32 << std::setw(9) << m._33
        << "]\n";
    return out;
}
template<typename T>
[[nodiscard]]
XPVec2<T>
operator+(const XPVec2<T>& lhs, const XPVec2<T>& rhs)
{
    return { lhs.x + rhs.x, lhs.y + rhs.y };
}
template<typename T>
[[nodiscard]]
XPVec2<T>
operator+(const XPVec2<T>& lhs, const T rhs)
{
    return { lhs.x + rhs, lhs.y + rhs };
}
template<typename T>
[[nodiscard]]
XPVec2<T>
operator+(const T lhs, const XPVec2<T>& rhs)
{
    return { lhs + rhs.x, lhs + rhs.y };
}
template<typename T>
[[nodiscard]]
XPVec2<T>
operator-(const XPVec2<T>& lhs, const XPVec2<T>& rhs)
{
    return { lhs.x - rhs.x, lhs.y - rhs.y };
}
template<typename T>
[[nodiscard]]
XPVec2<T>
operator-(const XPVec2<T>& lhs, const T rhs)
{
    return { lhs.x - rhs, lhs.y - rhs };
}
template<typename T>
[[nodiscard]]
XPVec2<T>
operator-(const T lhs, const XPVec2<T>& rhs)
{
    return { lhs - rhs.x, lhs - rhs.y };
}
template<typename T>
[[nodiscard]]
XPVec2<T>
operator*(const XPVec2<T>& lhs, const XPVec2<T>& rhs)
{
    return { lhs.x * rhs.x, lhs.y * rhs.y };
}
template<typename T>
[[nodiscard]]
XPVec2<T>
operator*(const XPVec2<T>& lhs, const T rhs)
{
    return { lhs.x * rhs, lhs.y * rhs };
}
template<typename T>
[[nodiscard]]
XPVec2<T>
operator*(const T lhs, const XPVec2<T>& rhs)
{
    return { lhs * rhs.x, lhs * rhs.y };
}
template<typename T>
[[nodiscard]]
XPVec2<T>
operator/(const XPVec2<T>& lhs, const XPVec2<T>& rhs)
{
    return { lhs.x / rhs.x, lhs.y / rhs.y };
}
template<typename T>
[[nodiscard]]
XPVec2<T>
operator/(const XPVec2<T>& lhs, const T rhs)
{
    return { lhs.x / rhs, lhs.y / rhs };
}
template<typename T>
[[nodiscard]]
XPVec2<T>
operator/(const T lhs, const XPVec2<T>& rhs)
{
    return { lhs / rhs.x, lhs / rhs.y };
}
template<typename T>
[[nodiscard]]
XPVec3<T>
operator+(const XPVec3<T>& lhs, const XPVec3<T>& rhs)
{
    return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z };
}
template<typename T>
[[nodiscard]]
XPVec3<T>
operator+(const XPVec3<T>& lhs, const T rhs)
{
    return { lhs.x + rhs, lhs.y + rhs, lhs.z + rhs };
}
template<typename T>
[[nodiscard]]
XPVec3<T>
operator+(const T lhs, const XPVec3<T>& rhs)
{
    return { lhs + rhs.x, lhs + rhs.y, lhs + rhs.z };
}
template<typename T>
[[nodiscard]]
XPVec3<T>
operator-(const XPVec3<T>& lhs, const XPVec3<T>& rhs)
{
    return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
}
template<typename T>
[[nodiscard]]
XPVec3<T>
operator-(const XPVec3<T>& lhs, const T rhs)
{
    return { lhs.x - rhs, lhs.y - rhs, lhs.z - rhs };
}
template<typename T>
[[nodiscard]]
XPVec3<T>
operator-(const T lhs, const XPVec3<T>& rhs)
{
    return { lhs - rhs.x, lhs - rhs.y, lhs - rhs.z };
}
template<typename T>
[[nodiscard]]
XPVec3<T>
operator*(const XPVec3<T>& lhs, const XPVec3<T>& rhs)
{
    return { lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z };
}
template<typename T>
[[nodiscard]]
XPVec3<T>
operator*(const XPVec3<T>& lhs, const T rhs)
{
    return { lhs.x * rhs, lhs.y * rhs, lhs.z * rhs };
}
template<typename T>
[[nodiscard]]
XPVec3<T>
operator*(const T lhs, const XPVec3<T>& rhs)
{
    return { lhs * rhs.x, lhs * rhs.y, lhs * rhs.z };
}
template<typename T>
[[nodiscard]]
XPVec3<T>
operator/(const XPVec3<T>& lhs, const XPVec3<T>& rhs)
{
    return { lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z };
}
template<typename T>
[[nodiscard]]
XPVec3<T>
operator/(const XPVec3<T>& lhs, const T rhs)
{
    return { lhs.x / rhs, lhs.y / rhs, lhs.z / rhs };
}
template<typename T>
[[nodiscard]]
XPVec3<T>
operator/(const T lhs, const XPVec3<T>& rhs)
{
    return { lhs / rhs.x, lhs / rhs.y, lhs / rhs.z };
}
template<typename T>
[[nodiscard]]
XPVec4<T>
operator+(const XPVec4<T>& lhs, const XPVec4<T>& rhs)
{
    return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w };
}
template<typename T>
[[nodiscard]]
XPVec4<T>
operator+(const XPVec4<T>& lhs, const T rhs)
{
    return { lhs.x + rhs, lhs.y + rhs, lhs.z + rhs, lhs.w + rhs };
}
template<typename T>
[[nodiscard]]
XPVec4<T>
operator+(const T lhs, const XPVec4<T>& rhs)
{
    return { lhs + rhs.x, lhs + rhs.y, lhs + rhs.z, lhs + rhs.w };
}
template<typename T>
[[nodiscard]]
XPVec4<T>
operator-(const XPVec4<T>& lhs, const XPVec4<T>& rhs)
{
    return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w };
}
template<typename T>
[[nodiscard]]
XPVec4<T>
operator-(const XPVec4<T>& lhs, const T rhs)
{
    return { lhs.x - rhs, lhs.y - rhs, lhs.z - rhs, lhs.w - rhs };
}
template<typename T>
[[nodiscard]]
XPVec4<T>
operator-(const T lhs, const XPVec4<T>& rhs)
{
    return { lhs - rhs.x, lhs - rhs.y, lhs - rhs.z, lhs - rhs.w };
}
template<typename T>
[[nodiscard]]
XPVec4<T>
operator*(const XPVec4<T>& lhs, const XPVec4<T>& rhs)
{
    return { lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w };
}
template<typename T>
[[nodiscard]]
XPVec4<T>
operator*(const XPVec4<T>& lhs, const T rhs)
{
    return { lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs };
}
template<typename T>
[[nodiscard]]
XPVec4<T>
operator*(const T lhs, const XPVec4<T>& rhs)
{
    return { lhs * rhs.x, lhs * rhs.y, lhs * rhs.z, lhs * rhs.w };
}
template<typename T>
[[nodiscard]]
XPVec4<T>
operator/(const XPVec4<T>& lhs, const XPVec4<T>& rhs)
{
    return { lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z, lhs.w / rhs.w };
}
template<typename T>
[[nodiscard]]
XPVec4<T>
operator/(const XPVec4<T>& lhs, const T rhs)
{
    return { lhs.x / rhs, lhs.y / rhs, lhs.z / rhs, lhs.w / rhs };
}
template<typename T>
[[nodiscard]]
XPVec4<T>
operator/(const T lhs, const XPVec4<T>& rhs)
{
    return { lhs / rhs.x, lhs / rhs.y, lhs / rhs.z, lhs / rhs.w };
}
template<typename T>
[[nodiscard]] XPMat4<T>
operator*(const XPMat4<T>& lhs, const XPMat4<T>& rhs)
{
    XPMat4<T> result;
    result.glm = lhs.glm * rhs.glm;
    return result;
}
template<typename T>
[[nodiscard]] XPVec4<T>
operator*(const XPMat4<T>& lhs, const XPVec4<T>& rhs)
{
    XPVec4<T> result;
    result.glm = lhs.glm * rhs.glm;
    return result;
}
template<typename T>
[[nodiscard]] XPVec4<T>
operator*(const XPMat4<T>& lhs, const XPVec3<T>& rhs)
{
    XPVec4<T> nrhs = { rhs.x, rhs.y, rhs.z, 1.0f };
    return lhs * nrhs;
}
// ----------------------------------------------------------------------------------------------------------------------------------------

/// A line from start to end point
struct XPLine
{
    XPLine(XPVec4<float> start, XPVec4<float> end)
      : start(start)
      , end(end)
    {
    }

    XPVec4<float> start;
    XPVec4<float> end;
};

/// A plane cuts three-dimensional space into two separate parts.
/// These parts are called halfspaces.
/// The halfspace the plane’s normals vector points into is called the positive halfspace, and the other halfspace is
/// called the negative halfspace.
///
///                             |
///                             |
///                             |
///                             |    Plane Normal
///    [negative halfspace]     |--------------------->      [positive halfspace]
///                             |
///                             |
///                             |
///                             |
///                             |
///                           Plane
///
///
enum XPHalfSpace_
{
    XPHalfSpace_Negative = -1,
    XPHalfSpace_OnPlane  = 0,
    XPHalfSpace_Positive = 1
};

/// A plane representing the plane equation denoted as: ax + by + cz + d = 0
struct XPPlane
{
    XPHalfSpace_ classifyPoint(const XPVec3<float>& point)
    {
        float d = distance(point);
        if (d < 0) return XPHalfSpace_Negative;
        if (d > 0) return XPHalfSpace_Positive;
        return XPHalfSpace_OnPlane;
    }

    float distance(const XPVec3<float>& point) { return a * point.x + b * point.y + c * point.z; }

    float a;
    float b;
    float c;
    float d;
};

struct XPBoundingBox
{
    XPBoundingBox()
      : minPoint(XPVec4<float>(-1.0f, -1.0f, -1.0f, 1.0f))
      , maxPoint(XPVec4<float>(1.0f, 1.0f, 1.0f, 1.0f))
    {
        // buildCornersFromLocalBounds();
    }

    XPBoundingBox(const XPVec4<float>& minPoint, const XPVec4<float>& maxPoint)
      : minPoint(minPoint)
      , maxPoint(maxPoint)
    {
        // buildCornersFromLocalBounds();
    }

    XPBoundingBox(const XPBoundingBox& other)
    {
        minPoint = other.minPoint;
        maxPoint = other.maxPoint;
        // buildCornersFromLocalBounds();
    }

    XPBoundingBox& operator=(const XPBoundingBox& rhs)
    {
        if (this == &rhs) { return *this; }
        minPoint = rhs.minPoint;
        maxPoint = rhs.maxPoint;
        // buildCornersFromLocalBounds();
        return *this;
    }

    //    void buildCornersFromLocalBounds()
    //    {
    //        worldCorners[0].glm  = minPoint.glm;
    //        worldCorners[7].glm  = minPoint.glm;
    //        worldCorners[16].glm = minPoint.glm;
    //        worldCorners[11].glm = maxPoint.glm;
    //        worldCorners[12].glm = maxPoint.glm;
    //        worldCorners[21].glm = maxPoint.glm;
    //
    //        XPVec4<float> a(XPVec4<float>(worldCorners[11].x, worldCorners[0].y, worldCorners[0].z, 1.0));
    //        XPVec4<float> b(XPVec4<float>(worldCorners[0].x, worldCorners[11].y, worldCorners[0].z, 1.0));
    //        XPVec4<float> c(XPVec4<float>(worldCorners[11].x, worldCorners[11].y, worldCorners[0].z, 1.0));
    //        XPVec4<float> d(XPVec4<float>(worldCorners[0].x, worldCorners[0].y, worldCorners[11].z, 1.0));
    //        XPVec4<float> e(XPVec4<float>(worldCorners[11].x, worldCorners[0].y, worldCorners[11].z, 1.0));
    //        XPVec4<float> f(XPVec4<float>(worldCorners[0].x, worldCorners[11].y, worldCorners[11].z, 1.0));
    //
    //        worldCorners[5]  = a;
    //        worldCorners[6]  = a;
    //        worldCorners[22] = a;
    //        worldCorners[1]  = b;
    //        worldCorners[2]  = b;
    //        worldCorners[18] = b;
    //        worldCorners[3]  = c;
    //        worldCorners[4]  = c;
    //        worldCorners[20] = c;
    //        worldCorners[8]  = d;
    //        worldCorners[15] = d;
    //        worldCorners[17] = d;
    //        worldCorners[13] = e;
    //        worldCorners[14] = e;
    //        worldCorners[23] = e;
    //        worldCorners[9]  = f;
    //        worldCorners[10] = f;
    //        worldCorners[19] = f;
    //    }

    // ---------------------------------------------------------------------------------------
    // Now done on the GPU, compute shader called BoundingBoxTransformCompute
    // ---------------------------------------------------------------------------------------
    //    void transform()
    //    {
    //        // clang-format off
    //            ///                d x-----------x e
    //            ///      min x------------x a    |
    //            ///          |       |    |      |
    //            ///          |    f  x----|------x max
    //            ///       b  x------------x c
    //            ///
    //            /// COUNTER CLOCK-WISE
    //            ///  min|b, b|c, c|a, a|min, d|f, f|max, max|e, e|d, min|d, b|f, c|max, a|e
    //            /// return {
    //            ///     minPoint, b,
    //            ///     b, c,
    //            ///     c, a,
    //            ///     a, minPoint,
    //            ///     d, f,
    //            ///     f, maxPoint,
    //            ///     maxPoint, e,
    //            ///     e, d,
    //            ///     minPoint, d,
    //            ///     b, f,
    //            ///     c, maxPoint,
    //            ///     a, e
    //            /// };
    //        // clang-format on
    //
    //        //        worldCorners[0].glm  = modelMatrix.glm * minPoint.glm;
    //        //        worldCorners[11].glm = modelMatrix.glm * maxPoint.glm;
    //
    //        worldCorners[7].glm  = worldCorners[0].glm;
    //        worldCorners[16].glm = worldCorners[0].glm;
    //        worldCorners[12].glm = worldCorners[11].glm;
    //        worldCorners[21].glm = worldCorners[11].glm;
    //
    //        XPVec4<float> a(XPVec4<float>(worldCorners[11].x, worldCorners[0].y, worldCorners[0].z, 1.0));
    //        XPVec4<float> b(XPVec4<float>(worldCorners[0].x, worldCorners[11].y, worldCorners[0].z, 1.0));
    //        XPVec4<float> c(XPVec4<float>(worldCorners[11].x, worldCorners[11].y, worldCorners[0].z, 1.0));
    //        XPVec4<float> d(XPVec4<float>(worldCorners[0].x, worldCorners[0].y, worldCorners[11].z, 1.0));
    //        XPVec4<float> e(XPVec4<float>(worldCorners[11].x, worldCorners[0].y, worldCorners[11].z, 1.0));
    //        XPVec4<float> f(XPVec4<float>(worldCorners[0].x, worldCorners[11].y, worldCorners[11].z, 1.0));
    //
    //        worldCorners[5]  = a;
    //        worldCorners[6]  = a;
    //        worldCorners[22] = a;
    //        worldCorners[1]  = b;
    //        worldCorners[2]  = b;
    //        worldCorners[18] = b;
    //        worldCorners[3]  = c;
    //        worldCorners[4]  = c;
    //        worldCorners[20] = c;
    //        worldCorners[8]  = d;
    //        worldCorners[15] = d;
    //        worldCorners[17] = d;
    //        worldCorners[13] = e;
    //        worldCorners[14] = e;
    //        worldCorners[23] = e;
    //        worldCorners[9]  = f;
    //        worldCorners[10] = f;
    //        worldCorners[19] = f;
    //    }

    const std::array<XPVec4<float>, 24>& getLinesPoints() const { return worldCorners; }

    std::array<XPVec4<float>, 24> worldCorners;
    XPVec4<float>                 minPoint;
    XPVec4<float>                 maxPoint;
};

// /// A Frustum
// struct XPFrustum
// {
//     enum Planes
//     {
//         Left = 0,
//         Right,
//         Bottom,
//         Top,
//         ZNear,
//         ZFar,

//         Count,
//     };

//     // http://iquilezles.org/www/articles/frustumcorrect/frustumcorrect.htm
//     inline bool IsBoxVisible(const glm::vec3& minp, const glm::vec3& maxp) const
//     {
//         // check box outside/inside of frustum
//         for (int i = 0; i < Count; i++) {
//             if ((glm::dot(_planes[i].glm, glm::vec4(minp.x, minp.y, minp.z, 1.0f)) < 0.0) &&
//                 (glm::dot(_planes[i].glm, glm::vec4(maxp.x, minp.y, minp.z, 1.0f)) < 0.0) &&
//                 (glm::dot(_planes[i].glm, glm::vec4(minp.x, maxp.y, minp.z, 1.0f)) < 0.0) &&
//                 (glm::dot(_planes[i].glm, glm::vec4(maxp.x, maxp.y, minp.z, 1.0f)) < 0.0) &&
//                 (glm::dot(_planes[i].glm, glm::vec4(minp.x, minp.y, maxp.z, 1.0f)) < 0.0) &&
//                 (glm::dot(_planes[i].glm, glm::vec4(maxp.x, minp.y, maxp.z, 1.0f)) < 0.0) &&
//                 (glm::dot(_planes[i].glm, glm::vec4(minp.x, maxp.y, maxp.z, 1.0f)) < 0.0) &&
//                 (glm::dot(_planes[i].glm, glm::vec4(maxp.x, maxp.y, maxp.z, 1.0f)) < 0.0)) {
//                 return false;
//             }
//         }

//         // check frustum outside/inside box
//         int out;
//         out = 0;
//         for (int i = 0; i < 8; i++) out += ((_points[i].x > maxp.x) ? 1 : 0);
//         if (out == 8) return false;
//         out = 0;
//         for (int i = 0; i < 8; i++) out += ((_points[i].x < minp.x) ? 1 : 0);
//         if (out == 8) return false;
//         out = 0;
//         for (int i = 0; i < 8; i++) out += ((_points[i].y > maxp.y) ? 1 : 0);
//         if (out == 8) return false;
//         out = 0;
//         for (int i = 0; i < 8; i++) out += ((_points[i].y < minp.y) ? 1 : 0);
//         if (out == 8) return false;
//         out = 0;
//         for (int i = 0; i < 8; i++) out += ((_points[i].z > maxp.z) ? 1 : 0);
//         if (out == 8) return false;
//         out = 0;
//         for (int i = 0; i < 8; i++) out += ((_points[i].z < minp.z) ? 1 : 0);
//         if (out == 8) return false;

//         return true;
//     }

//     XPFrustum(float                fov,
//               float                aspectRatio,
//               float                znear,
//               float                zfar,
//               const XPMat4<float>& viewProjectionMatrix,
//               const XPMat4<float>& viewMatrix,
//               const XPMat4<float>& inverseViewMatrix,
//               const XPMat4<float>& projectionMatrix,
//               const XPMat4<float>& inverseProjectionMatrix)
//     {
//         reset(fov,
//               aspectRatio,
//               znear,
//               zfar,
//               viewProjectionMatrix,
//               viewMatrix,
//               inverseViewMatrix,
//               projectionMatrix,
//               inverseProjectionMatrix);
//     }

//     XPFrustum(const XPFrustum&)                  = delete;
//     XPFrustum  operator=(XPFrustum other)        = delete;
//     XPFrustum& operator=(const XPFrustum& other) = delete;

//     void reset(float                fov,
//                float                aspectRatio,
//                float                znear,
//                float                zfar,
//                const XPMat4<float>& viewProjectionMatrix,
//                const XPMat4<float>& viewMatrix,
//                const XPMat4<float>& inverseViewMatrix,
//                const XPMat4<float>& projectionMatrix,
//                const XPMat4<float>& inverseProjectionMatrix)
//     {
//         _fov                     = fov;
//         _aspectRatio             = aspectRatio;
//         _znear                   = znear;
//         _zfar                    = zfar;
//         _viewProjectionMatrix    = viewProjectionMatrix;
//         _viewMatrix              = viewMatrix;
//         _inverseViewMatrix       = inverseViewMatrix;
//         _projectionMatrix        = projectionMatrix;
//         _inverseProjectionMatrix = inverseProjectionMatrix;

//         glm::mat4 m     = glm::transpose(viewProjectionMatrix.glm);
//         _planes[Left]   = m[3] + m[0];
//         _planes[Right]  = m[3] - m[0];
//         _planes[Bottom] = m[3] + m[1];
//         _planes[Top]    = m[3] - m[1];
//         _planes[ZNear]  = m[3] + m[2];
//         _planes[ZFar]   = m[3] - m[2];

//         std::array<glm::vec4, 8> csCoords = {
//             // clang-format off
//             glm::vec4{ -1.0f, -1.0f, -1.0f, 1.0f },
//             glm::vec4{ -1.0f, 1.0f, -1.0f, 1.0f },
//             glm::vec4{ 1.0f, 1.0f, -1.0f, 1.0f },
//             glm::vec4{ 1.0f, -1.0f, -1.0f, 1.0f },

//             glm::vec4{ -1.0f, -1.0f, 1.0f, 1.0f },
//             glm::vec4{ -1.0f, 1.0f, 1.0f, 1.0f },
//             glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f },
//             glm::vec4{ 1.0f, -1.0f, 1.0f, 1.0f }
//             // clang-format on
//         };

//         glm::mat4 inverseViewProjection = glm::inverse(_viewProjectionMatrix.glm);

//         for (size_t i = 0; i < csCoords.size(); ++i) {
//             glm::vec4 corner = inverseViewProjection * csCoords[i];
//             _points[i].x     = corner.x / corner.w;
//             _points[i].y     = corner.y / corner.w;
//             _points[i].z     = corner.z / corner.w;
//         }

//         _corners = { _points[1], _points[0], _points[0], _points[3], _points[3], _points[2], _points[2], _points[1],
//                      _points[5], _points[4], _points[4], _points[7], _points[7], _points[6], _points[6], _points[5],
//                      _points[1], _points[5], _points[0], _points[4], _points[2], _points[6], _points[3], _points[7]
//                      };
//     }

//     bool isBoundingBoxInside(const XPBoundingBox& boundingBox) const
//     {
//         //                d x-----------x e
//         //      min x------------x a    |
//         //          |       |    |      |
//         //          |    f  x----|------x max
//         //       b  x------------x c
//         //
//         //
//         const auto& worldCorners = boundingBox.getLinesPoints();
//         return IsBoxVisible(worldCorners[0].glm, worldCorners[11].glm);
//     }

//     std::array<XPVec3<float>, 8>& getPoints() { return _points; }

//     std::array<XPVec3<float>, 24>& getCorners()
//     {
//         // clang-format off

//         ///
//         ///                 5             6              (-1, 1, 1)          (1, 1, 1)
//         ///                 x-----------x                          x-----------x
//         ///                /|           |                         /|           |
//         ///               / |          /|                        / |          /|
//         ///              /  |         / |                       /  |         / |
//         ///             /   |        /  |                      /   |        /  |
//         ///            /  4 x-------/---x 7                   /  f x-------/---x (1, -1, 1)
//         ///           /            /   /                     /            /   /
//         ///        1 x------------x 2 /          (-1, 1, 0) x------------x a /
//         ///          |            |  /                      |            |  /
//         ///          |            | /                       |            | /
//         ///          x------------x                         x------------x
//         ///       0               3              (-1, -1, 0)             (1, -1, 0)
//         /// COUNTER CLOCK-WISE
//         // clang-format on

//         return _corners;
//     }

//     std::array<XPVec4<float>, Planes::Count>& getPlanes() { return _planes; }
//     const XPMat4<float>&                      getViewProjectionMatrix() const { return _viewProjectionMatrix; }
//     const XPMat4<float>&                      getViewMatrix() const { return _viewMatrix; }
//     const XPMat4<float>&                      getInverseViewMatrix() const { return _inverseViewMatrix; }
//     const XPMat4<float>&                      getProjectionMatrix() const { return _projectionMatrix; }
//     const XPMat4<float>&                      getInverseProjectionMatrix() const { return _inverseProjectionMatrix; }

//   private:
//     std::array<XPVec4<float>, Planes::Count> _planes;
//     std::array<XPVec3<float>, 8>             _points;
//     std::array<XPVec3<float>, 24>            _corners;
//     XPMat4<float>                            _viewProjectionMatrix;
//     XPMat4<float>                            _viewMatrix;
//     XPMat4<float>                            _inverseViewMatrix;
//     XPMat4<float>                            _projectionMatrix;
//     XPMat4<float>                            _inverseProjectionMatrix;
//     float                                    _fov;
//     float                                    _aspectRatio;
//     float                                    _znear;
//     float                                    _zfar;
// };

static_assert(sizeof(XPVec2<float>) == (sizeof(float) * 2));
static_assert(sizeof(XPVec3<float>) == (sizeof(float) * 3));
static_assert(sizeof(XPVec4<float>) == (sizeof(float) * 4));
static_assert(sizeof(XPMat4<float>) == (sizeof(float) * 16));