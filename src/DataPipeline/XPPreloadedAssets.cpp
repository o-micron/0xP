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

#include <DataPipeline/XPPreloadedAssets.h>

const std::map<const std::string, XPPreloadedMesh>&
XPPreloadedAssets::meshes()
{
    static const std::map<const std::string, XPPreloadedMesh>
      meshes = { { "TRIANGLE",
                   { .positions   = { XPVec4<float>(0.5, 0.5, 0.0, 0.0),
                                    XPVec4<float>(-0.5, 0.5, 0.0, 0.0),
                                    XPVec4<float>(0.0, -0.5, 0.0, 0.0) },
                     .normals     = { XPVec4<float>(0, 0, -1.0, 0.0),
                                  XPVec4<float>(0, 0, -1.0, 0.0),
                                  XPVec4<float>(0, 0, -1.0, 0.0) },
                     .texcoords   = { XPVec4<float>(1, 0, 0, 0),
                                    XPVec4<float>(0, 0, 0, 0),
                                    XPVec4<float>(0.5, 0.5, 0, 0) },
                     .indices     = { 0, 1, 2 },
                     .boundingBox = XPBoundingBox(XPVec4<float>(-0.5, -0.5, 0.0, 1.0), XPVec4<float>(0.5, 0.5, 0.0, 1.0)) } },
                 { "QUAD",
                   { .positions   = { XPVec4<float>(1.0, -1.0, 0.0, 0),
                                    XPVec4<float>(-1.0, -1.0, 0.0, 0),
                                    XPVec4<float>(-1.0, 1.0, 0.0, 0),
                                    XPVec4<float>(1.0, 1.0, 0.0, 0) },
                     .normals     = { XPVec4<float>(0.0, 0.0, 1.0, 0),
                                  XPVec4<float>(0.0, 0.0, 1.0, 0),
                                  XPVec4<float>(0.0, 0.0, 1.0, 0),
                                  XPVec4<float>(0.0, 0.0, 1.0, 0) },
                     .texcoords   = { XPVec4<float>(1.0, 1.0, 0, 0),
                                    XPVec4<float>(0.0, 1.0, 0, 0),
                                    XPVec4<float>(0.0, 0.0, 0, 0),
                                    XPVec4<float>(1.0, 0.0, 0, 0) },
                     .indices     = { 0, 1, 2, 2, 3, 0 },
                     .boundingBox = XPBoundingBox(XPVec4<float>(-1.0, -1.0, 0.0, 1.0), XPVec4<float>(1.0, 1.0, 0.0, 1.0)) } },
                 { "CUBE",
                   { .positions = { XPVec4<float>(-1.0, 1.0, -1.0, 0),  XPVec4<float>(-1.0, -1.0, 1.0, 0),
                                    XPVec4<float>(-1.0, 1.0, 1.0, 0),   XPVec4<float>(-1.0, -1.0, -1.0, 0),
                                    XPVec4<float>(1.0, -1.0, 1.0, 0),   XPVec4<float>(-1.0, -1.0, 1.0, 0),
                                    XPVec4<float>(1.0, -1.0, -1.0, 0),  XPVec4<float>(1.0, 1.0, 1.0, 0),
                                    XPVec4<float>(1.0, -1.0, 1.0, 0),   XPVec4<float>(1.0, 1.0, -1.0, 0),
                                    XPVec4<float>(-1.0, 1.0, 1.0, 0),   XPVec4<float>(1.0, 1.0, 1.0, 0),
                                    XPVec4<float>(1.0, -1.0, 1.0, 0),   XPVec4<float>(-1.0, 1.0, 1.0, 0),
                                    XPVec4<float>(-1.0, -1.0, 1.0, 0),  XPVec4<float>(-1.0, -1.0, -1.0, 0),
                                    XPVec4<float>(1.0, 1.0, -1.0, 0),   XPVec4<float>(1.0, -1.0, -1.0, 0),
                                    XPVec4<float>(-1.0, -1.0, -1.0, 0), XPVec4<float>(1.0, -1.0, -1.0, 0),
                                    XPVec4<float>(1.0, 1.0, -1.0, 0),   XPVec4<float>(-1.0, 1.0, -1.0, 0),
                                    XPVec4<float>(1.0, 1.0, 1.0, 0),    XPVec4<float>(-1.0, 1.0, -1.0, 0) },
                     .normals   = { XPVec4<float>(-1.0, -0.0, 0.0, 0), XPVec4<float>(-1.0, -0.0, 0.0, 0),
                                  XPVec4<float>(-1.0, -0.0, 0.0, 0), XPVec4<float>(0.0, -1.0, 0.0, 0),
                                  XPVec4<float>(0.0, -1.0, 0.0, 0),  XPVec4<float>(0.0, -1.0, 0.0, 0),
                                  XPVec4<float>(1.0, 0.0, 0.0, 0),   XPVec4<float>(1.0, 0.0, 0.0, 0),
                                  XPVec4<float>(1.0, 0.0, 0.0, 0),   XPVec4<float>(0.0, 1.0, 0.0, 0),
                                  XPVec4<float>(0.0, 1.0, 0.0, 0),   XPVec4<float>(0.0, 1.0, 0.0, 0),
                                  XPVec4<float>(0.0, 0.0, 1.0, 0),   XPVec4<float>(0.0, 0.0, 1.0, 0),
                                  XPVec4<float>(0.0, 0.0, 1.0, 0),   XPVec4<float>(0.0, -0.0, -1.0, 0),
                                  XPVec4<float>(0.0, -0.0, -1.0, 0), XPVec4<float>(0.0, -0.0, -1.0, 0),
                                  XPVec4<float>(-1.0, -0.0, 0.0, 0), XPVec4<float>(0.0, -1.0, 0.0, 0),
                                  XPVec4<float>(1.0, 0.0, 0.0, 0),   XPVec4<float>(0.0, 1.0, 0.0, 0),
                                  XPVec4<float>(0.0, 0.0, 1.0, 0),   XPVec4<float>(0.0, -0.0, -1.0, 0) },
                     .texcoords = { XPVec4<float>(0.0, 0.333333, 0, 0),      XPVec4<float>(0.333333, 0.666667, 0, 0),
                                    XPVec4<float>(0.333333, 0.333333, 0, 0), XPVec4<float>(0.333333, 1.0, 0, 0),
                                    XPVec4<float>(0.666667, 0.666667, 0, 0), XPVec4<float>(0.333333, 0.666667, 0, 0),
                                    XPVec4<float>(0.666667, 0.333333, 0, 0), XPVec4<float>(0.333333, 0.666667, 0, 0),
                                    XPVec4<float>(0.666667, 0.666667, 0, 0), XPVec4<float>(0.0, 1.0, 0, 0),
                                    XPVec4<float>(0.333333, 0.666667, 0, 0), XPVec4<float>(0.0, 0.666667, 0, 0),
                                    XPVec4<float>(0.666667, 1.0, 0, 0),      XPVec4<float>(1.0, 0.666667, 0, 0),
                                    XPVec4<float>(0.666667, 0.666667, 0, 0), XPVec4<float>(0.0, 0.0, 0, 0),
                                    XPVec4<float>(0.333333, 0.333333, 0, 0), XPVec4<float>(0.333333, 0.0, 0, 0),
                                    XPVec4<float>(0.0, 0.666667, 0, 0),      XPVec4<float>(0.666667, 1.0, 0, 0),
                                    XPVec4<float>(0.333333, 0.333333, 0, 0), XPVec4<float>(0.333333, 1.0, 0, 0),
                                    XPVec4<float>(1.0, 1.0, 0, 0),           XPVec4<float>(0.0, 0.333333, 0, 0) },
                     .indices   = {
                       0, 1,  2, 3, 4,  5, 6, 7,  8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
                       0, 18, 1, 3, 19, 4, 6, 20, 7, 9, 21, 10, 12, 22, 13, 15, 23, 16,
                     },
                     .boundingBox = XPBoundingBox(XPVec4<float>(-1.0, -1.0, -1.0, 1.0), XPVec4<float>(1.0, 1.0, 1.0, 1.0)) } } };

    return meshes;
}
