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

#include <Utilities/XPLogger.h>
#include <Utilities/XPMacros.h>
#include <Utilities/XPPlatforms.h>

#include <assert.h>
#include <stdint.h>
#include <string>

enum XPNodeEditorPinDirection
{
    XPNodeEditorPinDirection_Input,
    XPNodeEditorPinDirection_Output,

    XPNodeEditorPinDirection_Count
};

enum XPNodeEditorPinType
{
    XPNodeEditorPinType_Void = 0,
    XPNodeEditorPinType_Bool,
    XPNodeEditorPinType_Int,
    XPNodeEditorPinType_UInt,
    XPNodeEditorPinType_Float,
    XPNodeEditorPinType_Double,
    XPNodeEditorPinType_BVec2,
    XPNodeEditorPinType_BVec3,
    XPNodeEditorPinType_BVec4,
    XPNodeEditorPinType_IVec2,
    XPNodeEditorPinType_IVec3,
    XPNodeEditorPinType_IVec4,
    XPNodeEditorPinType_UVec2,
    XPNodeEditorPinType_UVec3,
    XPNodeEditorPinType_UVec4,
    XPNodeEditorPinType_Vec2,
    XPNodeEditorPinType_Vec3,
    XPNodeEditorPinType_Vec4,
    XPNodeEditorPinType_DVec2,
    XPNodeEditorPinType_DVec3,
    XPNodeEditorPinType_DVec4,
    XPNodeEditorPinType_Mat2,
    XPNodeEditorPinType_Mat3,
    XPNodeEditorPinType_Mat4,
    XPNodeEditorPinType_Mat2x3,
    XPNodeEditorPinType_Mat2x4,
    XPNodeEditorPinType_Mat3x2,
    XPNodeEditorPinType_Mat3x4,
    XPNodeEditorPinType_Mat4x2,
    XPNodeEditorPinType_Mat4x3,
    XPNodeEditorPinType_DMat2,
    XPNodeEditorPinType_DMat3,
    XPNodeEditorPinType_DMat4,
    XPNodeEditorPinType_DMat2x3,
    XPNodeEditorPinType_DMat2x4,
    XPNodeEditorPinType_DMat3x2,
    XPNodeEditorPinType_DMat3x4,
    XPNodeEditorPinType_DMat4x2,
    XPNodeEditorPinType_DMat4x3,
    XPNodeEditorPinType_Sampler,
    XPNodeEditorPinType_Sampler2D,
    XPNodeEditorPinType_SamplerCube,
    XPNodeEditorPinType_Function
};

struct XPNodeEditorBlock;

struct XPNodeEditorPin
{
    std::string              name;
    XPNodeEditorPinDirection direction;
    XPNodeEditorPinType      type;
    XPNodeEditorBlock*       block;
};

inline const char*
xp_node_editor_pin_type_to_str(XPNodeEditorPinType type)
{
    switch (type) {
        case XPNodeEditorPinType_Bool: return "bool";
        case XPNodeEditorPinType_Int: return "int";
        case XPNodeEditorPinType_UInt: return "uint";
        case XPNodeEditorPinType_Float: return "float";
        case XPNodeEditorPinType_Double: return "double";
        case XPNodeEditorPinType_BVec2: return "bvec2";
        case XPNodeEditorPinType_BVec3: return "bvec3";
        case XPNodeEditorPinType_BVec4: return "bvec4";
        case XPNodeEditorPinType_IVec2: return "ivec2";
        case XPNodeEditorPinType_IVec3: return "ivec3";
        case XPNodeEditorPinType_IVec4: return "ivec4";
        case XPNodeEditorPinType_UVec2: return "uvec2";
        case XPNodeEditorPinType_UVec3: return "uvec3";
        case XPNodeEditorPinType_UVec4: return "uvec4";
        case XPNodeEditorPinType_Vec2: return "vec2";
        case XPNodeEditorPinType_Vec3: return "vec3";
        case XPNodeEditorPinType_Vec4: return "vec4";
        case XPNodeEditorPinType_DVec2: return "dvec2";
        case XPNodeEditorPinType_DVec3: return "dvec3";
        case XPNodeEditorPinType_DVec4: return "dvec4";
        case XPNodeEditorPinType_Mat2: return "mat2";
        case XPNodeEditorPinType_Mat3: return "mat3";
        case XPNodeEditorPinType_Mat4: return "mat4";
        case XPNodeEditorPinType_Mat2x3: return "mat2x3";
        case XPNodeEditorPinType_Mat2x4: return "mat2x4";
        case XPNodeEditorPinType_Mat3x2: return "mat3x2";
        case XPNodeEditorPinType_Mat3x4: return "mat3x4";
        case XPNodeEditorPinType_Mat4x2: return "mat4x2";
        case XPNodeEditorPinType_Mat4x3: return "mat4x3";
        case XPNodeEditorPinType_DMat2: return "dmat2";
        case XPNodeEditorPinType_DMat3: return "dmat3";
        case XPNodeEditorPinType_DMat4: return "dmat4";
        case XPNodeEditorPinType_DMat2x3: return "dmat2x3";
        case XPNodeEditorPinType_DMat2x4: return "dmat2x4";
        case XPNodeEditorPinType_DMat3x2: return "dmat3x2";
        case XPNodeEditorPinType_DMat3x4: return "dmat3x4";
        case XPNodeEditorPinType_DMat4x2: return "dmat4x2";
        case XPNodeEditorPinType_DMat4x3: return "dmat4x3";
    }
    XP_UNIMPLEMENTED("Unreachable");
    return "";
}