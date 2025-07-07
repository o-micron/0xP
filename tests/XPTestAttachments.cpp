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

#include <Engine/XPConsole.h>
#include <Engine/XPEngine.h>
#include <SceneDescriptor/XPLayer.h>
#include <SceneDescriptor/XPNode.h>
#include <SceneDescriptor/XPScene.h>
#include <SceneDescriptor/XPStore.h>
#include <gtest/gtest.h>

class AttachmentTests : public ::testing::Test
{
  public:
    AttachmentTests()
      : scene(nullptr)
      , layer(nullptr)
      , node(nullptr)
    {
    }

  protected:
    void SetUp() override
    {
        engine   = XP_NEW XPEngine();
        console  = std::make_unique<XPConsole>(engine);
        registry = std::make_unique<XPRegistry>(engine);
        store    = new XPSDStore(registry.get());
        scene    = store->createScene("scene").value();
        layer    = scene->createLayer("layer").value();
        node     = layer->createNode("node").value();
        scene->createNodeFilterPass(FreeCameraAttachmentDescriptor);
    }
    void TearDown() override
    {
        delete store;
        delete engine;
    }

    XPEngine*                   engine   = nullptr;
    std::unique_ptr<XPConsole>  console  = nullptr;
    std::unique_ptr<XPRegistry> registry = nullptr;
    XPSDStore*                  store    = nullptr;
    XPScene*                    scene    = nullptr;
    XPLayer*                    layer    = nullptr;
    XPNode*                     node     = nullptr;
};

TEST_F(AttachmentTests, AttachAll)
{
    XPAttachmentDescriptor allAttachmentDescriptor = 0;
    for (int i = 0; i < AttachmentsCount; ++i) { allAttachmentDescriptor |= AttachmentsTable[i].second; }

    node->attachAll();
    EXPECT_EQ(node->getAttachmentDescriptor(), allAttachmentDescriptor);
    EXPECT_TRUE(node->hasAttachments(allAttachmentDescriptor));
    EXPECT_TRUE(node->hasColliderAttachment());
    EXPECT_TRUE(node->hasFreeCameraAttachment());
    EXPECT_TRUE(node->hasLogicAttachment());
    EXPECT_TRUE(node->hasMeshRendererAttachment());
    EXPECT_TRUE(node->hasOrbitCameraAttachment());
    EXPECT_TRUE(node->hasRigidbodyAttachment());
    EXPECT_TRUE(node->hasTransformAttachment());
    EXPECT_NE(node->getCollider(), nullptr);
    EXPECT_NE(node->getFreeCamera(), nullptr);
    EXPECT_NE(node->getLogic(), nullptr);
    EXPECT_NE(node->getMeshRenderer(), nullptr);
    EXPECT_NE(node->getOrbitCamera(), nullptr);
    EXPECT_NE(node->getRigidbody(), nullptr);
    EXPECT_NE(node->getTransform(), nullptr);
}

TEST_F(AttachmentTests, DetachAll)
{
    XPAttachmentDescriptor allAttachmentDescriptor = 0;
    for (int i = 0; i < AttachmentsCount; ++i) { allAttachmentDescriptor |= AttachmentsTable[i].second; }

    node->attachAll();
    node->detachAll();
    EXPECT_EQ(node->getAttachmentDescriptor(), 0);
    EXPECT_FALSE(node->hasAttachments(allAttachmentDescriptor));
    EXPECT_FALSE(node->hasColliderAttachment());
    EXPECT_FALSE(node->hasFreeCameraAttachment());
    EXPECT_FALSE(node->hasLogicAttachment());
    EXPECT_FALSE(node->hasMeshRendererAttachment());
    EXPECT_FALSE(node->hasOrbitCameraAttachment());
    EXPECT_FALSE(node->hasRigidbodyAttachment());
    EXPECT_FALSE(node->hasTransformAttachment());
    EXPECT_EQ(node->getCollider(), nullptr);
    EXPECT_EQ(node->getFreeCamera(), nullptr);
    EXPECT_EQ(node->getLogic(), nullptr);
    EXPECT_EQ(node->getMeshRenderer(), nullptr);
    EXPECT_EQ(node->getOrbitCamera(), nullptr);
    EXPECT_EQ(node->getRigidbody(), nullptr);
    EXPECT_EQ(node->getTransform(), nullptr);
}

TEST_F(AttachmentTests, BasicAttachDetach)
{
    for (int i = 0; i < AttachmentsCount; ++i) {
        XPAttachmentDescriptor attachmentDescriptor = AttachmentsTable[i].second;
        node->attach(attachmentDescriptor);
        EXPECT_EQ(node->getAttachmentDescriptor(), attachmentDescriptor);
        EXPECT_TRUE(node->hasAttachments(attachmentDescriptor));
        node->detach(attachmentDescriptor);
        EXPECT_EQ(node->getAttachmentDescriptor(), 0);
        EXPECT_FALSE(node->hasAttachments(attachmentDescriptor));
    }
}

TEST_F(AttachmentTests, ComplexAttachDetach)
{
    XPAttachmentDescriptor allDescriptor = 0;
    for (int i = 0; i < AttachmentsCount; ++i) {
        XPAttachmentDescriptor attachmentDescriptor = AttachmentsTable[i].second;
        allDescriptor |= attachmentDescriptor;
    }

    for (int i = 0; i < AttachmentsCount; ++i) {
        XPAttachmentDescriptor attachmentDescriptor = AttachmentsTable[i].second;
        node->attachAll();
        EXPECT_EQ(node->getAttachmentDescriptor(), allDescriptor);
        node->detach(attachmentDescriptor);
        EXPECT_EQ(node->getAttachmentDescriptor(), allDescriptor & ~attachmentDescriptor);
        node->detachAll();
        node->attach(allDescriptor);
        EXPECT_EQ(node->getAttachmentDescriptor(), allDescriptor);
        node->detach(attachmentDescriptor);
        EXPECT_EQ(node->getAttachmentDescriptor(), allDescriptor & ~attachmentDescriptor);
    }
}

TEST_F(AttachmentTests, DuplicateAttachDetach)
{
    for (int i = 0; i < AttachmentsCount; ++i) {
        EXPECT_EQ(node->getAttachmentDescriptor(), 0);
        XPAttachmentDescriptor attachmentDescriptor = AttachmentsTable[i].second;
        node->attach(attachmentDescriptor);
        EXPECT_EQ(node->getAttachmentDescriptor(), attachmentDescriptor);
        node->detach(attachmentDescriptor);
        EXPECT_EQ(node->getAttachmentDescriptor(), 0);
        node->attach(attachmentDescriptor);
        node->attach(attachmentDescriptor);
        node->attach(attachmentDescriptor);
        EXPECT_EQ(node->getAttachmentDescriptor(), attachmentDescriptor);
        node->detach(attachmentDescriptor);
        node->detach(attachmentDescriptor);
        node->detach(attachmentDescriptor);
        EXPECT_EQ(node->getAttachmentDescriptor(), 0);
        node->detachAll();
    }
}

TEST_F(AttachmentTests, FetchingAttachmentPointers)
{
    node->attachFreeCamera();
    EXPECT_NE(node->getFreeCamera(), nullptr);
    EXPECT_EQ(node->getFreeCamera()->owner, node);
    node->attachCollider();
    EXPECT_NE(node->getCollider(), nullptr);
    EXPECT_EQ(node->getCollider()->owner, node);
    node->attachAll();
    EXPECT_NE(node->getCollider(), nullptr);
    EXPECT_EQ(node->getCollider()->owner, node);
    EXPECT_NE(node->getFreeCamera(), nullptr);
    EXPECT_EQ(node->getFreeCamera()->owner, node);
    EXPECT_NE(node->getLogic(), nullptr);
    EXPECT_EQ(node->getLogic()->owner, node);
    EXPECT_NE(node->getMeshRenderer(), nullptr);
    EXPECT_EQ(node->getMeshRenderer()->owner, node);
    EXPECT_NE(node->getOrbitCamera(), nullptr);
    EXPECT_EQ(node->getOrbitCamera()->owner, node);
    EXPECT_NE(node->getRigidbody(), nullptr);
    EXPECT_EQ(node->getRigidbody()->owner, node);
    EXPECT_NE(node->getTransform(), nullptr);
    EXPECT_EQ(node->getTransform()->owner, node);
    node->detachAll();
    EXPECT_EQ(node->getCollider(), nullptr);
    EXPECT_EQ(node->getFreeCamera(), nullptr);
    EXPECT_EQ(node->getLogic(), nullptr);
    EXPECT_EQ(node->getMeshRenderer(), nullptr);
    EXPECT_EQ(node->getOrbitCamera(), nullptr);
    EXPECT_EQ(node->getRigidbody(), nullptr);
    EXPECT_EQ(node->getTransform(), nullptr);
}

TEST_F(AttachmentTests, CheckMemoryDeallocation)
{
    XPNode* node2 = node->createNode("node2").value();
    EXPECT_EQ(node->getNodes().size(), 1);
    node2->attachAll();
    EXPECT_NE(node2->getCollider(), nullptr);
    EXPECT_EQ(node2->getCollider()->owner, node2);
    EXPECT_NE(node2->getFreeCamera(), nullptr);
    EXPECT_EQ(node2->getFreeCamera()->owner, node2);
    EXPECT_NE(node2->getLogic(), nullptr);
    EXPECT_EQ(node2->getLogic()->owner, node2);
    EXPECT_NE(node2->getMeshRenderer(), nullptr);
    EXPECT_EQ(node2->getMeshRenderer()->owner, node2);
    EXPECT_NE(node2->getOrbitCamera(), nullptr);
    EXPECT_EQ(node2->getOrbitCamera()->owner, node2);
    EXPECT_NE(node2->getRigidbody(), nullptr);
    EXPECT_EQ(node2->getRigidbody()->owner, node2);
    EXPECT_NE(node2->getTransform(), nullptr);
    EXPECT_EQ(node2->getTransform()->owner, node2);
    node->destroyNode(node2);
    EXPECT_EQ(node->getNodes().size(), 0);
    EXPECT_EQ(node2->getCollider(), nullptr);
    EXPECT_EQ(node2->getFreeCamera(), nullptr);
    EXPECT_EQ(node2->getLogic(), nullptr);
    EXPECT_EQ(node2->getMeshRenderer(), nullptr);
    EXPECT_EQ(node2->getOrbitCamera(), nullptr);
    EXPECT_EQ(node2->getRigidbody(), nullptr);
    EXPECT_EQ(node2->getTransform(), nullptr);

    // check detach all and destroy node
    node2 = node->createNode("node2").value();
    EXPECT_EQ(node->getNodes().size(), 1);
    EXPECT_NE(node2, nullptr);
    node2->attachAll();
    EXPECT_NE(node2->getCollider(), nullptr);
    EXPECT_EQ(node2->getCollider()->owner, node2);
    EXPECT_NE(node2->getFreeCamera(), nullptr);
    EXPECT_EQ(node2->getFreeCamera()->owner, node2);
    EXPECT_NE(node2->getLogic(), nullptr);
    EXPECT_EQ(node2->getLogic()->owner, node2);
    EXPECT_NE(node2->getMeshRenderer(), nullptr);
    EXPECT_EQ(node2->getMeshRenderer()->owner, node2);
    EXPECT_NE(node2->getOrbitCamera(), nullptr);
    EXPECT_EQ(node2->getOrbitCamera()->owner, node2);
    EXPECT_NE(node2->getRigidbody(), nullptr);
    EXPECT_EQ(node2->getRigidbody()->owner, node2);
    EXPECT_NE(node2->getTransform(), nullptr);
    EXPECT_EQ(node2->getTransform()->owner, node2);
    node2->detachAll();
    node->destroyNode(node2);
    EXPECT_EQ(node->getNodes().size(), 0);
    EXPECT_EQ(node2->getCollider(), nullptr);
    EXPECT_EQ(node2->getFreeCamera(), nullptr);
    EXPECT_EQ(node2->getLogic(), nullptr);
    EXPECT_EQ(node2->getMeshRenderer(), nullptr);
    EXPECT_EQ(node2->getOrbitCamera(), nullptr);
    EXPECT_EQ(node2->getRigidbody(), nullptr);
    EXPECT_EQ(node2->getTransform(), nullptr);
}

TEST_F(AttachmentTests, SceneFiltering)
{
    XPNode* cameraNode = layer->createNode("camera node").value();
    XPNode* meshNode1  = layer->createNode("mesh node 1").value();
    XPNode* meshNode2  = layer->createNode("mesh node 2").value();

    cameraNode->attachFreeCamera();

    meshNode1->attachMeshRenderer();
    meshNode1->attachTransform();

    meshNode2->attachMeshRenderer();
    meshNode2->attachTransform();
    meshNode2->attachCollider();

    auto cameraNodes = scene->getNodes(FreeCameraAttachmentDescriptor);

    auto meshNodes = scene->getNodes(TransformAttachmentDescriptor | MeshRendererAttachmentDescriptor);

    auto physicalMeshNodes =
      scene->getNodes(TransformAttachmentDescriptor | MeshRendererAttachmentDescriptor | ColliderAttachmentDescriptor);

    EXPECT_EQ(cameraNodes.size(), 1);
    EXPECT_EQ(meshNodes.size(), 2);
    EXPECT_EQ(physicalMeshNodes.size(), 1);

    EXPECT_NE(cameraNodes.find(cameraNode), cameraNodes.end());

    EXPECT_NE(meshNodes.find(meshNode1), meshNodes.end());
    EXPECT_NE(meshNodes.find(meshNode2), meshNodes.end());

    EXPECT_EQ(physicalMeshNodes.find(cameraNode), physicalMeshNodes.end());
    EXPECT_EQ(physicalMeshNodes.find(meshNode1), physicalMeshNodes.end());
    EXPECT_NE(physicalMeshNodes.find(meshNode2), physicalMeshNodes.end());

    layer->destroyNode(meshNode2);

    meshNodes = scene->getNodes(TransformAttachmentDescriptor | MeshRendererAttachmentDescriptor);

    physicalMeshNodes =
      scene->getNodes(TransformAttachmentDescriptor | MeshRendererAttachmentDescriptor | ColliderAttachmentDescriptor);

    EXPECT_EQ(physicalMeshNodes.size(), 0);
    EXPECT_EQ(meshNodes.size(), 1);

    layer->destroyNode(meshNode1);
    layer->destroyNode(cameraNode);
}