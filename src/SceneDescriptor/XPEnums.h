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

#include <Utilities/XPAnnotations.h>
#include <Utilities/XPMacros.h>

#include <string>

enum XPEColliderShape
{
    XPEColliderShapePlane,
    XPEColliderShapeBox,
    XPEColliderShapeSphere,
    XPEColliderShapeCapsule,
    XPEColliderShapeCylinder,
    XPEColliderShapeConvexMesh,
    XPEColliderShapeTriangleMesh,

    XPEColliderShapeCount
};

XP_FOR_EACH_X(XP_STRINGIFY_ENUM_EXTENDED,
              XP_STRINGIFY_ENUM,
              XPEColliderShapePlane,
              XPEColliderShapeBox,
              XPEColliderShapeSphere,
              XPEColliderShapeCapsule,
              XPEColliderShapeCylinder,
              XPEColliderShapeConvexMesh,
              XPEColliderShapeTriangleMesh);

enum XPEMeshRendererPolygonMode
{
    XPEMeshRendererPolygonModePoint,
    XPEMeshRendererPolygonModeLine,
    XPEMeshRendererPolygonModeFill,

    XPEMeshRendererPolygonModeCount
};

XP_FOR_EACH_X(XP_STRINGIFY_ENUM_EXTENDED,
              XP_STRINGIFY_ENUM,
              XPEMeshRendererPolygonModePoint,
              XPEMeshRendererPolygonModeLine,
              XPEMeshRendererPolygonModeFill);

enum XPEHeaderFlags : uint32_t
{
    XPEHeaderFlagsNone      = 0,
    XPEHeaderFlagsTreeOpen  = 1,
    XPEHeaderFlagsTableOpen = 2
};
