#pragma once

#include "R5RRenderer.h"
#include "R5RRendererCommon.h"
#include "R5RSceneDescriptor.h"
#include "R5RThirdParty.h"
#include "R5RWorld.h"

#include <sstream>

#define REFLECTED_RAY_MIN_OFFSET   0.001f;
#define REFLECTED_RAY_MAX_DISTANCE 1000000.0f

struct R5RRenderer;

class TriangleRayCallback : public btCollisionWorld::ClosestRayResultCallback
{
  public:
    btVector3 m_triangleVertices[3];
    uint32_t  m_triangleIndices[3];
    btVector3 m_barycentricCoords; // (u, v, w) where w = 1-u-v
    bool      m_hasTriangle;
    int       m_triangleIndex;
    int       m_partId;

    TriangleRayCallback(const btVector3& rayFromWorld, const btVector3& rayToWorld)
      : btCollisionWorld::ClosestRayResultCallback(rayFromWorld, rayToWorld)
      , m_hasTriangle(false)
      , m_triangleIndex(-1)
      , m_partId(-1)
    {
    }

    btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace) override
    {
        // Store the part ID and triangle index
        m_partId        = rayResult.m_localShapeInfo->m_shapePart;
        m_triangleIndex = rayResult.m_localShapeInfo->m_triangleIndex;

        // Call parent to store the normal hit info
        const btScalar res = ClosestRayResultCallback::addSingleResult(rayResult, normalInWorldSpace);

        // Get the mesh interface
        const btCollisionObject* colObj    = rayResult.m_collisionObject;
        const auto*              meshShape = dynamic_cast<const btBvhTriangleMeshShape*>(colObj->getCollisionShape());
        const btStridingMeshInterface* meshInterface = meshShape->getMeshInterface();

        // Lock and get triangle vertices
        const unsigned char* vertexBase;
        int                  numVerts;
        PHY_ScalarType       vertexType;
        int                  vertexStride;
        const unsigned char* indexBase;
        int                  indexStride;
        int                  numFaces;
        PHY_ScalarType       indexType;

        meshInterface->getLockedReadOnlyVertexIndexBase(
          &vertexBase, numVerts, vertexType, vertexStride, &indexBase, indexStride, numFaces, indexType, m_partId);

        // Get the triangle indices based on index type
        if (indexType == PHY_INTEGER) {
            // 32-bit indices
            auto* triIndices     = (unsigned int*)(indexBase + m_triangleIndex * indexStride);
            m_triangleIndices[0] = static_cast<int>(triIndices[0]);
            m_triangleIndices[1] = static_cast<int>(triIndices[1]);
            m_triangleIndices[2] = static_cast<int>(triIndices[2]);
        } else {
            // 16-bit indices
            auto* triIndices     = (unsigned short*)(indexBase + m_triangleIndex * indexStride);
            m_triangleIndices[0] = static_cast<int>(triIndices[0]);
            m_triangleIndices[1] = static_cast<int>(triIndices[1]);
            m_triangleIndices[2] = static_cast<int>(triIndices[2]);
        }

        // Get the triangle indices
        unsigned int* gfxBase = (unsigned int*)(indexBase + m_triangleIndex * indexStride);

        // Get the vertices
        for (int j = 2; j >= 0; j--) {
            int graphicsindex;
            if (indexType == PHY_INTEGER) {
                graphicsindex = gfxBase[j];
            } else {
                graphicsindex = ((unsigned short*)gfxBase)[j];
            }

            if (vertexType == PHY_FLOAT) {
                auto* vertex          = (float*)(vertexBase + graphicsindex * vertexStride);
                m_triangleVertices[j] = btVector3(vertex[0], vertex[1], vertex[2]);
            } else {
                auto* vertex          = (double*)(vertexBase + graphicsindex * vertexStride);
                m_triangleVertices[j] = btVector3(vertex[0], vertex[1], vertex[2]);
            }
        }

        // Release the lock
        meshInterface->unLockReadOnlyVertexBase(m_partId);

        // Transform vertices to world space if needed
        const btTransform& objTransform = colObj->getWorldTransform();
        m_triangleVertices[0]           = objTransform * m_triangleVertices[0];
        m_triangleVertices[1]           = objTransform * m_triangleVertices[1];
        m_triangleVertices[2]           = objTransform * m_triangleVertices[2];

        // Calculate barycentric coordinates
        m_barycentricCoords = calculateBarycentricCoords(
          m_hitPointWorld, m_triangleVertices[0], m_triangleVertices[1], m_triangleVertices[2]);

        m_hasTriangle = true;
        return res;
    }

  private:
    // Calculates barycentric coordinates (u, v, w) for point p in triangle (a, b, c)
    static btVector3 calculateBarycentricCoords(const btVector3& p,
                                                const btVector3& a,
                                                const btVector3& b,
                                                const btVector3& c)
    {
        const btVector3 v0 = b - a;
        const btVector3 v1 = c - a;
        const btVector3 v2 = p - a;

        const float d00 = v0.dot(v0);
        const float d01 = v0.dot(v1);
        const float d11 = v1.dot(v1);
        const float d20 = v2.dot(v0);
        const float d21 = v2.dot(v1);

        const float denominator = d00 * d11 - d01 * d01;
        const float v           = (d11 * d20 - d01 * d21) / denominator;
        const float w           = (d00 * d21 - d01 * d20) / denominator;
        const float u           = 1.0f - v - w;

        return btVector3(u, v, w);
    }
};

template<typename T>
struct R5RRaytracer
{
    explicit R5RRaytracer(R5RRenderer* renderer)
      : renderer(renderer)
      , scene(nullptr)
      , world(nullptr)
    {
    }
    void setScene(Scene<T>* scene) { this->scene = scene; }
    void initialize()
    {
        world = new R5RWorld();
        assert(world);
        world->collisionConfig      = nullptr;
        world->dispatcher           = nullptr;
        world->overlappingPairCache = new btDbvtBroadphase();
        assert(world->overlappingPairCache);
        world->solver = new btSequentialImpulseConstraintSolver();
        assert(world->solver);
        world->world = new btDiscreteDynamicsWorld(
          world->dispatcher, world->overlappingPairCache, world->solver, world->collisionConfig);
        assert(world->world);
        world->world->setGravity(btVector3(0.0, -9.8, 0.0));

        for (int64_t mi = 0; mi < scene->meshes.size(); ++mi) {
            Mesh<T>& mesh = scene->meshes[mi];
            createTriMeshRigidBody(mesh.vertices, mesh.indices, &mesh);
        }
    }
    void finalize()
    {
        // Remove rigid bodies
        while (!world->rigidBodies.empty()) {
            btRigidBody* body = world->rigidBodies.back();
            world->rigidBodies.pop_back();
            if (body && body->getMotionState()) { delete body->getMotionState(); }
            if (body && body->getCollisionShape()) { delete body->getCollisionShape(); }
            delete body;
        }
        world->rigidBodies.clear();

        // Cleanup world components
        delete world->world;
        world->world = nullptr;
        delete world->solver;
        world->solver = nullptr;
        delete world->overlappingPairCache;
        world->overlappingPairCache = nullptr;
        delete world->dispatcher;
        world->dispatcher = nullptr;
        delete world->collisionConfig;
        world->collisionConfig = nullptr;
        delete world;
        world = nullptr;
    }
    btRigidBody* createTriMeshRigidBody(const std::vector<Vec4<T>>&  vertices,
                                        const std::vector<uint32_t>& indices,
                                        const Mesh<T>*               mesh,
                                        float                        mass = 0.0f)
    {
        // Create mesh interface
        btTriangleIndexVertexArray* meshInterface = new btTriangleIndexVertexArray();

        // Set up indexed mesh
        btIndexedMesh indexedMesh;
        indexedMesh.m_numTriangles        = indices.size() / 3;
        indexedMesh.m_triangleIndexBase   = reinterpret_cast<const unsigned char*>(indices.data());
        indexedMesh.m_triangleIndexStride = 3 * sizeof(uint32_t);
        indexedMesh.m_numVertices         = vertices.size() / 3;
        indexedMesh.m_vertexBase          = reinterpret_cast<const unsigned char*>(vertices.data());
        indexedMesh.m_vertexStride        = 4 * sizeof(float);

        meshInterface->addIndexedMesh(indexedMesh);

        // Create mesh shape
        const bool              useQuantizedAabbCompression = false;
        btBvhTriangleMeshShape* meshShape = new btBvhTriangleMeshShape(meshInterface, useQuantizedAabbCompression);

        // Create rigid body
        btVector3 localInertia(0, 0, 0);
        if (mass != 0.0f) { meshShape->calculateLocalInertia(mass, localInertia); }

        btTransform startTransform;
        startTransform.setFromOpenGLMatrix(&mesh->transform.arr[0]);
        auto*                                    motionState = new btDefaultMotionState(startTransform);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, meshShape, localInertia);
        auto*                                    rigidBody = new btRigidBody(rbInfo);

        rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
        rigidBody->setUserPointer((void*)mesh);

        world->world->addRigidBody(rigidBody);

        return rigidBody;
    }
    Vec3<T> trace(const Ray<T>& primaryRay, const Vec3<T>& viewPos, int maxBounces) const
    {
        Vec3<T> color      = Vec3<T>{ 0.0f, 0.0f, 0.0f };
        Vec3<T> throughput = Vec3<T>{ 1.0f, 1.0f, 1.0f };

        Ray currentRay = primaryRay;

        for (int bounce = 0; bounce < maxBounces; bounce++) {
            R5RRaytracerHitPoint<T> hit;
            raycast(currentRay, hit);

            if (hit.mesh == nullptr) {
                // Sample skybox or background
                color = color + (throughput * Vec3<T>{ 0.0f, 0.0f, 0.0f }); // sampleSkybox(currentRay.direction);
                break;
            }

            fvec3    RTBaseColor;
            fvec3    RTN;
            fvec3    RTEmissionColor;
            float    RTMetallic;
            float    RTRoughness;
            float    RTAO;
            Mesh<T>* mesh           = hit.mesh;
            Vec3<T>  vertexPosition = hit.barycentricCoords.x * mesh->vertices[hit.hitTriangleIndices.x].xyz +
                                     hit.barycentricCoords.y * mesh->vertices[hit.hitTriangleIndices.y].xyz +
                                     hit.barycentricCoords.z * mesh->vertices[hit.hitTriangleIndices.z].xyz;
            Vec3<T> vertexNormal = hit.barycentricCoords.x * mesh->normals[hit.hitTriangleIndices.x] +
                                   hit.barycentricCoords.y * mesh->normals[hit.hitTriangleIndices.y] +
                                   hit.barycentricCoords.z * mesh->normals[hit.hitTriangleIndices.z];
            Vec2<T> vertexUVCoord = hit.barycentricCoords.x * mesh->texCoords[hit.hitTriangleIndices.x] +
                                    hit.barycentricCoords.y * mesh->texCoords[hit.hitTriangleIndices.y] +
                                    hit.barycentricCoords.z * mesh->texCoords[hit.hitTriangleIndices.z];
            Vec3<T> vertexColor = hit.barycentricCoords.x * mesh->colors[hit.hitTriangleIndices.x] +
                                  hit.barycentricCoords.y * mesh->colors[hit.hitTriangleIndices.y] +
                                  hit.barycentricCoords.z * mesh->colors[hit.hitTriangleIndices.z];
            Vec3<T> vertexTangent = hit.barycentricCoords.x * mesh->tangents[hit.hitTriangleIndices.x] +
                                    hit.barycentricCoords.y * mesh->tangents[hit.hitTriangleIndices.y] +
                                    hit.barycentricCoords.z * mesh->tangents[hit.hitTriangleIndices.z];
            Vec3<T> vertexBiTangent = hit.barycentricCoords.x * mesh->biTangents[hit.hitTriangleIndices.x] +
                                      hit.barycentricCoords.y * mesh->biTangents[hit.hitTriangleIndices.y] +
                                      hit.barycentricCoords.z * mesh->biTangents[hit.hitTriangleIndices.z];
            getPBRMaterial(scene->materials[mesh->materialIndex],
                           vertexUVCoord,
                           vertexColor,
                           vertexNormal,
                           RTBaseColor,
                           RTN,
                           RTEmissionColor,
                           RTMetallic,
                           RTRoughness,
                           RTAO,
                           true);

            Vec3<T> viewDir = viewPos - hit.hitPoint;
            viewDir.normalize();

            // Direct lighting from all light sources
            Vec3<T> directLighting = calculateLighting(hit, viewDir);

            // Calculate reflection
            Vec3<T> reflectionColor = Vec3<T>(0.0f, 0.0f, 0.0f);
            float   reflectivity    = calculateReflectivity(RTBaseColor, RTMetallic, RTRoughness);
            if (reflectivity > 0.0f && bounce < maxBounces - 1) {
                Vec3<T> direction =
                  glm::normalize(glm::reflect((currentRay.end - currentRay.start).glm, hit.hitNormal.glm));
                Ray<T> reflectedRay = {};
                reflectedRay.start =
                  hit.hitPoint + hit.hitNormal * REFLECTED_RAY_MIN_OFFSET; // Offset to avoid self-intersection
                reflectedRay.end = reflectedRay.start + direction * REFLECTED_RAY_MAX_DISTANCE;

                // Add some roughness to the reflection for non-perfect mirrors
                if (RTRoughness > 0.0f) {
                    direction.glm    = glm::normalize(glm::mix(
                      direction.glm, randomUnitVectorInHemisphere(hit.hitNormal, RTRoughness).glm, RTRoughness));
                    reflectedRay.end = reflectedRay.start + direction * REFLECTED_RAY_MAX_DISTANCE;
                }

                reflectionColor = calculateReflection(reflectedRay, hit, bounce + 1);
            }

            // Combine reflection with direct lighting
            Vec3<T> materialColor = glm::mix(RTBaseColor.glm * directLighting.glm, reflectionColor.glm, reflectivity);
            color                 = color + (throughput * materialColor);

            // Update throughput for next bounce
            throughput = throughput * RTBaseColor * reflectivity;

            // Russian Roulette termination
            if (bounce > maxBounces) {
                float continueProbability =
                  std::min(std::max(std::max(throughput.x, throughput.y), throughput.z), 0.95f);
                if (static_cast<float>(rand()) / static_cast<float>(RAND_MAX) > continueProbability) { break; }
                throughput = throughput / continueProbability;
            }

            // Setup ray for next bounce
            Vec3<T> direction =
              glm::normalize(glm::reflect((currentRay.end - currentRay.start).glm, hit.hitNormal.glm));
            currentRay.start = hit.hitPoint + hit.hitNormal * REFLECTED_RAY_MIN_OFFSET;
            currentRay.end   = currentRay.start + direction * REFLECTED_RAY_MAX_DISTANCE;
        }

        return color;
    }
    void raycast(const Ray<T>& ray, R5RRaytracerHitPoint<T>& result) const
    {
        const auto          rayFrom = btVector3(ray.start.x, ray.start.y, ray.start.z);
        const auto          rayTo   = btVector3(ray.end.x, ray.end.y, ray.end.z);
        TriangleRayCallback rayCallback(rayFrom, rayTo);
        world->world->rayTest(
          btVector3(ray.start.x, ray.start.y, ray.start.z), btVector3(ray.end.x, ray.end.y, ray.end.z), rayCallback);
        if (rayCallback.hasHit() && rayCallback.m_hasTriangle) {
            const btCollisionObject* collisionObject = rayCallback.m_collisionObject;
            auto*                    meshPtr         = static_cast<Mesh<T>*>(collisionObject->getUserPointer());
            if (meshPtr) {
                result.mesh               = meshPtr;
                result.hitNormal          = Vec3<T>{ rayCallback.m_hitNormalWorld.x(),
                                                     rayCallback.m_hitNormalWorld.y(),
                                                     rayCallback.m_hitNormalWorld.z() };
                result.hitPoint           = Vec3<T>{ rayCallback.m_hitPointWorld.x(),
                                                     rayCallback.m_hitPointWorld.y(),
                                                     rayCallback.m_hitPointWorld.z() };
                result.hitTriangleIndices = Vec3<uint32_t>{ rayCallback.m_triangleIndices[0],
                                                            rayCallback.m_triangleIndices[1],
                                                            rayCallback.m_triangleIndices[2] };
                result.barycentricCoords  = Vec3<T>{ rayCallback.m_barycentricCoords.x(),
                                                     rayCallback.m_barycentricCoords.y(),
                                                     rayCallback.m_barycentricCoords.z() };
                result.distance           = glm::distance(ray.start.glm, result.hitPoint.glm);
                return;
            }
        }

        // we hit nothing ...
        result.mesh = nullptr;
    }
    void renderFrame(RasterizerEventListener& listener, Camera<T>& camera)
    {
        camera.clearColorBuffer();
#ifdef __EMSCRIPTEN__
        listener.onFrameSetColorBufferPtr(camera.colorBuffer.data(), camera.resolution.x, camera.resolution.y);
#endif

        for (int y = 0; y < camera.resolution.y; ++y) {
            for (int x = 0; x < camera.resolution.x; ++x) {
                Ray<T>  mainRay    = camera.getProjectedRay(Vec2<int>{ x, y });
                Vec3<T> color      = gammaCorrect(trace(mainRay, camera.location, 1), 2.2f);
                Vec4<T> finalcolor = Vec4<T>{ color.x, color.y, color.z, 1.0f };
                camera.writeColorBuffer(finalcolor, x, y);
            }
        }

#ifndef __EMSCRIPTEN__
        std::stringstream ss;
        ss << std::filesystem::path(scene->filepath).stem().string() << "_" << camera.name << "_raytraced_color";
        camera.saveColorToEXR(ss.str());
#else
        listener.onFrameRenderBoundingSquare(0, camera.resolution.x, 0, camera.resolution.y);
#endif
    }
    void render(RasterizerEventListener& listener)
    {
        if (scene == nullptr) { return; }
        for (size_t ci = 0; ci < scene->cameras.size(); ++ci) {
            Camera<T>& camera = scene->cameras[ci];
            camera.createFrameBuffers();
            renderFrame(listener, camera);
            camera.destroyFrameBuffers();
        }
    }

    R5RRenderer* renderer;
    Scene<T>*    scene;
    R5RWorld*    world;

  private:
    // Calculate lighting from all light sources
    Vec3<T> calculateLighting(const R5RRaytracerHitPoint<T>& hit, const Vec3<T>& viewDir) const
    {
        Vec3<T> totalLight = Vec3<T>{ 0.0f, 0.0f, 0.0f };

        fvec3 RTBaseColor;
        fvec3 RTN;
        fvec3 RTEmissionColor;
        float RTMetallic;
        float RTRoughness;
        float RTAO;
        if (hit.mesh != nullptr) {
            Mesh<T>* mesh           = hit.mesh;
            Vec3<T>  vertexPosition = hit.barycentricCoords.x * mesh->vertices[hit.hitTriangleIndices.x].xyz +
                                     hit.barycentricCoords.y * mesh->vertices[hit.hitTriangleIndices.y].xyz +
                                     hit.barycentricCoords.z * mesh->vertices[hit.hitTriangleIndices.z].xyz;
            Vec3<T> vertexNormal = hit.barycentricCoords.x * mesh->normals[hit.hitTriangleIndices.x] +
                                   hit.barycentricCoords.y * mesh->normals[hit.hitTriangleIndices.y] +
                                   hit.barycentricCoords.z * mesh->normals[hit.hitTriangleIndices.z];
            Vec2<T> vertexUVCoord = hit.barycentricCoords.x * mesh->texCoords[hit.hitTriangleIndices.x] +
                                    hit.barycentricCoords.y * mesh->texCoords[hit.hitTriangleIndices.y] +
                                    hit.barycentricCoords.z * mesh->texCoords[hit.hitTriangleIndices.z];
            Vec3<T> vertexColor = hit.barycentricCoords.x * mesh->colors[hit.hitTriangleIndices.x] +
                                  hit.barycentricCoords.y * mesh->colors[hit.hitTriangleIndices.y] +
                                  hit.barycentricCoords.z * mesh->colors[hit.hitTriangleIndices.z];
            Vec3<T> vertexTangent = hit.barycentricCoords.x * mesh->tangents[hit.hitTriangleIndices.x] +
                                    hit.barycentricCoords.y * mesh->tangents[hit.hitTriangleIndices.y] +
                                    hit.barycentricCoords.z * mesh->tangents[hit.hitTriangleIndices.z];
            Vec3<T> vertexBiTangent = hit.barycentricCoords.x * mesh->biTangents[hit.hitTriangleIndices.x] +
                                      hit.barycentricCoords.y * mesh->biTangents[hit.hitTriangleIndices.y] +
                                      hit.barycentricCoords.z * mesh->biTangents[hit.hitTriangleIndices.z];
            getPBRMaterial(scene->materials[mesh->materialIndex],
                           vertexUVCoord,
                           vertexColor,
                           vertexNormal,
                           RTBaseColor,
                           RTN,
                           RTEmissionColor,
                           RTMetallic,
                           RTRoughness,
                           RTAO,
                           true);
        } else {
            RTBaseColor     = Vec3<T>{ 1.0f, 1.0f, 1.0f };
            RTN             = Vec3<T>{ 0.0f, 1.0f, 0.0f };
            RTEmissionColor = Vec3<T>{ 0.0f, 0.0f, 0.0f };
            RTMetallic      = 0.0f;
            RTRoughness     = 1.0f;
            RTAO            = 1.0f;
        }

        for (int li = 0; li < scene->lights.size(); ++li) {
            Light<T>& light    = scene->lights[li];
            Vec3<T>   lightDir = light.location - hit.hitPoint;
            lightDir.normalize();
            float distance    = glm::length((light.location - hit.hitPoint).glm);
            float attenuation = 1.0f / (distance * distance);

            // Shadow check
            Ray<T> shadowRay;
            auto   direction = lightDir;
            shadowRay.start  = hit.hitPoint + hit.hitNormal * REFLECTED_RAY_MIN_OFFSET;
            shadowRay.end    = shadowRay.start + direction * REFLECTED_RAY_MAX_DISTANCE;

            R5RRaytracerHitPoint<T> shadowHit;
            raycast(shadowRay, shadowHit);

            if (shadowHit.mesh != nullptr && shadowHit.distance < distance) {
                continue; // Light is occluded
            }

            // Diffuse (Lambert)
            float   NdotL   = std::max(hit.hitNormal.dot(lightDir), 0.0f);
            Vec3<T> diffuse = RTBaseColor * NdotL;

            // Specular (Blinn-Phong)
            Vec3<T> halfwayDir = lightDir + viewDir;
            halfwayDir.normalize();
            float NdotH         = std::max(hit.hitNormal.dot(halfwayDir), 0.0f);
            float specularPower = std::exp2(10.0f * (1.0f - RTRoughness) + 1.0f);
            float specular      = std::pow(NdotH, specularPower);

            // Combine with Fresnel for metals
            Vec3<T> F0 = glm::mix(Vec3<T>(0.04f, 0.04f, 0.04f).glm, RTBaseColor.glm, RTMetallic);
            Vec3<T> F  = F0 + (1.0f - F0) * std::pow(1.0f - std::max(halfwayDir.dot(viewDir), 0.0f), 5.0f);

            Vec3<T> specularColor =
              glm::mix(Vec3<T>(specular, specular, specular).glm, specular * RTBaseColor.glm, RTMetallic);

            // Energy conservation
            float kD = ((1.0f - RTMetallic) * (1.0f - F)).x; // .x == .y == .z

            // Light contribution
            Vec3<T> lightContribution =
              (kD * diffuse + F * specularColor) * light.diffuse * light.intensity * attenuation;

            totalLight = totalLight + lightContribution;
        }

        // Add ambient term
        totalLight = totalLight + (RTBaseColor * RTAO);

        return totalLight;
    }
    float calculateReflectivity(Vec3<T> albedo, float metallic, float roughness) const
    {
        // Base reflectivity for dielectrics (non-metals)
        const Vec3<T> F0_dielectric = Vec3<T>(0.04f, 0.04f, 0.04f);

        // For metals, F0 comes from the albedo (base color)
        Vec3<T> F0 = glm::mix(F0_dielectric.glm, albedo.glm, metallic);

        // Roughness affects the sharpness of reflections but not the base reflectivity
        // (though in some models you might want to modify F0 based on roughness)
        return F0.x;
    }
    Vec3<T> calculateReflection(const Ray<T>& ray, const R5RRaytracerHitPoint<T>& hit, int depth) const
    {
        if (depth > 3) { // Maximum reflection depth
            return Vec3<T>(0.0f, 0.0f, 0.0f);
        }

        R5RRaytracerHitPoint<T> reflectedHit;
        raycast(ray, reflectedHit);

        if (reflectedHit.mesh == nullptr) {
            return Vec3<T>{ 0.0f, 0.0f, 0.0f }; // sampleSkybox(ray.direction);
        }

        fvec3    RTBaseColor;
        fvec3    RTN;
        fvec3    RTEmissionColor;
        float    RTMetallic;
        float    RTRoughness;
        float    RTAO;
        Mesh<T>* mesh           = hit.mesh;
        Vec3<T>  vertexPosition = hit.barycentricCoords.x * mesh->vertices[hit.hitTriangleIndices.x].xyz +
                                 hit.barycentricCoords.y * mesh->vertices[hit.hitTriangleIndices.y].xyz +
                                 hit.barycentricCoords.z * mesh->vertices[hit.hitTriangleIndices.z].xyz;
        Vec3<T> vertexNormal = hit.barycentricCoords.x * mesh->normals[hit.hitTriangleIndices.x] +
                               hit.barycentricCoords.y * mesh->normals[hit.hitTriangleIndices.y] +
                               hit.barycentricCoords.z * mesh->normals[hit.hitTriangleIndices.z];
        Vec2<T> vertexUVCoord = hit.barycentricCoords.x * mesh->texCoords[hit.hitTriangleIndices.x] +
                                hit.barycentricCoords.y * mesh->texCoords[hit.hitTriangleIndices.y] +
                                hit.barycentricCoords.z * mesh->texCoords[hit.hitTriangleIndices.z];
        Vec3<T> vertexColor = hit.barycentricCoords.x * mesh->colors[hit.hitTriangleIndices.x] +
                              hit.barycentricCoords.y * mesh->colors[hit.hitTriangleIndices.y] +
                              hit.barycentricCoords.z * mesh->colors[hit.hitTriangleIndices.z];
        Vec3<T> vertexTangent = hit.barycentricCoords.x * mesh->tangents[hit.hitTriangleIndices.x] +
                                hit.barycentricCoords.y * mesh->tangents[hit.hitTriangleIndices.y] +
                                hit.barycentricCoords.z * mesh->tangents[hit.hitTriangleIndices.z];
        Vec3<T> vertexBiTangent = hit.barycentricCoords.x * mesh->biTangents[hit.hitTriangleIndices.x] +
                                  hit.barycentricCoords.y * mesh->biTangents[hit.hitTriangleIndices.y] +
                                  hit.barycentricCoords.z * mesh->biTangents[hit.hitTriangleIndices.z];
        getPBRMaterial(scene->materials[mesh->materialIndex],
                       vertexUVCoord,
                       vertexColor,
                       vertexNormal,
                       RTBaseColor,
                       RTN,
                       RTEmissionColor,
                       RTMetallic,
                       RTRoughness,
                       RTAO,
                       true);

        Vec3<T> viewDir = ray.start - reflectedHit.hitPoint;
        viewDir.normalize();
        Vec3<T> lighting = calculateLighting(reflectedHit, viewDir);

        float reflectivity = calculateReflectivity(RTBaseColor, RTMetallic, RTRoughness);
        // Recursively trace reflections
        if (reflectivity > 0.0f) {
            Ray<T>  newReflectedRay = {};
            Vec3<T> direction     = glm::normalize(glm::reflect((ray.end - ray.start).glm, reflectedHit.hitNormal.glm));
            newReflectedRay.start = reflectedHit.hitPoint + reflectedHit.hitNormal * REFLECTED_RAY_MIN_OFFSET;
            newReflectedRay.end   = newReflectedRay.start + direction * REFLECTED_RAY_MAX_DISTANCE;

            // Apply roughness
            if (RTRoughness > 0.0f) {
                Vec3<T> newDirection;
                newDirection.glm =
                  glm::normalize(glm::mix((newReflectedRay.end.glm - newReflectedRay.start.glm),
                                          randomUnitVectorInHemisphere(reflectedHit.hitNormal, RTRoughness).glm,
                                          RTRoughness));
                newReflectedRay.start = newReflectedRay.end;
                newReflectedRay.end   = newReflectedRay.start + newDirection * REFLECTED_RAY_MAX_DISTANCE;
            }

            Vec3<T> nextReflection = calculateReflection(newReflectedRay, reflectedHit, depth + 1);
            lighting               = glm::mix(lighting.glm, nextReflection.glm, reflectivity);
        }

        return lighting;
    }
    void createOrthonormalBasis(const Vec3<T>& normal, Vec3<T>& tangent, Vec3<T>& bitangent) const
    {
        // Choose a helper vector that's not parallel to normal
        Vec3<T> helper = abs(normal.x) > 0.99 ? Vec3<T>(0, 1, 0) : Vec3<T>(1, 0, 0);

        tangent = normal.cross(helper);
        tangent.normalize();
        bitangent = normal.cross(tangent);
    }
    Vec3<T> randomUnitVectorInHemisphere(const Vec3<T>& normal, float roughness) const
    {
        float r1 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        float r2 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

        // GGX importance sampling
        float a        = roughness * roughness;
        float phi      = 2.0f * static_cast<float>(M_PI) * r1;
        float cosTheta = std::sqrt((1.0f - r2) / (1.0f + (a * a - 1.0f) * r2));
        float sinTheta = std::sqrt(1.0f - cosTheta * cosTheta);

        // Create half-vector in tangent space
        Vec3<T> H = Vec3<T>(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);

        // Create orthonormal basis
        Vec3<T> tangent, bitangent;
        createOrthonormalBasis(normal, tangent, bitangent);

        // Transform to world space
        return glm::normalize(tangent.glm * H.x + bitangent.glm * H.y + normal.glm * H.z);
    }
};