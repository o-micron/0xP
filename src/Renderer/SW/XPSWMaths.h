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

#include <Renderer/SW/XPSWThirdParty.h>

#include <Utilities/XPMaths.h>
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

// generate a unique color for an object based on its ID
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

template<typename T>
inline XPMat4<T>
createModelMatrix(const XPVec3<T>& location, const XPVec3<T>& rotation, const XPVec3<T>& scale)
{
    XPMat4<T> result;
    result.glm = glm::mat<4, 4, T, glm::defaultp>(static_cast<T>(1));
    result.glm = glm::translate<T, glm::defaultp>(result.glm, location.glm);
    result.glm = result.glm * glm::eulerAngleZ<T>(glm::radians<T>(rotation.z)) *
                 glm::eulerAngleY<T>(glm::radians<T>(rotation.y)) * glm::eulerAngleX<T>(glm::radians<T>(rotation.x));
    result.glm = glm::scale<T, glm::defaultp>(result.glm, scale.glm);
    return result;
}

template<typename T>
inline XPMat4<T>
createViewMatrix(const XPVec3<T>& location, const XPVec3<T>& target, const XPVec3<T>& up)
{
    XPVec3<T> UP = up;
    UP.normalize();

    XPVec3<T> FWD = target - location;
    FWD.normalize();

    XPVec3<T> RIGHT = FWD.cross(UP);
    RIGHT.normalize();

    // Recalculate the up vector to ensure it's perpendicular to forward and right
    UP = RIGHT.cross(FWD);

    return XPMat4<T>{
        // clang-format off
        RIGHT.x,              UP.x,            -FWD.x,             T(0),
        RIGHT.y,              UP.y,            -FWD.y,             T(0),
        RIGHT.z,              UP.z,            -FWD.z,             T(0),
       -RIGHT.dot(location), -UP.dot(location), FWD.dot(location), T(1)
        // clang-format on
    };
}

template<typename T>
inline XPMat4<T>
createPerspectiveProjectionMatrix(T fov, T width, T height, T near, T far)
{
    fov      = glm::radians(fov);
    T aspect = width / height;
    return XPMat4<T>{
        // clang-format off
        T(1) / (tan(fov * 0.5f) * aspect),  T(0),                           T(0),                       T(0),
        T(0),                               T(1) / tan(fov * 0.5f),         T(0),                       T(0),
        T(0),                               T(0),                   (far + near) / (near - far),        T(-1),
        T(0),                               T(0),                   T(2) * far * near / (near - far),   T(0)
        // clang-format on
    };
    // Mat4<T> result;
    // result.glm = glm::perspectiveFovRH<T>(glm::radians<T>(fov), width, height, near, far);
    // return result;
}

template<typename T>
inline XPMat4<T>
createOrthographicProjectionMatrix(T width, T height, T near, T far)
{
    XPMat4<T> result;
    result.glm =
      glm::orthoRH<T>(static_cast<T>(-1), static_cast<T>(1), static_cast<T>(-1), static_cast<T>(1), near, far);
    return result;
}

template<typename T>
T
LinearizeDepth(T depth, T zNearPlane, T zFarPlane)
{
    return (2.0 * zNearPlane) / (zFarPlane + zNearPlane - depth * (zFarPlane - zNearPlane));
}

template<typename T>
T
LinearToExponentialInvertedZ(T linearZ, T zNearPlane, T zFarPlane)
{
    const T factor      = 1.0; // Adjust this for precision distribution (matches with ExponentialInvertedToLinearZ)
    T       normalizedZ = (linearZ - zNearPlane) / (zFarPlane - zNearPlane);
    return 1.0 - exp(-normalizedZ * factor);
}

template<typename T>
T
ExponentialInvertedToLinearZ(T invExpZ)
{
    const T factor = 1.0; // Adjust this for precision distribution (matches LinearToExponentialInvertedZ)
    invExpZ        = std::min(invExpZ, T(0.999999)); // Avoid log(0)
    return -log(1.0 - invExpZ) / factor;
}

template<typename T>
T
ReverseExponentialZ(T linearZ, T zNearPlane, T zFarPlane)
{
    T z = (linearZ - zNearPlane) / (zFarPlane - zNearPlane);
    return 1.0 - pow(2.0, -10.0 * z);
}