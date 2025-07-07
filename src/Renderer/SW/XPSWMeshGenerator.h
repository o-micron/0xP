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

#include "R5RSceneDescriptor.h"

#include <vector>

template<typename T>
void
generateSphere(std::vector<Vertex<T>>& vertices, const double radius, const int detailLevel)
{
    vertices.clear();
    vertices.reserve((detailLevel + 1) * (detailLevel * 2 + 1));

    for (size_t i = 0; i < detailLevel + 1; ++i) {
        const double lat = M_PI * (-0.5 + static_cast<double>(i) / detailLevel);
        for (size_t j = 0; j < detailLevel * 2 + 1; ++j) {
            const double lon = 2.0 * M_PI * static_cast<double>(j) / (detailLevel * 2);
            Vertex<T>    v   = {};
            v.location.x     = static_cast<T>(radius * cos(lat) * cos(lon));
            v.location.y     = static_cast<T>(radius * cos(lat) * sin(lon));
            v.location.z     = static_cast<T>(radius * sin(lat));
            v.color.x = v.color.y = v.color.z = v.color.w = 1.0f;
            vertices.emplace_back(v);
        }
    }
}
