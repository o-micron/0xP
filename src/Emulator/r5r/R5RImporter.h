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

#include "R5RLogger.h"
#include "R5RMaths.h"
#include "R5RSceneDescriptor.h"
#include "R5RTexture.h"
#include "R5RThirdParty.h"

#ifdef __EMSCRIPTEN__
    #include "wasm_interface.h"
#endif

#include <iostream>
#include <map>
#include <vector>

inline void
copyTextureRtoRGBA(const uint8_t* from, uint8_t* to, int64_t width, int64_t height)
{
    for (int64_t y = 0; y < height; ++y) {
        for (int64_t x = 0; x < width; ++x) {
            int64_t idx     = y * width + x;
            to[4 * idx + 0] = from[1 * idx + 0];
            to[4 * idx + 1] = 0.0f;
            to[4 * idx + 2] = 0.0f;
            to[4 * idx + 3] = 1.0f;
        }
    }
}

inline void
copyTextureRGtoRGBA(const uint8_t* from, uint8_t* to, int64_t width, int64_t height)
{
    for (int64_t y = 0; y < height; ++y) {
        for (int64_t x = 0; x < width; ++x) {
            int64_t idx     = y * width + x;
            to[4 * idx + 0] = from[2 * idx + 0];
            to[4 * idx + 1] = from[2 * idx + 1];
            to[4 * idx + 2] = 0.0f;
            to[4 * idx + 3] = 1.0f;
        }
    }
}

inline void
copyTextureRGBtoRGBA(const uint8_t* from, uint8_t* to, int64_t width, int64_t height)
{
    for (int64_t y = 0; y < height; ++y) {
        for (int64_t x = 0; x < width; ++x) {
            int64_t idx     = y * width + x;
            to[4 * idx + 0] = from[3 * idx + 0];
            to[4 * idx + 1] = from[3 * idx + 1];
            to[4 * idx + 2] = from[3 * idx + 2];
            to[4 * idx + 3] = 1.0f;
        }
    }
}

inline void
copyTextureRGBAtoRGBA(const uint8_t* from, uint8_t* to, int64_t width, int64_t height)
{
    memcpy(to, from, sizeof(uint8_t) * width * height * 4);
}

inline void
copyTextureARGBtoRGBA(const uint8_t* from, uint8_t* to, int64_t width, int64_t height)
{
    for (int64_t y = 0; y < height; ++y) {
        for (int64_t x = 0; x < width; ++x) {
            int64_t idx     = y * width + x;
            to[4 * idx + 0] = from[4 * idx + 1];
            to[4 * idx + 1] = from[4 * idx + 2];
            to[4 * idx + 2] = from[4 * idx + 3];
            to[4 * idx + 3] = from[4 * idx + 0];
        }
    }
}

template<typename T>
void
processTextureLoading(Scene<T>&         scene,
                      const aiScene*    aiScene,
                      const aiMaterial* aiMaterial,
                      aiTextureType     type,
                      bool&             hasTexture,
                      Texture2D&        texture)
{
    aiString       texPath;
    const aiReturn ret = aiMaterial->GetTexture(type, 0, &texPath);
    hasTexture         = false;
    if (ret == AI_SUCCESS) {
        // Check if texture is embedded
        const aiTexture* embeddedTexture = aiScene->GetEmbeddedTexture(texPath.C_Str());
        if (embeddedTexture) {
            texture.path = texPath.C_Str();
            // Handle embedded texture
            if (embeddedTexture->mHeight == 0) {
                texture.height = log2(embeddedTexture->mWidth);
                texture.width  = log2(embeddedTexture->mWidth);
                ASSERT_ERROR(texture.width * texture.height == embeddedTexture->mWidth,
                             "Texture buffer is not power of 2");
            } else {
                texture.width  = embeddedTexture->mWidth;
                texture.height = embeddedTexture->mHeight;
            }
            // force it always to 4 channels for now ... todo
            texture.channels = 4;
            texture.u8data   = (uint8_t*)malloc(sizeof(uint8_t) * texture.width * texture.height * texture.channels);
            if (texture.u8data == nullptr) {
                LOGV_CRITICAL("Could not allocate embedded texture memory of {} bytes",
                              sizeof(uint8_t) * texture.width * texture.height * texture.channels);
            }

            // try to parse the hint to figure out the number of channels
            std::string hint = embeddedTexture->achFormatHint;
            if (hint.substr(0, 4) == "rgba") {
                // desired case scenario
                if (hint.substr(4, 4) == "8888") {
                    copyTextureRGBAtoRGBA(
                      (uint8_t*)embeddedTexture->pcData, texture.u8data, texture.width, texture.height);
                } else if (hint.substr(4, 4) == "8880") {
                    copyTextureRGBtoRGBA(
                      (uint8_t*)embeddedTexture->pcData, texture.u8data, texture.width, texture.height);
                } else if (hint.substr(4, 4) == "8800") {
                    copyTextureRGtoRGBA(
                      (uint8_t*)embeddedTexture->pcData, texture.u8data, texture.width, texture.height);
                } else if (hint.substr(4, 4) == "8000") {
                    copyTextureRtoRGBA(
                      (uint8_t*)embeddedTexture->pcData, texture.u8data, texture.width, texture.height);
                } else {
                    ASSERT_ERROR(false, "We do not handle such cases yet.");
                }
            } else if (hint.substr(0, 4) == "argb") {
                // needs swizzling of values
                if (hint.substr(4, 4) == "8888") {
                    copyTextureARGBtoRGBA(
                      (uint8_t*)embeddedTexture->pcData, texture.u8data, texture.width, texture.height);
                } else {
                    ASSERT_ERROR(false, "We do not handle such cases yet.");
                }
            } else if (hint.substr(0, 3) == "jpg") {
                copyTextureRGBtoRGBA((uint8_t*)embeddedTexture->pcData, texture.u8data, texture.width, texture.height);
            } else if (hint.substr(0, 3) == "png") {
                copyTextureRGBAtoRGBA((uint8_t*)embeddedTexture->pcData, texture.u8data, texture.width, texture.height);
            } else {
                ASSERT_ERROR(false, "We do not handle such cases yet.");
            }
            hasTexture = true;
        } else {
            // Handle external texture file
            auto path    = std::filesystem::path("./assets/").append(texPath.C_Str());
            texture.path = path.string();
            if (path.extension() == ".png" || path.extension() == ".jpg") {
#ifndef __EMSCRIPTEN__
                texture.u8data =
                  stbi_load(path.string().c_str(), &texture.width, &texture.height, &texture.channels, 4);
#else
                getMappedTexture(
                  path.string().c_str(), &texture.u8data, &texture.width, &texture.height, &texture.channels);
#endif
                if (texture.u8data == nullptr) {
                    LOGV_WARNING("Failed to load texture <{}>", path.string());
                    texture.width    = 0;
                    texture.height   = 0;
                    texture.channels = 0;
                } else {
                    LOGV_NOTICE("Done loading texture <{}>", path.string());
                    hasTexture = true;
                }
            }
        }
    }
}

template<typename T>
void
processMaterial(Scene<T>& scene, const aiScene* aiScene, aiMaterial* aiMaterial, Material<T>& material)
{
    LOGV_DEBUG("Loading material: {}", aiMaterial->GetName().C_Str());
    {
        processTextureLoading<T>(scene,
                                 aiScene,
                                 aiMaterial,
                                 aiTextureType_BASE_COLOR,
                                 material.hasBaseColorTexture,
                                 material.baseColorTexture);
        if (material.hasBaseColorTexture == false) {
            aiColor4D baseColor;
            auto      ret = aiMaterial->Get(AI_MATKEY_BASE_COLOR, baseColor);
            if (ret == AI_SUCCESS) {
                material.baseColorValue.x = baseColor.r;
                material.baseColorValue.y = baseColor.g;
                material.baseColorValue.z = baseColor.b;
            } else {
                material.baseColorValue.x = 1.0f;
                material.baseColorValue.y = 1.0f;
                material.baseColorValue.z = 1.0f;
            }
        }
    }
    {
        processTextureLoading<T>(scene,
                                 aiScene,
                                 aiMaterial,
                                 aiTextureType_NORMAL_CAMERA,
                                 material.hasNormalMapTexture,
                                 material.normalMapTexture);
    }
    {
        processTextureLoading<T>(scene,
                                 aiScene,
                                 aiMaterial,
                                 aiTextureType_EMISSION_COLOR,
                                 material.hasEmissionColorTexture,
                                 material.emissionColorTexture);
        if (material.hasEmissionColorTexture == false) { material.emissionColorValue = Vec3<T>{ 0.0f, 0.0f, 0.0f }; }
    }
    {
        processTextureLoading<T>(
          scene, aiScene, aiMaterial, aiTextureType_METALNESS, material.hasMetallicTexture, material.metallicTexture);
        if (material.hasMetallicTexture == false) {
            float metallic;
            auto  ret = aiMaterial->Get(AI_MATKEY_METALLIC_FACTOR, metallic);
            if (ret == AI_SUCCESS) {
                material.metallicValue = metallic;
            } else {
                material.metallicValue = 0.0f;
            }
        }
    }
    {
        processTextureLoading<T>(scene,
                                 aiScene,
                                 aiMaterial,
                                 aiTextureType_DIFFUSE_ROUGHNESS,
                                 material.hasRoughnessTexture,
                                 material.roughnessTexture);
        if (material.hasRoughnessTexture == false) {
            float roughness;
            auto  ret = aiMaterial->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness);
            if (ret == AI_SUCCESS) {
                material.roughnessValue = roughness;
            } else {
                material.roughnessValue = 0.0f;
            }
        }
    }
    {
        processTextureLoading<T>(
          scene, aiScene, aiMaterial, aiTextureType_AMBIENT_OCCLUSION, material.hasAOTexture, material.aoTexture);
        if (material.hasAOTexture == false) { material.aoValue = 0.01f; }
    }

    LOGV_DEBUG("\tbase color texture: {}",
               (material.hasBaseColorTexture ? material.baseColorTexture.path.c_str() : "n/a"));
    LOGV_DEBUG("\tnormal map texture: {}",
               (material.hasNormalMapTexture ? material.normalMapTexture.path.c_str() : "n/a"));
    LOGV_DEBUG("\temission color texture: {}",
               (material.hasEmissionColorTexture ? material.emissionColorTexture.path.c_str() : "n/a"));
    LOGV_DEBUG("\tmetallic texture: {}", (material.hasMetallicTexture ? material.metallicTexture.path.c_str() : "n/a"));
    LOGV_DEBUG("\troughness texture: {}",
               (material.hasRoughnessTexture ? material.roughnessTexture.path.c_str() : "n/a"));
    LOGV_DEBUG("\tao texture: {}", (material.hasAOTexture ? material.aoTexture.path.c_str() : "n/a"));

    LOGV_DEBUG("\tbase color value: {}", material.baseColorValue);
    LOGV_DEBUG("\temission color value: {}", material.emissionColorValue);
    LOGV_DEBUG("\tmetallic value: {}", material.metallicValue);
    LOGV_DEBUG("\troughness value: {}", material.roughnessValue);
    LOGV_DEBUG("\tao value: {}", material.aoValue);
}

// Recursive function to traverse the scene graph and compute global transformations
template<typename T>
void
processNode(const aiNode*                           node,
            const aiScene*                          aiScene,
            const glm::mat<4, 4, T, glm::defaultp>& parentTransform,
            Scene<T>&                               scene,
            std::unordered_set<std::string>&        uniqueMeshNames,
            std::unordered_set<std::string>&        uniqueMaterialNames,
            std::unordered_set<std::string>&        uniqueCameraNames,
            std::unordered_set<std::string>&        uniqueLightNames)
{
    // Compute the global transformation matrix for this node
    glm::mat<4, 4, T, glm::defaultp> globalTransform =
      parentTransform * glm::transpose(glm::make_mat4(&node->mTransformation.a1));

    // Compute the inverse transpose of the global transformation for normals
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(globalTransform)));

    // Process all meshes in this node
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* aiMesh = aiScene->mMeshes[node->mMeshes[i]];
        if ((aiMesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE) || !aiMesh->HasPositions() || !aiMesh->HasNormals() ||
            !aiMesh->HasTextureCoords(0)) {
            LOGV_WARNING("Incompatible mesh, not loaded <{}>", aiMesh->mName.C_Str());
            continue;
        }
        if (uniqueMeshNames.find(aiMesh->mName.C_Str()) != uniqueMeshNames.end()) { continue; }
        uniqueMeshNames.insert(aiMesh->mName.C_Str());

        // Create a mesh to store mesh data
        scene.meshes.push_back({});
        Mesh<T>& meshData        = scene.meshes.back();
        meshData.name            = aiMesh->mName.C_Str();
        meshData.transform       = Mat4<T>::identity().glm;
        meshData.boundingBox.min = Vec3<T>{ std::numeric_limits<T>::infinity(),
                                            std::numeric_limits<T>::infinity(),
                                            std::numeric_limits<T>::infinity() };
        meshData.boundingBox.max = Vec3<T>{ -std::numeric_limits<T>::infinity(),
                                            -std::numeric_limits<T>::infinity(),
                                            -std::numeric_limits<T>::infinity() };

        // process materials
        {
            aiMaterial*  aiMaterial = aiScene->mMaterials[aiMesh->mMaterialIndex];
            Material<T>& material   = scene.materials[aiMesh->mMaterialIndex];
            std::string  name       = aiMaterial->GetName().C_Str();
            meshData.materialIndex  = aiMesh->mMaterialIndex;
            if (uniqueMaterialNames.find(name) == uniqueMaterialNames.end()) {
                uniqueMaterialNames.insert(name);
                processMaterial(scene, aiScene, aiMaterial, material);
            }
        }

        // Extract vertices and apply the global transformation
        for (unsigned int j = 0; j < aiMesh->mNumVertices; j++) {
            const aiVector3D&             vertex = aiMesh->mVertices[j];
            glm::vec<4, T, glm::defaultp> transformedVertex =
              globalTransform * glm::vec<4, T, glm::defaultp>(vertex.x, vertex.y, vertex.z, 1.0f);
            if (transformedVertex.x < meshData.boundingBox.min.x) { meshData.boundingBox.min.x = transformedVertex.x; }
            if (transformedVertex.x > meshData.boundingBox.max.x) { meshData.boundingBox.max.x = transformedVertex.x; }
            if (transformedVertex.y < meshData.boundingBox.min.y) { meshData.boundingBox.min.y = transformedVertex.y; }
            if (transformedVertex.y > meshData.boundingBox.max.y) { meshData.boundingBox.max.y = transformedVertex.y; }
            if (transformedVertex.z < meshData.boundingBox.min.z) { meshData.boundingBox.min.z = transformedVertex.z; }
            if (transformedVertex.z > meshData.boundingBox.max.z) { meshData.boundingBox.max.z = transformedVertex.z; }
            meshData.vertices.push_back(transformedVertex);

            const aiVector3D& tangent = aiMesh->mTangents[j];
            meshData.tangents.push_back(glm::vec<3, T, glm::defaultp>(tangent.x, tangent.y, tangent.z));

            const aiVector3D& biTangent = aiMesh->mBitangents[j];
            meshData.biTangents.push_back(glm::vec<3, T, glm::defaultp>(biTangent.x, biTangent.y, biTangent.z));

            meshData.colors.push_back(Vec3<T>{ 1.0f, 1.0f, 1.0f });
        }

        // Extract normals and apply the normal matrix
        ASSERT_ERROR(aiMesh->HasNormals(), "Expecting a mesh with normals");
        if (aiMesh->HasNormals()) {
            for (unsigned int j = 0; j < aiMesh->mNumVertices; j++) {
                const aiVector3D&             normal = aiMesh->mNormals[j];
                glm::vec<3, T, glm::defaultp> transformedNormal =
                  normalMatrix * glm::vec<3, T, glm::defaultp>(normal.x, normal.y, normal.z);
                meshData.normals.push_back(glm::normalize(transformedNormal));
            }
        }

        // Extract texture coordinates (UVs)
        ASSERT_ERROR(aiMesh->HasTextureCoords(0), "Expecting a mesh with texture coordinates");
        if (aiMesh->HasTextureCoords(0)) { // Check for the first set of texture coordinates
            for (unsigned int j = 0; j < aiMesh->mNumVertices; j++) {
                const aiVector3D& texCoord = aiMesh->mTextureCoords[0][j]; // First UV channel
                meshData.texCoords.push_back(glm::vec<2, T, glm::defaultp>(texCoord.x, texCoord.y));
            }
        }

        // Extract indices
        for (unsigned int j = 0; j < aiMesh->mNumFaces; j++) {
            const aiFace& face = aiMesh->mFaces[j];
            for (unsigned int k = 0; k < face.mNumIndices; k++) { meshData.indices.push_back(face.mIndices[k]); }
        }
    }

    // Process cameras
    for (unsigned int i = 0; i < aiScene->mNumCameras; i++) {
        aiCamera* aiCam = aiScene->mCameras[i];
        if (uniqueCameraNames.find(aiCam->mName.C_Str()) != uniqueCameraNames.end()) { continue; }
        uniqueCameraNames.insert(aiCam->mName.C_Str());

        // Find the camera node to get its global transformation
        aiNode* cameraNode = aiScene->mRootNode->FindNode(aiCam->mName);
        if (cameraNode) {
            auto cameraTransform = globalTransform * glm::transpose(glm::make_mat4(&cameraNode->mTransformation.a1));
            auto cameraPosition  = cameraTransform * glm::vec<4, T, glm::defaultp>(
                                                      aiCam->mPosition.x, aiCam->mPosition.y, aiCam->mPosition.z, 1.0f);

            // Extract camera data
            scene.cameras.push_back({});
            Camera<T>& camera = scene.cameras.back();
            camera.name       = aiCam->mName.C_Str();
            camera.fov        = glm::degrees(aiCam->mHorizontalFOV / 2.0f);
            camera.resolution = u32vec2{ WIDTH, HEIGHT };
            camera.zNearPlane = aiCam->mClipPlaneNear;
            camera.zFarPlane  = aiCam->mClipPlaneFar;
            camera.location =
              glm::vec<3, T, glm::defaultp>(cameraTransform[3][0], cameraTransform[3][1], cameraTransform[3][2]);
            camera.target = glm::normalize(
              glm::vec<3, T, glm::defaultp>(-cameraTransform[2][0], -cameraTransform[2][1], -cameraTransform[2][2]));
            camera.target = camera.location + camera.target;
            camera.up     = glm::normalize(
              glm::vec<3, T, glm::defaultp>(cameraTransform[1][0], cameraTransform[1][1], cameraTransform[1][2]));
            camera.updateMatrices();
        }
    }

    // Process lights
    for (unsigned int i = 0; i < aiScene->mNumLights; i++) {
        aiLight* aiLight = aiScene->mLights[i];
        if (uniqueLightNames.find(aiLight->mName.C_Str()) != uniqueLightNames.end()) { continue; }
        uniqueLightNames.insert(aiLight->mName.C_Str());
        LOGV_DEBUG("Loading light: {}", aiLight->mName.C_Str());

        // Find the light node to get its global transformation
        aiNode* lightNode = aiScene->mRootNode->FindNode(aiLight->mName);
        if (lightNode) {
            glm::mat<4, 4, T, glm::defaultp> lightTransform =
              globalTransform * glm::transpose(glm::make_mat4(&lightNode->mTransformation.a1));
            glm::vec<4, T, glm::defaultp> lightPosition =
              lightTransform *
              glm::vec<4, T, glm::defaultp>(aiLight->mPosition.x, aiLight->mPosition.y, aiLight->mPosition.z, 1.0f);
            glm::vec<4, T, glm::defaultp> lightDirection =
              lightTransform *
              glm::vec<4, T, glm::defaultp>(aiLight->mDirection.x, aiLight->mDirection.y, aiLight->mDirection.z, 1.0f);

            // Extract light data
            scene.lights.push_back({});
            Light<T>& light = scene.lights.back();
            light.name      = aiLight->mName.C_Str();
            light.location  = Vec3<T>{ lightPosition.x, lightPosition.y, lightPosition.z };
            light.direction = Vec3<T>{ lightDirection.x, lightDirection.y, lightDirection.z };
            light.ambient   = Vec3<T>{ static_cast<T>(aiLight->mColorAmbient.r),
                                       static_cast<T>(aiLight->mColorAmbient.g),
                                       static_cast<T>(aiLight->mColorAmbient.b) };
            light.diffuse   = Vec3<T>{ aiLight->mColorDiffuse.r, aiLight->mColorDiffuse.g, aiLight->mColorDiffuse.b };
            light.specular  = Vec3<T>{ static_cast<T>(aiLight->mColorSpecular.r),
                                       static_cast<T>(aiLight->mColorSpecular.g),
                                       static_cast<T>(aiLight->mColorSpecular.b) };
            if (light.ambient.x == 0 && light.ambient.y == 0 && light.ambient.z == 0) {
                light.ambient = Vec3<T>{ 1.0f, 1.0f, 1.0f };
            }
            if (light.diffuse.x == 0 && light.diffuse.y == 0 && light.diffuse.z == 0) {
                light.diffuse = Vec3<T>{ 1.0f, 1.0f, 1.0f };
            }
            if (light.specular.x == 0 && light.specular.y == 0 && light.specular.z == 0) {
                light.specular = Vec3<T>{ 1.0f, 1.0f, 1.0f };
            }
            light.intensity            = static_cast<T>(2.0f);
            light.attenuationConstant  = static_cast<T>(aiLight->mAttenuationConstant);
            light.attenuationLinear    = static_cast<T>(aiLight->mAttenuationLinear);
            light.attenuationQuadratic = static_cast<T>(aiLight->mAttenuationQuadratic);
            light.type                 = aiLight->mType == aiLightSource_DIRECTIONAL
                                           ? ELightType_Directional
                                           : (aiLight->mType == aiLightSource_POINT ? ELightType_Point : ELightType_Spot);
            light.angleInnerCone       = aiLight->mAngleInnerCone;
            light.angleOuterCone       = aiLight->mAngleOuterCone;

            if (light.type == ELightType_Directional &&
                (light.direction.x == 0 && light.direction.y == 0 && light.direction.z == 0)) {
                light.direction = Vec3<T>{ 0.0f, -1.0f, 0.0f };
            }

            LOGV_DEBUG("\tType: {}",
                       (light.type == ELightType_Directional
                          ? "Directional"
                          : (light.type == ELightType_Point ? "Point" : "SpotLight")));
            LOGV_DEBUG("\tLocation: {}", light.location);
            LOGV_DEBUG("\tDirection: {}", light.direction);
            LOGV_DEBUG("\tAmbient: {}", light.ambient);
            LOGV_DEBUG("\tDiffuse: {}", light.diffuse);
            LOGV_DEBUG("\tSpecular: {}", light.specular);
            LOGV_DEBUG("\tIntensity: {}", light.intensity);
            LOGV_DEBUG("\tAttenuation: {}", light.attenuationConstant);
            LOGV_DEBUG("\tAttenuationLinear: {}", light.attenuationLinear);
            LOGV_DEBUG("\tAttenuationQuadratic: {}", light.attenuationQuadratic);
        }
    }

    // Recursively process child nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i],
                    aiScene,
                    globalTransform,
                    scene,
                    uniqueMeshNames,
                    uniqueMaterialNames,
                    uniqueCameraNames,
                    uniqueLightNames);
    }
}

template<typename T>
bool
importAsset(Scene<T>& scene)
{
    // Load the scene using Assimp
    Assimp::Importer importer;
    const aiScene*   aiScene = importer.ReadFile(scene.filepath.c_str(),
                                               aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace |
                                                 aiProcess_FlipWindingOrder);

    if (!aiScene || !aiScene->mRootNode || aiScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
        LOGV_ERROR("Error loading aiScene: {}", importer.GetErrorString());
        return false;
    }

    // Vectors to store meshes, cameras, and lights
    std::unordered_set<std::string> uniqueMeshNames;
    std::unordered_set<std::string> uniqueMaterialNames;
    std::unordered_set<std::string> uniqueCameraNames;
    std::unordered_set<std::string> uniqueLightNames;

    // Start processing the aiScene graph from the root node
    processNode(aiScene->mRootNode,
                aiScene,
                glm::mat<4, 4, T, glm::defaultp>(1.0f),
                scene,
                uniqueMeshNames,
                uniqueMaterialNames,
                uniqueCameraNames,
                uniqueLightNames);

    if (scene.cameras.size() == 0) {
        scene.cameras.push_back({});
        Camera<T>& camera = scene.cameras.back();
        camera.name       = "default";
        camera.fov        = static_cast<T>(60.0);
        camera.resolution = u32vec2{ WIDTH, HEIGHT };
        camera.zNearPlane = static_cast<T>(1);
        camera.zFarPlane  = static_cast<T>(100000);
        camera.location   = Vec3<T>{ static_cast<T>(0), static_cast<T>(0), static_cast<T>(0) };
        camera.target     = Vec3<T>{ static_cast<T>(0), static_cast<T>(0), static_cast<T>(1) };
        camera.up         = Vec3<T>{ static_cast<T>(0), static_cast<T>(1), static_cast<T>(0) };
        camera.updateMatrices();
    }

    if (scene.lights.size() == 0) {
        scene.lights.push_back({});
        Light<T>& light            = scene.lights.back();
        light.location             = scene.cameras[0].location;
        light.direction            = Vec3<T>{ static_cast<T>(0), static_cast<T>(0), static_cast<T>(-1) };
        light.ambient              = Vec3<T>{ static_cast<T>(1.0), static_cast<T>(1.0), static_cast<T>(1.0) };
        light.diffuse              = Vec3<T>{ static_cast<T>(100.0), static_cast<T>(100.0), static_cast<T>(100.0) };
        light.specular             = Vec3<T>{ static_cast<T>(100.0), static_cast<T>(100.0), static_cast<T>(100.0) };
        light.intensity            = static_cast<T>(100);
        light.attenuationConstant  = static_cast<T>(0.1);
        light.attenuationLinear    = static_cast<T>(0.01);
        light.attenuationQuadratic = static_cast<T>(0.001);
        light.type                 = ELightType_Point;
        light.angleInnerCone       = static_cast<T>(20);
        light.angleOuterCone       = static_cast<T>(45);
    }

    return true;
}

template<typename T>
bool
importDownloadedMeshAsset(Scene<T>& scene, const void* pBuffer, size_t pLength)
{
    printf("importDownloadedMeshAsset <%s>\n", scene.filepath.c_str());
    // Load the scene using Assimp
    Assimp::Importer importer;
    const aiScene*   aiScene = importer.ReadFileFromMemory(
      pBuffer,
      pLength,
      aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_FlipWindingOrder,
      std::filesystem::path(scene.filepath).extension().string().substr(1).c_str());

    if (!aiScene || !aiScene->mRootNode || aiScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
        LOGV_ERROR("Error loading aiScene: {}", importer.GetErrorString());
        return false;
    }

    // Vectors to store meshes, cameras, and lights
    std::unordered_set<std::string> uniqueMeshNames;
    std::unordered_set<std::string> uniqueMaterialNames;
    std::unordered_set<std::string> uniqueCameraNames;
    std::unordered_set<std::string> uniqueLightNames;

    // Start processing the aiScene graph from the root node
    processNode(aiScene->mRootNode,
                aiScene,
                glm::mat<4, 4, T, glm::defaultp>(1.0f),
                scene,
                uniqueMeshNames,
                uniqueMaterialNames,
                uniqueCameraNames,
                uniqueLightNames);

    if (scene.cameras.size() == 0) {
        scene.cameras.push_back({});
        Camera<T>& camera = scene.cameras.back();
        camera.name       = "default";
        camera.fov        = static_cast<T>(60.0);
        camera.resolution = u32vec2{ WIDTH, HEIGHT };
        camera.zNearPlane = static_cast<T>(1);
        camera.zFarPlane  = static_cast<T>(100000);
        camera.location   = Vec3<T>{ static_cast<T>(0), static_cast<T>(0), static_cast<T>(0) };
        camera.target     = Vec3<T>{ static_cast<T>(0), static_cast<T>(0), static_cast<T>(1) };
        camera.up         = Vec3<T>{ static_cast<T>(0), static_cast<T>(1), static_cast<T>(0) };
        camera.updateMatrices();
    }

    if (scene.lights.size() == 0) {
        scene.lights.push_back({});
        Light<T>& light            = scene.lights.back();
        light.location             = scene.cameras[0].location;
        light.direction            = Vec3<T>{ static_cast<T>(0), static_cast<T>(0), static_cast<T>(-1) };
        light.ambient              = Vec3<T>{ static_cast<T>(1.0), static_cast<T>(1.0), static_cast<T>(1.0) };
        light.diffuse              = Vec3<T>{ static_cast<T>(100.0), static_cast<T>(100.0), static_cast<T>(100.0) };
        light.specular             = Vec3<T>{ static_cast<T>(100.0), static_cast<T>(100.0), static_cast<T>(100.0) };
        light.intensity            = static_cast<T>(100);
        light.attenuationConstant  = static_cast<T>(0.1);
        light.attenuationLinear    = static_cast<T>(0.01);
        light.attenuationQuadratic = static_cast<T>(0.001);
        light.type                 = ELightType_Point;
        light.angleInnerCone       = static_cast<T>(20);
        light.angleOuterCone       = static_cast<T>(45);
    }

    return true;
}