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

#include <DataPipeline/XPAssimpModelLoader.h>

#include <DataPipeline/XPDataPipelineStore.h>
#include <DataPipeline/XPEnums.h>
#include <DataPipeline/XPFile.h>
#include <DataPipeline/XPMaterialAsset.h>
#include <DataPipeline/XPMaterialBuffer.h>
#include <DataPipeline/XPMeshAsset.h>
#include <DataPipeline/XPMeshBuffer.h>
#include <DataPipeline/XPShaderBuffer.h>
#include <DataPipeline/XPTextureBuffer.h>
#include <Renderer/Interface/XPIRenderer.h>
#include <SceneDescriptor/XPAttachments.h>
#include <SceneDescriptor/XPLayer.h>
#include <SceneDescriptor/XPNode.h>
#include <SceneDescriptor/XPScene.h>
#include <SceneDescriptor/XPSceneDescriptorStore.h>
#include <Utilities/XPFS.h>
#include <Utilities/XPLogger.h>
#include <Utilities/XPMaths.h>

#include <functional>
#include <vector>

#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wall"
    #pragma clang diagnostic ignored "-Weverything"
#endif
#include <assimp/Importer.hpp>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <meshoptimizer.h>

#define FMT_HEADER_ONLY
#include <fmt/format.h>
#ifdef __clang__
    #pragma clang diagnostic pop
#endif

struct SceneTextureData
{};

XPProfilable void
XPAssimpModelLoader::loadModel(XPMeshAsset* meshAsset, XPDataPipelineStore& dataPipelineStore)
{
    unsigned int defaultFlags = aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals |
                                aiProcess_CalcTangentSpace | aiProcess_GenBoundingBoxes;
    Assimp::Importer importer;
    const aiScene*   assimpScene = importer.ReadFile(meshAsset->getFile()->getPath(), defaultFlags);
    if (!assimpScene) {
        XP_LOGV(XPLoggerSeverityWarning,
                "Mesh file not found %s\n%s",
                meshAsset->getFile()->getPath().c_str(),
                importer.GetErrorString());
        return;
    }
    if (assimpScene->mNumMeshes <= 0) {
        XP_LOGV(
          XPLoggerSeverityWarning, "Mesh Asset does not contain meshes %s", meshAsset->getFile()->getPath().c_str());
        return;
    }

    XPMeshBuffer* meshBuffer = meshAsset->getMeshBuffer();

    meshBuffer->deallocateResources();
    loadModelMetaData(meshAsset, dataPipelineStore, assimpScene);
    meshBuffer->allocateForResources();

    // load Materials
    std::vector<XPMaterialBuffer*> materialBuffers;
    materialBuffers.resize(assimpScene->mNumMaterials);
    {
        for (uint32_t m = 0; m < assimpScene->mNumMaterials; ++m) {
            const aiMaterial* aiMaterial = assimpScene->mMaterials[m];
            if (!aiMaterial) { continue; }
            std::string matName = aiMaterial->GetName().C_Str();

            loadMaterialTextures(aiMaterial, meshAsset, dataPipelineStore);

            auto optMaterialAsset = dataPipelineStore.createMaterialAsset(matName);
            if (!optMaterialAsset.has_value()) { continue; }
            auto optMaterialBuffer = dataPipelineStore.createMaterialBuffer(optMaterialAsset.value());
            if (!optMaterialBuffer.has_value()) { continue; }
            materialBuffers[m]        = optMaterialBuffer.value();
            XPPhongSystem phongSystem = {};
            {
                aiColor4D diffuse;
                if (AI_SUCCESS == aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_DIFFUSE, &diffuse)) {
                    phongSystem.diffuse = XPVec3<float>(diffuse.r, diffuse.g, diffuse.b);
                }
                aiColor4D ambient;
                if (AI_SUCCESS == aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_AMBIENT, &ambient)) {
                    phongSystem.ambient = XPVec3<float>(ambient.r, ambient.g, ambient.b);
                }
                aiColor4D specular;
                if (AI_SUCCESS == aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_SPECULAR, &specular)) {
                    phongSystem.specular = XPVec3<float>(specular.r, specular.g, specular.b);
                }
                aiColor4D emission;
                if (AI_SUCCESS == aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_EMISSIVE, &emission)) {
                    phongSystem.emission = XPVec3<float>(emission.r, emission.g, emission.b);
                }
            }
            materialBuffers[m]->setSystem(phongSystem);
        }
    }

    uint32_t numVertices = 0;
    uint32_t numIndices  = 0;
    // count vertices and indices
    for (uint32_t m = 0; m < assimpScene->mNumMeshes; ++m) {
        const aiMesh*       sceneMesh        = assimpScene->mMeshes[m];
        XPMeshBufferObject& meshBufferObject = meshBuffer->objectAtIndex(m);
        meshBufferObject.name                = std::string(sceneMesh->mName.C_Str());
        meshBufferObject.meshBuffer          = meshBuffer;
        meshBufferObject.materialBuffer      = materialBuffers[sceneMesh->mMaterialIndex];
        meshBufferObject.vertexOffset        = numVertices;
        meshBufferObject.indexOffset         = numIndices;
        meshBufferObject.numIndices          = sceneMesh->mNumFaces * 3;
        meshBufferObject.boundingBox =
          XPBoundingBox(XPVec4<float>(sceneMesh->mAABB.mMin.x, sceneMesh->mAABB.mMin.y, sceneMesh->mAABB.mMin.z, 1.0f),
                        XPVec4<float>(sceneMesh->mAABB.mMax.x, sceneMesh->mAABB.mMax.y, sceneMesh->mAABB.mMax.z, 1.0f));

        const aiVector3D ZERO_VECTOR(0.0f, 0.0f, 0.0f);
        // populate vertex attribute vectors
        for (uint32_t v = 0; v < sceneMesh->mNumVertices; ++v) {
            const aiVector3D& mPosition = sceneMesh->mVertices[v];
            const aiVector3D& mNormal   = sceneMesh->mNormals[v];
            const aiVector3D& mTexcoord =
              sceneMesh->HasTextureCoords(0) ? sceneMesh->mTextureCoords[0][v] : ZERO_VECTOR;
            size_t         offsetV  = v + numVertices;
            XPVec4<float>& position = meshBuffer->positionAtIndex(offsetV);
            XPVec4<float>& normal   = meshBuffer->normalAtIndex(offsetV);
            XPVec4<float>& texcoord = meshBuffer->texcoordAtIndex(offsetV);

            position.x = mPosition.x;
            position.y = mPosition.y;
            position.z = mPosition.z;
            position.w = 1.0f;

            normal.x = mNormal.x;
            normal.y = mNormal.y;
            normal.z = mNormal.z;
            normal.w = 1.0f;

            texcoord.x = mTexcoord.x;
            texcoord.y = mTexcoord.y;
            texcoord.z = 1.0f;
            texcoord.w = 1.0f;
        }

        // populate indices
        for (uint32_t f = 0; f < sceneMesh->mNumFaces; ++f) {
            const aiFace& face = sceneMesh->mFaces[f];
            if (face.mNumIndices == 2) continue;
            assert(face.mNumIndices == 3);
            size_t    offsetF = f * 3 + numIndices;
            uint32_t& index0  = meshBuffer->indexAtIndex(offsetF);
            index0            = face.mIndices[0];
            uint32_t& index1  = meshBuffer->indexAtIndex(offsetF + 1);
            index1            = face.mIndices[1];
            uint32_t& index2  = meshBuffer->indexAtIndex(offsetF + 2);
            index2            = face.mIndices[2];
        }

        numVertices += sceneMesh->mNumVertices;
        numIndices += meshBufferObject.numIndices;
    }

    //        // mesh optimize
    //        size_t                    index_count = sceneMesh->mNumFaces * 3;
    //        std::vector<unsigned int> remap(index_count);
    //        size_t                    vertex_count = meshopt_generateVertexRemap(
    //          &remap[0], nullptr, index_count, &positions[0], index_count, sizeof(positions[0]));
    //        positions.resize(vertex_count);
    //        indices.resize(index_count);
    //        // indexing
    //        meshopt_remapIndexBuffer(indices.data(), nullptr, index_count, &remap[0]);
    //        meshopt_remapVertexBuffer(positions.data(), &positions[0], index_count, sizeof(positions[0]), &remap[0]);
    //        // vertex cache optimization
    //        meshopt_optimizeVertexCache(indices.data(), indices.data(), index_count, vertex_count);
    //        // reduce overdraw
    //        //        meshopt_optimizeOverdraw(
    //        //          indices.data(), indices.data(), index_count, &positions[0].x, vertex_count,
    //        //          sizeof(positions[0]), 1.05f);
    //        optMeshBuffer.value()->setName(sceneMesh->mName.C_Str());
    //        optMeshBuffer.value()->setPositions(std::move(positions));
    //        optMeshBuffer.value()->setNormals(std::move(normals));
    //        optMeshBuffer.value()->setTexcoords(std::move(coords));
    //        optMeshBuffer.value()->setIndices(std::move(indices));
}

XPProfilable void
XPAssimpModelLoader::loadScene(XPMeshAsset* meshAsset, XPScene& scene, XPDataPipelineStore& dataPipelineStore)
{
    XP_UNUSED(dataPipelineStore)

    std::function<void(XPMeshAsset * meshAsset, XPLayer * layer, XPNode*, const aiScene*, const aiNode*)> processNode =
      [&](XPMeshAsset*   meshAsset,
          XPLayer*       layer,
          XPNode*        parentNode,
          const aiScene* assimpScene,
          const aiNode*  assimpNode) {
          XP_UNUSED(parentNode)

          if (!assimpNode) { return; }

          C_STRUCT aiMatrix4x4 mat = assimpNode->mTransformation;
          aiTransposeMatrix4(&mat);

          std::optional<XPNode*> optNode;

          if (assimpNode->mNumMeshes > 0) {
              static size_t emptyNodesCounter = 0;
              std::string   fixedNodeName     = strlen(assimpNode->mName.C_Str()) > 0
                                                  ? assimpNode->mName.C_Str()
                                                  : fmt::format("empty_node {}", ++emptyNodesCounter);
              optNode                         = layer->createNode(fixedNodeName);
              if (!optNode.has_value()) {
                  static uint32_t nodeId = 0;
                  optNode                = layer->createNode(fmt::format("node_{}", ++nodeId));
                  if (!optNode.has_value()) {
                      XP_LOGV(XPLoggerSeverityFatal,
                              "Couldn't create a new scene node from parent layer <%s>",
                              layer->getName().c_str());
                      return;
                  }
              }

              XPNode* node = optNode.value();
              {
                  glm::vec3 scale;
                  glm::vec3 position;
                  glm::vec3 rotation;
                  glm::quat orientation;
                  glm::vec3 skew;
                  glm::vec4 perspective;
                  glm::decompose(
                    *reinterpret_cast<const glm::mat4*>(&mat[0][0]), scale, orientation, position, skew, perspective);
                  rotation = glm::degrees(glm::eulerAngles(orientation));
                  node->attachTransform();
                  node->getTransform()->location = XPVec3<float>(position.x, position.y, position.z);
                  node->getTransform()->euler    = XPVec3<float>(rotation.x, rotation.y, rotation.z);
                  node->getTransform()->scale    = XPVec3<float>(scale.x, scale.y, scale.z);
              }

              const auto& meshBufferObjects = meshAsset->getMeshBuffer()->getObjects();
              node->attachMeshRenderer();
              MeshRenderer* mr = node->getMeshRenderer();
              mr->info.resize(assimpNode->mNumMeshes);
              for (unsigned int i = 0; i < assimpNode->mNumMeshes; ++i) {
                  const XPMeshBufferObject& meshBufferObject = meshBufferObjects[assimpNode->mMeshes[i]];
                  XPMeshRendererInfo&       mrinfo           = mr->info[i];
                  mrinfo.material.text                       = meshBufferObject.materialBuffer
                                                                 ? meshBufferObject.materialBuffer->getMaterialAsset()->getName()
                                                                 : "default";
                  mrinfo.material.inputBuffer                = mrinfo.material.text;
                  mrinfo.polygonMode                         = XPEMeshRendererPolygonModeFill;
                  mrinfo.mesh.text                           = meshBufferObject.name;
                  mrinfo.mesh.inputBuffer                    = mrinfo.mesh.text;
                  mrinfo.meshBuffer                          = meshAsset->getMeshBuffer();
                  mrinfo.meshBufferObjectIndex               = assimpNode->mMeshes[i];
              }
              // by the time we call node->attachCollider() the collider infos
              // would have already matched the mesh renderer info
              // I here want to overwrite that to make them all triMeshShapes instead.
              // If I don't continue on setting the clinfo then by default we would get
              // simple boxShapes ..
              node->attachCollider();
              Collider* cl = node->getCollider();
              cl->info.resize(assimpNode->mNumMeshes);
              for (unsigned int i = 0; i < assimpNode->mNumMeshes; ++i) {
                  const XPMeshBufferObject& meshBufferObject = meshBufferObjects[assimpNode->mMeshes[i]];
                  XPColliderInfo&           clinfo           = cl->info[i];
                  clinfo.meshBuffer                          = meshAsset->getMeshBuffer();
                  clinfo.meshBufferObjectIndex               = assimpNode->mMeshes[i];
                  clinfo.shapeName.text                      = meshBufferObject.name;
                  clinfo.shapeName.inputBuffer               = clinfo.shapeName.text;
                  if (clinfo.shapeName.text == "Cube.001") {
                      const XPBoundingBox& boundingBox = meshBufferObject.boundingBox;
                      clinfo.parameters.boxWidth       = abs(boundingBox.maxPoint.x - boundingBox.minPoint.x) / 2.0f;
                      clinfo.parameters.boxHeight      = abs(boundingBox.maxPoint.y - boundingBox.minPoint.y) / 2.0f;
                      clinfo.parameters.boxDepth       = abs(boundingBox.maxPoint.z - boundingBox.minPoint.z) / 2.0f;
                      clinfo.shape                     = XPEColliderShapeBox;
                  } else {
                      clinfo.shape = XPEColliderShapeTriangleMesh;
                  }
              }
              // this should call back to the physics backend since we now have attached both collider and rigidbody
              node->attachRigidbody();
          }

          for (uint32_t i = 0; i < assimpNode->mNumChildren; ++i) {
              processNode(meshAsset,
                          layer,
                          optNode.has_value() ? optNode.value() : nullptr,
                          assimpScene,
                          assimpNode->mChildren[i]);
          }
      };

    unsigned int defaultFlags = aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals |
                                aiProcess_CalcTangentSpace | aiProcess_GenBoundingBoxes;
    Assimp::Importer importer;
    const aiScene*   assimpScene = importer.ReadFile(meshAsset->getFile()->getPath(), defaultFlags);
    if (!assimpScene) {
        XP_LOGV(XPLoggerSeverityFatal,
                "Couldn't import scene from file, file not found <%s>\n%s",
                meshAsset->getFile()->getPath().c_str(),
                importer.GetErrorString());
        return;
    }
    if (assimpScene->mNumMeshes <= 0) {
        XP_LOGV(
          XPLoggerSeverityWarning, "Mesh Asset does not contain meshes %s", meshAsset->getFile()->getPath().c_str());
        return;
    }

    XPLayer* layer = scene.getOrCreateLayer("layer").value();
    processNode(meshAsset, layer, nullptr, assimpScene, assimpScene->mRootNode);
    auto cameraNode = layer->getOrCreateNode("CameraNode").value();
    {
        cameraNode->attachFreeCamera();
        FreeCamera* camera = cameraNode->getFreeCamera();

        if (assimpScene->mNumCameras > 0) {
            aiNode*              rootNode                   = assimpScene->mRootNode;
            aiNode*              cameraNode                 = rootNode->FindNode(assimpScene->mCameras[0]->mName);
            aiMatrix4x4          cameraTransformationMatrix = cameraNode->mTransformation;
            C_STRUCT aiMatrix4x4 mat                        = cameraTransformationMatrix;
            aiTransposeMatrix4(&mat);

            glm::vec3 scale;
            glm::vec3 position;
            glm::vec3 rotation;
            glm::quat orientation;
            glm::vec3 skew;
            glm::vec4 perspective;
            glm::decompose(
              *reinterpret_cast<const glm::mat4*>(&mat[0][0]), scale, orientation, position, skew, perspective);
            rotation = glm::degrees(glm::eulerAngles(orientation));

            camera->activeProperties.location = XPVec3<float>(position.x, position.y, position.z);
            camera->activeProperties.euler    = XPVec3<float>(rotation.x, rotation.y, rotation.z);
            camera->activeProperties.zfar     = assimpScene->mCameras[0]->mClipPlaneFar;
            camera->activeProperties.znear    = assimpScene->mCameras[0]->mClipPlaneNear;
            camera->activeProperties.fov      = glm::degrees(assimpScene->mCameras[0]->mHorizontalFOV);
        } else {
            camera->activeProperties.location = XPVec3<float>(0.0f, 0.0f, 10.0f);
            camera->activeProperties.euler    = XPVec3<float>(0.0f, 0.0f, 0.0f);
            camera->activeProperties.zfar     = 1000.0f;
            camera->activeProperties.znear    = 0.1f;
            camera->activeProperties.fov      = 60.0f;
        }
        camera->linearVelocity          = XPVec3<float>(0.0);
        camera->linearRotation          = XPVec2<float>(0.0);
        camera->interpolationTime       = 0.0f;
        const auto& resolution          = scene.getRegistry()->getRenderer()->getResolution();
        camera->activeProperties.width  = resolution.x;
        camera->activeProperties.height = resolution.y;
        camera->lookSensitivity         = 0.1f;
        camera->moveSensitivity         = 1.0f;

        XPMat4<float>::buildViewProjectionMatrices(camera->activeProperties.viewProjectionMatrix,
                                                   camera->activeProperties.inverseViewProjectionMatrix,
                                                   camera->activeProperties.viewMatrix,
                                                   camera->activeProperties.inverseViewMatrix,
                                                   camera->activeProperties.projectionMatrix,
                                                   camera->activeProperties.location,
                                                   camera->activeProperties.euler,
                                                   camera->activeProperties.fov,
                                                   camera->activeProperties.width,
                                                   camera->activeProperties.height,
                                                   camera->activeProperties.znear,
                                                   camera->activeProperties.zfar);
        memcpy(&camera->frozenProperties, &camera->activeProperties, sizeof(CameraProperties));
    }
}

XPProfilable void
XPAssimpModelLoader::loadModelMetaData(XPMeshAsset*         meshAsset,
                                       XPDataPipelineStore& dataPipelineStore,
                                       const aiScene*       scene)
{
    XPMeshBuffer* meshBuffer = meshAsset->getMeshBuffer();

    uint32_t numVertices = 0;
    uint32_t numIndices  = 0;

    // count vertices and indices
    for (uint32_t m = 0; m < scene->mNumMeshes; ++m) {
        const aiMesh* sceneMesh = scene->mMeshes[m];
        numVertices += sceneMesh->mNumVertices;
        numIndices += sceneMesh->mNumFaces * 3;
    }

    meshBuffer->setPositionsCount(numVertices);
    meshBuffer->setNormalsCount(numVertices);
    meshBuffer->setTexcoordsCount(numVertices);
    meshBuffer->setIndicesCount(numIndices);
    meshBuffer->setObjectsCount(scene->mNumMeshes);
}

XPProfilable void
XPAssimpModelLoader::loadMaterialTextures(const aiMaterial*    aiMaterial,
                                          XPMeshAsset*         meshAsset,
                                          XPDataPipelineStore& dataPipelineStore)
{
    for (uint32_t t = 0; t < aiMaterial->GetTextureCount(aiTextureType_DIFFUSE); ++t) {
        aiString path;
        aiMaterial->GetTexture(aiTextureType_DIFFUSE, t, &path);
        if (XPFS::isFile(path.C_Str())) {
            std::string textureFilePath = path.C_Str();
            dataPipelineStore.createFile(textureFilePath, XPEFileResourceType::Texture);
        } else {
            // try relative path to the asset location
            std::filesystem::path assetPath(meshAsset->getFile()->getPath());
            std::filesystem::path assetDir = assetPath.parent_path();
            assetDir /= path.C_Str();
            if (XPFS::isFile(assetDir.string().c_str())) {
                std::string textureFilePath = assetDir.string();
                dataPipelineStore.createFile(textureFilePath, XPEFileResourceType::Texture);
            }
        }
    }

    for (uint32_t t = 0; t < aiMaterial->GetTextureCount(aiTextureType_AMBIENT); ++t) {
        aiString path;
        aiMaterial->GetTexture(aiTextureType_AMBIENT, t, &path);
        if (XPFS::isFile(path.C_Str())) {
            std::string textureFilePath = path.C_Str();
            dataPipelineStore.createFile(textureFilePath, XPEFileResourceType::Texture);
        } else {
            // try relative path to the asset location
            std::filesystem::path assetPath(meshAsset->getFile()->getPath());
            std::filesystem::path assetDir = assetPath.parent_path();
            assetDir /= path.C_Str();
            if (XPFS::isFile(assetDir.string().c_str())) {
                std::string textureFilePath = assetDir.string();
                dataPipelineStore.createFile(textureFilePath, XPEFileResourceType::Texture);
            }
        }
    }

    for (uint32_t t = 0; t < aiMaterial->GetTextureCount(aiTextureType_NORMALS); ++t) {
        aiString path;
        aiMaterial->GetTexture(aiTextureType_NORMALS, t, &path);
        if (XPFS::isFile(path.C_Str())) {
            std::string textureFilePath = path.C_Str();
            dataPipelineStore.createFile(textureFilePath, XPEFileResourceType::Texture);
        } else {
            // try relative path to the asset location
            std::filesystem::path assetPath(meshAsset->getFile()->getPath());
            std::filesystem::path assetDir = assetPath.parent_path();
            assetDir /= path.C_Str();
            if (XPFS::isFile(assetDir.string().c_str())) {
                std::string textureFilePath = assetDir.string();
                dataPipelineStore.createFile(textureFilePath, XPEFileResourceType::Texture);
            }
        }
    }

    for (uint32_t t = 0; t < aiMaterial->GetTextureCount(aiTextureType_SPECULAR); ++t) {
        aiString path;
        aiMaterial->GetTexture(aiTextureType_SPECULAR, t, &path);
        if (XPFS::isFile(path.C_Str())) {
            std::string textureFilePath = path.C_Str();
            dataPipelineStore.createFile(textureFilePath, XPEFileResourceType::Texture);
        } else {
            // try relative path to the asset location
            std::filesystem::path assetPath(meshAsset->getFile()->getPath());
            std::filesystem::path assetDir = assetPath.parent_path();
            assetDir /= path.C_Str();
            if (XPFS::isFile(assetDir.string().c_str())) {
                std::string textureFilePath = assetDir.string();
                dataPipelineStore.createFile(textureFilePath, XPEFileResourceType::Texture);
            }
        }
    }

    for (uint32_t t = 0; t < aiMaterial->GetTextureCount(aiTextureType_EMISSIVE); ++t) {
        aiString path;
        aiMaterial->GetTexture(aiTextureType_EMISSIVE, t, &path);
        if (XPFS::isFile(path.C_Str())) {
            std::string textureFilePath = path.C_Str();
            dataPipelineStore.createFile(textureFilePath, XPEFileResourceType::Texture);
        } else {
            // try relative path to the asset location
            std::filesystem::path assetPath(meshAsset->getFile()->getPath());
            std::filesystem::path assetDir = assetPath.parent_path();
            assetDir /= path.C_Str();
            if (XPFS::isFile(assetDir.string().c_str())) {
                std::string textureFilePath = assetDir.string();
                dataPipelineStore.createFile(textureFilePath, XPEFileResourceType::Texture);
            }
        }
    }
}
