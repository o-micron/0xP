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

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_TRIANGLES_PER_LEAF 2

/*-------------------------------------*/
/*         Basic Vector Math           */
/*-------------------------------------*/
typedef struct
{
    float x, y, z;
} Vec3;

static inline Vec3
vec3_add(Vec3 a, Vec3 b)
{
    Vec3 r = { a.x + b.x, a.y + b.y, a.z + b.z };
    return r;
}

static inline Vec3
vec3_sub(Vec3 a, Vec3 b)
{
    Vec3 r = { a.x - b.x, a.y - b.y, a.z - b.z };
    return r;
}

static inline Vec3
vec3_scale(Vec3 a, float s)
{
    Vec3 r = { a.x * s, a.y * s, a.z * s };
    return r;
}

static inline Vec3
vec3_div(Vec3 a, float s)
{
    Vec3 r = { a.x / s, a.y / s, a.z / s };
    return r;
}

static inline float
dot(Vec3 a, Vec3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static inline Vec3
cross(Vec3 a, Vec3 b)
{
    Vec3 r = { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x };
    return r;
}

/*-------------------------------------*/
/*         XPSWRay and Primitives          */
/*-------------------------------------*/
typedef struct
{
    Vec3 origin;
    Vec3 direction; // Assumed to be normalized.
} XPSWRay;

typedef struct
{
    Vec3 v0, v1, v2;
} XPSWTriangle;

// Compute the centroid of a triangle.
Vec3
triangle_centroid(const XPSWTriangle* tri)
{
    Vec3 sum = vec3_add(vec3_add(tri->v0, tri->v1), tri->v2);
    return vec3_div(sum, 3.0f);
}

/*-------------------------------------*/
/*         Axis-Aligned BBox (AABB)    */
/*-------------------------------------*/
typedef struct
{
    Vec3 min;
    Vec3 max;
} AABB;

AABB
aabb_from_point(Vec3 p)
{
    AABB box;
    box.min = p;
    box.max = p;
    return box;
}

void
aabb_include_point(AABB* a, Vec3 p)
{
    if (p.x < a->min.x) a->min.x = p.x;
    if (p.y < a->min.y) a->min.y = p.y;
    if (p.z < a->min.z) a->min.z = p.z;
    if (p.x > a->max.x) a->max.x = p.x;
    if (p.y > a->max.y) a->max.y = p.y;
    if (p.z > a->max.z) a->max.z = p.z;
}

AABB
compute_triangle_aabb(const XPSWTriangle* tri)
{
    AABB box = aabb_from_point(tri->v0);
    aabb_include_point(&box, tri->v1);
    aabb_include_point(&box, tri->v2);
    return box;
}

AABB
combine_aabb(AABB a, AABB b)
{
    AABB box;
    box.min.x = (a.min.x < b.min.x) ? a.min.x : b.min.x;
    box.min.y = (a.min.y < b.min.y) ? a.min.y : b.min.y;
    box.min.z = (a.min.z < b.min.z) ? a.min.z : b.min.z;
    box.max.x = (a.max.x > b.max.x) ? a.max.x : b.max.x;
    box.max.y = (a.max.y > b.max.y) ? a.max.y : b.max.y;
    box.max.z = (a.max.z > b.max.z) ? a.max.z : b.max.z;
    return box;
}

/*-------------------------------------*/
/*             BVH Node              */
/*-------------------------------------*/
typedef struct XPSWBVHNode
{
    AABB bbox;
    // Leaf node: store an array of triangle pointers.
    XPSWTriangle** triangles;
    int            triangleCount;
    // Internal node: pointers to children.
    struct XPSWBVHNode* left;
    struct XPSWBVHNode* right;
} XPSWBVHNode;

/*-------------------------------------*/
/*  Sorting Functions for BVH          */
/*-------------------------------------*/
// Global variable to choose the sorting axis (0: x, 1: y, 2: z)
static int g_axis = 0;

int
compare_triangles(const void* a, const void* b)
{
    XPSWTriangle* triA = *(XPSWTriangle**)a;
    XPSWTriangle* triB = *(XPSWTriangle**)b;

    Vec3 centA = triangle_centroid(triA);
    Vec3 centB = triangle_centroid(triB);

    float diff = 0.0f;
    if (g_axis == 0) {
        diff = centA.x - centB.x;
    } else if (g_axis == 1) {
        diff = centA.y - centB.y;
    } else {
        diff = centA.z - centB.z;
    }

    return (diff < 0.0f) ? -1 : (diff > 0.0f) ? 1 : 0;
}

/*-------------------------------------*/
/*         BVH Construction            */
/*-------------------------------------*/
XPSWBVHNode*
build_bvh(XPSWTriangle** triangles, int count)
{
    XPSWBVHNode* node = (XPSWBVHNode*)malloc(sizeof(XPSWBVHNode));
    if (!node) {
        fprintf(stderr, "Memory allocation failed for XPSWBVHNode.\n");
        exit(EXIT_FAILURE);
    }
    node->left          = NULL;
    node->right         = NULL;
    node->triangles     = NULL;
    node->triangleCount = count;

    // Compute bounding box for current node.
    AABB bbox = compute_triangle_aabb(triangles[0]);
    for (int i = 1; i < count; i++) {
        AABB triBox = compute_triangle_aabb(triangles[i]);
        bbox        = combine_aabb(bbox, triBox);
    }
    node->bbox = bbox;

    // Create leaf node if count is small.
    if (count <= MAX_TRIANGLES_PER_LEAF) {
        node->triangles = (XPSWTriangle**)malloc(count * sizeof(XPSWTriangle*));
        if (!node->triangles) {
            fprintf(stderr, "Memory allocation failed for leaf triangles.\n");
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < count; i++) { node->triangles[i] = triangles[i]; }
        return node;
    }

    // Choose the longest axis to split.
    Vec3 extent = vec3_sub(bbox.max, bbox.min);
    int  axis   = 0;
    if (extent.y > extent.x) axis = 1;
    if (extent.z > ((axis == 0) ? extent.x : extent.y)) axis = 2;
    g_axis = axis;

    // Sort triangle pointers along the chosen axis.
    qsort(triangles, count, sizeof(XPSWTriangle*), compare_triangles);

    // Split the array into two halves.
    int mid     = count / 2;
    node->left  = build_bvh(triangles, mid);
    node->right = build_bvh(triangles + mid, count - mid);

    return node;
}

void
free_bvh(XPSWBVHNode* node)
{
    if (!node) return;
    free_bvh(node->left);
    free_bvh(node->right);
    if (node->triangles) free(node->triangles);
    free(node);
}

void
print_bvh(XPSWBVHNode* node, int depth)
{
    if (!node) return;
    for (int i = 0; i < depth; i++) printf("  ");
    printf("XPSWBVHNode: count=%d, bbox min=(%.2f, %.2f, %.2f) max=(%.2f, %.2f, %.2f)\n",
           node->triangleCount,
           node->bbox.min.x,
           node->bbox.min.y,
           node->bbox.min.z,
           node->bbox.max.x,
           node->bbox.max.y,
           node->bbox.max.z);
    print_bvh(node->left, depth + 1);
    print_bvh(node->right, depth + 1);
}

/*-------------------------------------*/
/*  Ray–Triangle Intersection (MT)   */
/*  with Barycentrics (u, v, w)         */
/*-------------------------------------*/
int
ray_triangle_intersect(const XPSWRay* ray, const XPSWTriangle* tri, float* t_out, float* u_out, float* v_out)
{
    const float EPSILON = 1e-6f;
    Vec3        edge1   = vec3_sub(tri->v1, tri->v0);
    Vec3        edge2   = vec3_sub(tri->v2, tri->v0);
    Vec3        h       = cross(ray->direction, edge2);
    float       a       = dot(edge1, h);
    if (fabsf(a) < EPSILON) return 0; // XPSWRay is parallel to the triangle.
    float f = 1.0f / a;
    Vec3  s = vec3_sub(ray->origin, tri->v0);
    float u = f * dot(s, h);
    if (u < 0.0f || u > 1.0f) return 0;
    Vec3  q = cross(s, edge1);
    float v = f * dot(ray->direction, q);
    if (v < 0.0f || u + v > 1.0f) return 0;
    float t = f * dot(edge2, q);
    if (t > EPSILON) { // A valid intersection.
        if (t_out) *t_out = t;
        if (u_out) *u_out = u;
        if (v_out) *v_out = v;
        return 1;
    }
    return 0;
}

/*-------------------------------------*/
/*      Ray–AABB Intersection          */
/*-------------------------------------*/
int
ray_aabb_intersect(const XPSWRay* ray, const AABB* box, float* tmin_out, float* tmax_out)
{
    float tmin = -FLT_MAX;
    float tmax = FLT_MAX;
    float invD, t1, t2;

    // X axis
    if (fabsf(ray->direction.x) < 1e-8f) {
        if (ray->origin.x < box->min.x || ray->origin.x > box->max.x) return 0;
    } else {
        invD = 1.0f / ray->direction.x;
        t1   = (box->min.x - ray->origin.x) * invD;
        t2   = (box->max.x - ray->origin.x) * invD;
        if (t1 > t2) {
            float tmp = t1;
            t1        = t2;
            t2        = tmp;
        }
        if (t1 > tmin) tmin = t1;
        if (t2 < tmax) tmax = t2;
    }

    // Y axis
    if (fabsf(ray->direction.y) < 1e-8f) {
        if (ray->origin.y < box->min.y || ray->origin.y > box->max.y) return 0;
    } else {
        invD = 1.0f / ray->direction.y;
        t1   = (box->min.y - ray->origin.y) * invD;
        t2   = (box->max.y - ray->origin.y) * invD;
        if (t1 > t2) {
            float tmp = t1;
            t1        = t2;
            t2        = tmp;
        }
        if (t1 > tmin) tmin = t1;
        if (t2 < tmax) tmax = t2;
    }

    // Z axis
    if (fabsf(ray->direction.z) < 1e-8f) {
        if (ray->origin.z < box->min.z || ray->origin.z > box->max.z) return 0;
    } else {
        invD = 1.0f / ray->direction.z;
        t1   = (box->min.z - ray->origin.z) * invD;
        t2   = (box->max.z - ray->origin.z) * invD;
        if (t1 > t2) {
            float tmp = t1;
            t1        = t2;
            t2        = tmp;
        }
        if (t1 > tmin) tmin = t1;
        if (t2 < tmax) tmax = t2;
    }

    if (tmax < tmin) return 0;

    if (tmin_out) *tmin_out = tmin;
    if (tmax_out) *tmax_out = tmax;
    return 1;
}

/*-------------------------------------*/
/*       BVH Traversal for Rays        */
/*  (Now returns barycentrics too)     */
/*-------------------------------------*/
// This function traverses the BVH and, if an intersection is found,
// outputs the closest hit along with its barycentric coordinates (u and v).
// (w can be computed as 1 - u - v.)
int
traverse_bvh(XPSWBVHNode*   node,
             const XPSWRay* ray,
             float*         closest_t,
             XPSWTriangle** hit_triangle,
             float*         hit_u,
             float*         hit_v)
{
    float tmin, tmax;
    if (!ray_aabb_intersect(ray, &node->bbox, &tmin, &tmax)) return 0; // XPSWRay misses the bounding box.

    int hit = 0;
    // If leaf node, test each triangle.
    if (node->left == NULL && node->right == NULL) {
        for (int i = 0; i < node->triangleCount; i++) {
            float t, u, v;
            if (ray_triangle_intersect(ray, node->triangles[i], &t, &u, &v)) {
                if (t < *closest_t) {
                    *closest_t    = t;
                    *hit_triangle = node->triangles[i];
                    *hit_u        = u;
                    *hit_v        = v;
                    hit           = 1;
                }
            }
        }
        return hit;
    }

    // Otherwise, recursively traverse child nodes.
    if (node->left)
        if (traverse_bvh(node->left, ray, closest_t, hit_triangle, hit_u, hit_v)) hit = 1;
    if (node->right)
        if (traverse_bvh(node->right, ray, closest_t, hit_triangle, hit_u, hit_v)) hit = 1;

    return hit;
}

/*-------------------------------------*/
/*            Main Function            */
/*-------------------------------------*/
// int
// main(void)
// {
//     // Sample triangles for our scene.
//     int      numTriangles    = 4;
//     XPSWTriangle triangleData[4] = { { { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
//                                  { { 1.0f, 1.0f, 1.0f }, { 2.0f, 1.0f, 1.0f }, { 1.0f, 2.0f, 1.0f } },
//                                  { { -1.0f, -1.0f, -1.0f }, { -2.0f, -1.0f, -1.0f }, { -1.0f, -2.0f, -1.0f } },
//                                  { { 0.5f, 0.5f, 0.5f }, { 1.5f, 0.5f, 0.5f }, { 0.5f, 1.5f, 0.5f } } };

//     // Build an array of triangle pointers.
//     XPSWTriangle** triangles = (XPSWTriangle**)malloc(numTriangles * sizeof(XPSWTriangle*));
//     if (!triangles) {
//         fprintf(stderr, "Memory allocation failed for triangle pointers.\n");
//         exit(EXIT_FAILURE);
//     }
//     for (int i = 0; i < numTriangles; i++) { triangles[i] = &triangleData[i]; }

//     // Build the BVH.
//     XPSWBVHNode* root = build_bvh(triangles, numTriangles);
//     printf("BVH Tree Structure:\n");
//     print_bvh(root, 0);

//     // Define a ray (for example, from (0,0,-5) in the +z direction).
//     XPSWRay ray;
//     ray.origin    = Vec3{ 0.0f, 0.0f, -5.0f };
//     ray.direction = Vec3{ 0.0f, 0.0f, 1.0f }; // Assume normalized.

//     // Traverse the BVH to find the closest intersection.
//     float     closest_t    = FLT_MAX;
//     XPSWTriangle* hit_triangle = NULL;
//     float     bary_u = 0.0f, bary_v = 0.0f;

//     if (traverse_bvh(root, &ray, &closest_t, &hit_triangle, &bary_u, &bary_v)) {
//         float bary_w = 1.0f - bary_u - bary_v;
//         printf("XPSWRay hit a triangle at t = %f\n", closest_t);
//         printf("Barycentric coords: u = %f, v = %f, w = %f\n", bary_u, bary_v, bary_w);
//     } else {
//         printf("XPSWRay missed all triangles.\n");
//     }

//     // Cleanup.
//     free_bvh(root);
//     free(triangles);

//     return 0;
// }