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
#include <Utilities/XPMemoryPool.h>
#include <gtest/gtest.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Weverything"
#define FMT_HEADER_ONLY
#include <fmt/format.h>
#pragma clang diagnostic pop

#include <array>

class SceneDescriptionTests : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        engine   = XP_NEW XPEngine();
        console  = std::make_unique<XPConsole>(engine);
        registry = std::make_unique<XPRegistry>(engine);
        store    = new XPSDStore(registry.get());
    }
    void TearDown() override
    {
        delete store;
        delete engine;
    }

  public:
    XPEngine*                   engine   = nullptr;
    std::unique_ptr<XPConsole>  console  = nullptr;
    std::unique_ptr<XPRegistry> registry = nullptr;
    XPSDStore*                  store    = nullptr;
};

TEST_F(SceneDescriptionTests, Scene_Create)
{
    XPScene* scene = store->createScene("scene").value();
    EXPECT_TRUE(store->getScene("scene").has_value());
    EXPECT_NE(scene, nullptr);
    EXPECT_EQ(scene->getName(), "scene");
    EXPECT_EQ(scene->getId(), 1);
    EXPECT_EQ(scene->getStore(), store);
    EXPECT_NE(scene->getSceneStore(), nullptr);
    EXPECT_EQ(scene->getLayers().size(), 0);
    store->destroyScene(scene);
}

TEST_F(SceneDescriptionTests, Scene_Destroy)
{
    XPScene* scene = store->createScene("scene").value();
    store->destroyScene(scene);
    EXPECT_FALSE(store->getScene("scene").has_value());

    scene = store->getOrCreateScene("scene").value();
    store->destroyScene(scene->getName());
    EXPECT_FALSE(store->getScene("scene").has_value());
}

TEST_F(SceneDescriptionTests, Layer_Create)
{
    XPScene* scene = store->createScene("scene").value();
    XPLayer* layer = scene->getOrCreateLayer("layer").value();
    EXPECT_TRUE(scene->getLayer("layer").has_value());
    EXPECT_NE(layer, nullptr);
    EXPECT_EQ(layer->getName(), "layer");
    EXPECT_EQ(layer->getId(), 1);
    EXPECT_EQ(layer->getParent(), scene);
    EXPECT_EQ(layer->getNodes().size(), 0);
    scene->destroyLayer(layer);
    store->destroyScene(scene);
}

TEST_F(SceneDescriptionTests, Layer_Destroy)
{
    XPScene* scene = store->createScene("scene").value();
    XPLayer* layer = scene->getOrCreateLayer("layer").value();
    scene->destroyLayer(layer);
    EXPECT_FALSE(scene->getLayer("scene").has_value());

    layer = scene->getOrCreateLayer("layer").value();
    scene->destroyLayer(layer->getName());
    EXPECT_FALSE(scene->getLayer("scene").has_value());
    store->destroyScene(scene);
}

TEST_F(SceneDescriptionTests, Node_Create)
{
    XPScene* scene = store->createScene("scene").value();
    XPLayer* layer = scene->getOrCreateLayer("layer").value();
    XPNode*  node  = layer->getOrCreateNode("node").value();
    EXPECT_TRUE(layer->getNode("node").has_value());
    EXPECT_NE(node, nullptr);
    EXPECT_EQ(node->getName(), "node");
    EXPECT_EQ(node->getId(), 1);
    EXPECT_EQ(std::get<XPLayer*>(node->getParent()), layer);
    EXPECT_TRUE(std::holds_alternative<XPLayer*>(node->getParent()));
    EXPECT_FALSE(std::holds_alternative<XPNode*>(node->getParent()));
    EXPECT_TRUE(node->getParentLayer().has_value());
    EXPECT_FALSE(node->getParentNode().has_value());
    EXPECT_EQ(node->getNodes().size(), 0);

    XPNode* node2 = node->createNode("node2").value();
    EXPECT_TRUE(node->getNode("node2").has_value());
    EXPECT_NE(node2, nullptr);
    EXPECT_EQ(node2->getName(), "node2");
    EXPECT_EQ(node2->getId(), 2);
    EXPECT_EQ(std::get<XPNode*>(node2->getParent()), node);
    EXPECT_FALSE(std::holds_alternative<XPLayer*>(node2->getParent()));
    EXPECT_TRUE(std::holds_alternative<XPNode*>(node2->getParent()));
    EXPECT_FALSE(node2->getParentLayer().has_value());
    EXPECT_TRUE(node2->getParentNode().has_value());
    EXPECT_EQ(node2->getNodes().size(), 0);

    EXPECT_EQ(node->getNodes().size(), 1);

    node->destroyNode(node2);
    layer->destroyNode(node);
    scene->destroyLayer(layer);
    store->destroyScene(scene);
}

TEST_F(SceneDescriptionTests, Node_Destroy)
{
    XPScene* scene = store->createScene("scene").value();
    XPLayer* layer = scene->getOrCreateLayer("layer").value();
    XPNode*  node  = layer->getOrCreateNode("node").value();
    layer->destroyNode(node);
    EXPECT_FALSE(layer->getNode("node").has_value());

    node = layer->getOrCreateNode("node").value();
    layer->destroyNode(node->getName());
    EXPECT_FALSE(layer->getNode("node").has_value());

    node          = layer->getOrCreateNode("node").value();
    XPNode* node2 = node->createNode("node2").value();
    node->destroyNode(node2);
    EXPECT_FALSE(node->getNode("node2").has_value());

    node2 = node->createNode("node2").value();
    node->destroyNode(node2->getName());
    EXPECT_FALSE(node->getNode("node2").has_value());

    layer->destroyNode(node);
    scene->destroyLayer(layer);
    store->destroyScene(scene);
}

TEST_F(SceneDescriptionTests, Scene_Chaining)
{
    XPScene* scene = store->createScene("scene").value();
    XPLayer* layer = scene->getOrCreateLayer("layer").value();
    XPNode*  node  = layer->getOrCreateNode("node").value();
    XPNode*  node2 = node->createNode("node2").value();
    EXPECT_TRUE(store->getScene("scene").has_value());
    EXPECT_TRUE(scene->getLayer("layer").has_value());
    EXPECT_TRUE(layer->getNode("node").has_value());
    EXPECT_TRUE(node->getNode("node2").has_value());
    EXPECT_NE(node2, nullptr);
    store->destroyScene(scene);
    EXPECT_FALSE(store->getScene("scene").has_value());
    // TODO:
    // Having references that point to invalid data is currently possible after destroying it.
    // We need to use something similar to weak pointers to notify pointers holders that they are not valid anymore.
    // EXPECT_EQ(scene, nullptr);
    // EXPECT_EQ(layer, nullptr);
    // EXPECT_EQ(node, nullptr);
}

TEST_F(SceneDescriptionTests, Node_Multiple_Creation_And_Destruction)
{
    constexpr uint32_t       NODES_COUNT = 10000;
    XPScene*                 scene       = store->createScene("scene").value();
    XPLayer*                 layer       = scene->getOrCreateLayer("layer").value();
    std::vector<std::string> names(NODES_COUNT);
    std::vector<XPNode*>     nodes(NODES_COUNT);
    for (size_t i = 0; i < NODES_COUNT; ++i) {
        names[i] = fmt::format("node {}", i);
        nodes[i] = layer->createNode(names[i]).value();
    }
    for (size_t i = 0; i < NODES_COUNT; ++i) { EXPECT_EQ(layer->getNode(names[i]).value(), nodes[i]); }
    for (size_t i = 0; i < NODES_COUNT; ++i) { layer->destroyNode(nodes[i]); }
    scene->destroyLayer(layer);
    store->destroyScene(scene);
}