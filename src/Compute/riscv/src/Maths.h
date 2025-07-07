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

#include <vector>

#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wall"
    #pragma clang diagnostic ignored "-Weverything"
#endif

#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>

#ifdef __clang__
    #pragma clang diagnostic pop
#endif

template<typename T>
struct Vec2
{
    Vec2()
      : x(0)
      , y(0)
    {
    }
    Vec2(T x, T y)
      : x(x)
      , y(y)
    {
    }
    Vec2(const Vec2<T>& other) = default;
    Vec2(Vec2<T>&& other)      = default;
    Vec2(const glm::vec<2, T, glm::defaultp>& other)
      : glm(other)
    {
    }
    union
    {
        struct
        {
            T arr[2];
        };
        struct
        {
            T x, y;
        };
        struct
        {
            glm::vec<2, T, glm::defaultp> glm;
        };
    };

    [[nodiscard]] T&       operator[](size_t index) { return arr[index]; }
    [[nodiscard]] const T& operator[](size_t index) const { return arr[index]; }
    bool                   operator==(const Vec2<T>& other) const { return x == other.x && y == other.y; }
    Vec2<T>&               operator=(const Vec2<T>& other)
    {
        if (this == &other) { return *this; }
        x = other.x;
        y = other.y;
        return *this;
    }
    Vec2<T>& operator=(const glm::vec<2, T, glm::defaultp>& other)
    {
        if (&this->glm == &other) { return *this; }
        x = other.x;
        y = other.y;
        return *this;
    }
    [[nodiscard]] T dot(const Vec2<T>& other) const { return glm::dot(glm, other.glm); }
};
static_assert(sizeof(Vec2<float>) == sizeof(float) * 2);

template<typename T>
struct Vec3
{
    Vec3()
      : x(0)
      , y(0)
      , z(0)
    {
    }
    Vec3(T x, T y, T z)
      : x(x)
      , y(y)
      , z(z)
    {
    }
    Vec3(const Vec3<T>& other) = default;
    Vec3(Vec3<T>&& other)      = default;
    Vec3(const glm::vec<3, T, glm::defaultp>& other)
      : glm(other)
    {
    }
    union
    {
        struct
        {
            T arr[3];
        };
        struct
        {
            T x, y, z;
        };
        struct
        {
            glm::vec<3, T, glm::defaultp> glm;
        };
        struct
        {
            Vec2<T> xy;
            T       _z;
        };
        struct
        {
            T       _x;
            Vec2<T> yz;
        };
    };

    [[nodiscard]] T&       operator[](size_t index) { return arr[index]; }
    [[nodiscard]] const T& operator[](size_t index) const { return arr[index]; }
    bool     operator==(const Vec3<T>& other) const { return x == other.x && y == other.y && z == other.z; }
    Vec3<T>& operator=(const Vec3<T>& other)
    {
        if (this == &other) { return *this; }
        x = other.x;
        y = other.y;
        z = other.z;
        return *this;
    }
    Vec3<T>& operator=(const glm::vec<3, T, glm::defaultp>& other)
    {
        if (&this->glm == &other) { return *this; }
        x = other.x;
        y = other.y;
        z = other.z;
        return *this;
    }
    void                  normalize() { this->glm = glm::normalize(this->glm); }
    [[nodiscard]] Vec3<T> cross(const Vec3<T>& other) const
    {
        Vec3<T> result;
        result.glm = glm::cross<T, glm::defaultp>(this->glm, other.glm);
        return result;
    }
    [[nodiscard]] T dot(const Vec3<T>& other) const { return glm::dot(glm, other.glm); }
    [[nodiscard]] T distanceTo(const Vec3<T>& other) const
    {
        return glm::distance<3, T, glm::defaultp>(this->glm, other.glm);
    }
};
static_assert(sizeof(Vec3<float>) == sizeof(float) * 3);

template<typename T>
struct Vec4
{
    Vec4()
      : x(0)
      , y(0)
      , z(0)
      , w(0)
    {
    }
    Vec4(T x, T y, T z, T w)
      : x(x)
      , y(y)
      , z(z)
      , w(w)
    {
    }
    Vec4(const Vec4<T>& other) = default;
    Vec4(Vec4<T>&& other)      = default;
    Vec4(const glm::vec<4, T, glm::defaultp>& other)
      : glm(other)
    {
    }
    union
    {
        struct
        {
            T arr[4];
        };
        struct
        {
            T x, y, z, w;
        };
        struct
        {
            glm::vec<4, T, glm::defaultp> glm;
        };
        struct
        {
            Vec2<T> xy, zw;
        };
        struct
        {
            Vec3<T> xyz;
            T       _w;
        };
        struct
        {
            T       _x;
            Vec3<T> yzw;
        };
    };

    [[nodiscard]] T&       operator[](size_t index) { return arr[index]; }
    [[nodiscard]] const T& operator[](size_t index) const { return arr[index]; }
    bool operator==(const Vec4<T>& other) const { return x == other.x && y == other.y && z == other.z && w == other.w; }
    Vec4<T>& operator=(const Vec4<T>& other)
    {
        if (this == &other) { return *this; }
        x = other.x;
        y = other.y;
        z = other.z;
        w = other.w;
        return *this;
    }
    Vec4<T>& operator=(const glm::vec<4, T, glm::defaultp>& other)
    {
        if (&this->glm == &other) { return *this; }
        x = other.x;
        y = other.y;
        z = other.z;
        w = other.w;
        return *this;
    }
    [[nodiscard]] T dot(const Vec4<T>& other) const { return glm::dot(glm, other.glm); }
};
static_assert(sizeof(Vec4<float>) == sizeof(float) * 4);

template<typename T>
struct Mat4
{
    Mat4()
      : row0(Vec4<T>{ 1, 0, 0, 0 })
      , row1(Vec4<T>{ 0, 1, 0, 0 })
      , row2(Vec4<T>{ 0, 0, 1, 0 })
      , row3(Vec4<T>{ 0, 0, 0, 1 })
    {
    }
    Mat4(T _00, T _01, T _02, T _03, T _10, T _11, T _12, T _13, T _20, T _21, T _22, T _23, T _30, T _31, T _32, T _33)
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
    Mat4(const Mat4<T>& other) = default;
    Mat4(Mat4<T>&& other)      = default;
    Mat4(const glm::mat<4, 4, T, glm::defaultp>& other)
      : glm(other)
    {
    }
    union
    {
        struct
        {
            T arr[16];
        };
        struct
        {
            T _00, _01, _02, _03;
            T _10, _11, _12, _13;
            T _20, _21, _22, _23;
            T _30, _31, _32, _33;
        };
        struct
        {
            Vec4<T> row0;
            Vec4<T> row1;
            Vec4<T> row2;
            Vec4<T> row3;
        };
        struct
        {
            glm::mat<4, 4, T, glm::defaultp> glm;
        };
    };

    [[nodiscard]] static Mat4 identity() { return Mat4{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 }; }
    [[nodiscard]] T&          operator[](const size_t index) { return arr[index]; }
    [[nodiscard]] const T&    operator[](const size_t index) const { return arr[index]; }
    bool                      operator==(const Mat4<T>& other) const
    {
        return _00 == other._00 && _01 == other._01 && _02 == other._02 && _03 == other._03 && _10 == other._10 &&
               _11 == other._11 && _12 == other._12 && _13 == other._13 && _20 == other._20 && _21 == other._21 &&
               _22 == other._22 && _23 == other._23 && _30 == other._30 && _31 == other._31 && _32 == other._32 &&
               _33 == other._33;
    }
    Mat4<T>& operator=(const Mat4<T>& other)
    {
        if (this == &other) { return *this; }
        _00 = other._00;
        _01 = other._01;
        _02 = other._02;
        _03 = other._03;

        _10 = other._10;
        _11 = other._11;
        _12 = other._12;
        _13 = other._13;

        _20 = other._20;
        _21 = other._21;
        _22 = other._22;
        _23 = other._23;

        _30 = other._30;
        _31 = other._31;
        _32 = other._32;
        _33 = other._33;
        return *this;
    }
    Mat4<T>& operator=(const glm::mat<4, 4, T, glm::defaultp>& other)
    {
        if (&this->glm == &other) { return *this; }
        _00 = other[0][0];
        _01 = other[0][1];
        _02 = other[0][2];
        _03 = other[0][3];

        _10 = other[1][0];
        _11 = other[1][1];
        _12 = other[1][2];
        _13 = other[1][3];

        _20 = other[2][0];
        _21 = other[2][1];
        _22 = other[2][2];
        _23 = other[2][3];

        _30 = other[3][0];
        _31 = other[3][1];
        _32 = other[3][2];
        _33 = other[3][3];
        return *this;
    }
    [[nodiscard]] Mat4 operator*(const Mat4& other) const
    {
        Mat4 result;
        result.glm = this->glm * other.glm;
        return result;
    }
    [[nodiscard]] Vec4<T> operator*(const Vec4<T>& other) const
    {
        Vec4<T> result;
        result.glm = this->glm * other.glm;
        return result;
    }
    [[nodiscard]] Vec4<T> operator*(const Vec3<T>& other) const
    {
        Vec4<T> otherV4 = { other.x, other.y, other.z, 1.0f };
        return operator*(otherV4);
    }
    [[nodiscard]] Mat4<T> transpose() const
    {
        Mat4<T> result;
        result.glm = glm::transpose<4, 4, T, glm::defaultp>(this->glm);
        return result;
    }
    [[nodiscard]] bool inverseMatrix4x4(Mat4<T>& result) const
    {
        result.glm = glm::inverse<4, 4, T, glm::defaultp>(this->glm);
        return true;
    }
};
static_assert(sizeof(Mat4<float>) == sizeof(float) * 16);

// ----------------------------------------------------------------------------------------------------------------------------------------
// OPERATOR OVERLOADING
// ----------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
[[nodiscard]]
Vec2<T>
operator+(const Vec2<T>& lhs, const Vec2<T>& rhs)
{
    return { lhs.x + rhs.x, lhs.y + rhs.y };
}
template<typename T>
[[nodiscard]]
Vec2<T>
operator+(const Vec2<T>& lhs, const T rhs)
{
    return { lhs.x + rhs, lhs.y + rhs };
}
template<typename T>
[[nodiscard]]
Vec2<T>
operator+(const T lhs, const Vec2<T>& rhs)
{
    return { lhs + rhs.x, lhs + rhs.y };
}
template<typename T>
[[nodiscard]]
Vec2<T>
operator-(const Vec2<T>& lhs, const Vec2<T>& rhs)
{
    return { lhs.x - rhs.x, lhs.y - rhs.y };
}
template<typename T>
[[nodiscard]]
Vec2<T>
operator-(const Vec2<T>& lhs, const T rhs)
{
    return { lhs.x - rhs, lhs.y - rhs };
}
template<typename T>
[[nodiscard]]
Vec2<T>
operator-(const T lhs, const Vec2<T>& rhs)
{
    return { lhs - rhs.x, lhs - rhs.y };
}
template<typename T>
[[nodiscard]]
Vec2<T>
operator*(const Vec2<T>& lhs, const Vec2<T>& rhs)
{
    return { lhs.x * rhs.x, lhs.y * rhs.y };
}
template<typename T>
[[nodiscard]]
Vec2<T>
operator*(const Vec2<T>& lhs, const T rhs)
{
    return { lhs.x * rhs, lhs.y * rhs };
}
template<typename T>
[[nodiscard]]
Vec2<T>
operator*(const T lhs, const Vec2<T>& rhs)
{
    return { lhs * rhs.x, lhs * rhs.y };
}
template<typename T>
[[nodiscard]]
Vec2<T>
operator/(const Vec2<T>& lhs, const Vec2<T>& rhs)
{
    return { lhs.x / rhs.x, lhs.y / rhs.y };
}
template<typename T>
[[nodiscard]]
Vec2<T>
operator/(const Vec2<T>& lhs, const T rhs)
{
    return { lhs.x / rhs, lhs.y / rhs };
}
template<typename T>
[[nodiscard]]
Vec2<T>
operator/(const T lhs, const Vec2<T>& rhs)
{
    return { lhs / rhs.x, lhs / rhs.y };
}
template<typename T>
[[nodiscard]]
Vec3<T>
operator+(const Vec3<T>& lhs, const Vec3<T>& rhs)
{
    return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z };
}
template<typename T>
[[nodiscard]]
Vec3<T>
operator+(const Vec3<T>& lhs, const T rhs)
{
    return { lhs.x + rhs, lhs.y + rhs, lhs.z + rhs };
}
template<typename T>
[[nodiscard]]
Vec3<T>
operator+(const T lhs, const Vec3<T>& rhs)
{
    return { lhs + rhs.x, lhs + rhs.y, lhs + rhs.z };
}
template<typename T>
[[nodiscard]]
Vec3<T>
operator-(const Vec3<T>& lhs, const Vec3<T>& rhs)
{
    return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
}
template<typename T>
[[nodiscard]]
Vec3<T>
operator-(const Vec3<T>& lhs, const T rhs)
{
    return { lhs.x - rhs, lhs.y - rhs, lhs.z - rhs };
}
template<typename T>
[[nodiscard]]
Vec3<T>
operator-(const T lhs, const Vec3<T>& rhs)
{
    return { lhs - rhs.x, lhs - rhs.y, lhs - rhs.z };
}
template<typename T>
[[nodiscard]]
Vec3<T>
operator*(const Vec3<T>& lhs, const Vec3<T>& rhs)
{
    return { lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z };
}
template<typename T>
[[nodiscard]]
Vec3<T>
operator*(const Vec3<T>& lhs, const T rhs)
{
    return { lhs.x * rhs, lhs.y * rhs, lhs.z * rhs };
}
template<typename T>
[[nodiscard]]
Vec3<T>
operator*(const T lhs, const Vec3<T>& rhs)
{
    return { lhs * rhs.x, lhs * rhs.y, lhs * rhs.z };
}
template<typename T>
[[nodiscard]]
Vec3<T>
operator/(const Vec3<T>& lhs, const Vec3<T>& rhs)
{
    return { lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z };
}
template<typename T>
[[nodiscard]]
Vec3<T>
operator/(const Vec3<T>& lhs, const T rhs)
{
    return { lhs.x / rhs, lhs.y / rhs, lhs.z / rhs };
}
template<typename T>
[[nodiscard]]
Vec3<T>
operator/(const T lhs, const Vec3<T>& rhs)
{
    return { lhs / rhs.x, lhs / rhs.y, lhs / rhs.z };
}
template<typename T>
[[nodiscard]]
Vec4<T>
operator+(const Vec4<T>& lhs, const Vec4<T>& rhs)
{
    return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w };
}
template<typename T>
[[nodiscard]]
Vec4<T>
operator+(const Vec4<T>& lhs, const T rhs)
{
    return { lhs.x + rhs, lhs.y + rhs, lhs.z + rhs, lhs.w + rhs };
}
template<typename T>
[[nodiscard]]
Vec4<T>
operator+(const T lhs, const Vec4<T>& rhs)
{
    return { lhs + rhs.x, lhs + rhs.y, lhs + rhs.z, lhs + rhs.w };
}
template<typename T>
[[nodiscard]]
Vec4<T>
operator-(const Vec4<T>& lhs, const Vec4<T>& rhs)
{
    return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w };
}
template<typename T>
[[nodiscard]]
Vec4<T>
operator-(const Vec4<T>& lhs, const T rhs)
{
    return { lhs.x - rhs, lhs.y - rhs, lhs.z - rhs, lhs.w - rhs };
}
template<typename T>
[[nodiscard]]
Vec4<T>
operator-(const T lhs, const Vec4<T>& rhs)
{
    return { lhs - rhs.x, lhs - rhs.y, lhs - rhs.z, lhs - rhs.w };
}
template<typename T>
[[nodiscard]]
Vec4<T>
operator*(const Vec4<T>& lhs, const Vec4<T>& rhs)
{
    return { lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w };
}
template<typename T>
[[nodiscard]]
Vec4<T>
operator*(const Vec4<T>& lhs, const T rhs)
{
    return { lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs };
}
template<typename T>
[[nodiscard]]
Vec4<T>
operator*(const T lhs, const Vec4<T>& rhs)
{
    return { lhs * rhs.x, lhs * rhs.y, lhs * rhs.z, lhs * rhs.w };
}
template<typename T>
[[nodiscard]]
Vec4<T>
operator/(const Vec4<T>& lhs, const Vec4<T>& rhs)
{
    return { lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z, lhs.w / rhs.w };
}
template<typename T>
[[nodiscard]]
Vec4<T>
operator/(const Vec4<T>& lhs, const T rhs)
{
    return { lhs.x / rhs, lhs.y / rhs, lhs.z / rhs, lhs.w / rhs };
}
template<typename T>
[[nodiscard]]
Vec4<T>
operator/(const T lhs, const Vec4<T>& rhs)
{
    return { lhs / rhs.x, lhs / rhs.y, lhs / rhs.z, lhs / rhs.w };
}
template<typename T>
[[nodiscard]] Mat4<T>
operator*(const Mat4<T>& lhs, const Mat4<T>& rhs)
{
    Mat4<T> result;
    result.glm = lhs.glm * rhs.glm;
    return result;
}
template<typename T>
[[nodiscard]] Vec4<T>
operator*(const Mat4<T>& lhs, const Vec4<T>& rhs)
{
    Vec4<T> result;
    result.glm = lhs.glm * rhs.glm;
    return result;
}
template<typename T>
[[nodiscard]] Vec4<T>
operator*(const Mat4<T>& lhs, const Vec3<T>& rhs)
{
    Vec4<T> nrhs = { rhs.x, rhs.y, rhs.z, 1.0f };
    return lhs * nrhs;
}

using bvec2   = Vec2<bool>;
using u8vec2  = Vec2<uint8_t>;
using u16vec2 = Vec2<uint16_t>;
using u32vec2 = Vec2<uint32_t>;
using u64vec2 = Vec2<uint64_t>;
using fvec2   = Vec2<float>;
using dvec2   = Vec2<double>;

using bvec3   = Vec3<bool>;
using u8vec3  = Vec3<uint8_t>;
using u16vec3 = Vec3<uint16_t>;
using u32vec3 = Vec3<uint32_t>;
using u64vec3 = Vec3<uint64_t>;
using fvec3   = Vec3<float>;
using dvec3   = Vec3<double>;

using bvec4   = Vec4<bool>;
using u8vec4  = Vec4<uint8_t>;
using u16vec4 = Vec4<uint16_t>;
using u32vec4 = Vec4<uint32_t>;
using u64vec4 = Vec4<uint64_t>;
using fvec4   = Vec4<float>;
using dvec4   = Vec4<double>;

using fmat4 = Mat4<float>;
using dmat4 = Mat4<double>;

struct RGBColor
{
    float r, g, b;
};

inline RGBColor
hsvToRgb(float h, float s, float v)
{
    RGBColor color;
    int      i;
    float    f, p, q, t;

    if (s == 0) {
        // Grayscale
        color.r = color.g = color.b = v;
        return color;
    }

    h /= 60; // Sector 0 to 5
    i = (int)h;
    f = h - i; // Fractional part of h
    p = v * (1 - s);
    q = v * (1 - s * f);
    t = v * (1 - s * (1 - f));

    switch (i) {
        case 0:
            color.r = v;
            color.g = t;
            color.b = p;
            break;
        case 1:
            color.r = q;
            color.g = v;
            color.b = p;
            break;
        case 2:
            color.r = p;
            color.g = v;
            color.b = t;
            break;
        case 3:
            color.r = p;
            color.g = q;
            color.b = v;
            break;
        case 4:
            color.r = t;
            color.g = p;
            color.b = v;
            break;
        default: // case 5:
            color.r = v;
            color.g = p;
            color.b = q;
            break;
    }

    return color;
}

inline RGBColor
generateColor(int objectId, int numObjects)
{
    // Use a hue-based approach to generate colors
    float hue        = (float)objectId / numObjects * 360; // Distribute hues evenly
    float saturation = 0.7f;                               // Fixed saturation
    float value      = 0.9f;                               // Fixed value (brightness)

    // Convert HSV to RGB
    return hsvToRgb(hue, saturation, value);
}
