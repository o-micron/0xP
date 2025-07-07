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
