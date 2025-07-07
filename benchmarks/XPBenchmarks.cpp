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

#include <DataPipeline/XPStore.h>
#include <Engine/XPEngine.h>
#include <Engine/XPRegistry.h>
#include <SceneDescriptor/XPLayer.h>
#include <SceneDescriptor/XPNode.h>
#include <SceneDescriptor/XPScene.h>
#include <SceneDescriptor/XPStore.h>
#include <Utilities/XPMemoryPool.h>
#include <benchmark/benchmark.h>

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Weverything"
#endif
#define FMT_HEADER_ONLY
#include <fmt/format.h>
#ifdef __clang__
#pragma clang diagnostic pop
#endif

#define NODES_COUNT 10000

static void
SCENE_DESCRIPTION_NODE_CREATION(benchmark::State& state)
{
    // Setup --------------------------------------------------------------------------------------
    auto engine                  = XP_NEW XPEngine();
    auto registry                = XP_NEW XPRegistry(engine);
    XPSDStore*             store = new XPSDStore(registry);
    XPScene*               scene = store->createScene("scene").value();
    XPLayer*               layer = scene->createLayer("layer").value();

    std::vector<std::string> names(NODES_COUNT);
    std::vector<XPNode*>     nodes(NODES_COUNT);
    for (size_t i = 0; i < NODES_COUNT; ++i) { names[i] = fmt::format("node {}", i); }
    // --------------------------------------------------------------------------------------------

    for (auto _ : state) {
        // Benchmarked code -----------------------------------------------------------------------
        for (size_t i = 0; i < NODES_COUNT; ++i) { nodes[i] = layer->createNode(names[i]).value(); }
        for (size_t i = 0; i < NODES_COUNT; ++i) { layer->destroyNode(nodes[i]); }
        // ----------------------------------------------------------------------------------------
    }

    // Cleanup ------------------------------------------------------------------------------------
    nodes.clear();
    names.clear();
    scene->destroyLayer(layer);
    store->destroyScene(scene);
    delete store;
    delete registry;
    delete engine;
    // --------------------------------------------------------------------------------------------
}

static void
SCENE_DESCRIPTION_NODE_FETCHING(benchmark::State& state)
{
    // Setup --------------------------------------------------------------------------------------
    auto engine                  = XP_NEW XPEngine();
    auto registry                = XP_NEW XPRegistry(engine);
    XPSDStore*             store = new XPSDStore(registry);
    XPScene*               scene = store->createScene("scene").value();
    XPLayer*               layer = scene->createLayer("layer").value();

    std::vector<std::string> names(NODES_COUNT);
    std::vector<XPNode*>     nodes(NODES_COUNT);
    for (size_t i = 0; i < NODES_COUNT; ++i) { names[i] = fmt::format("node {}", i); }
    // --------------------------------------------------------------------------------------------

    for (auto _ : state) {
        // Benchmarked code -----------------------------------------------------------------------
        for (size_t i = 0; i < NODES_COUNT; ++i) { nodes[i] = layer->createNode(names[i]).value(); }
        for (size_t i = 0; i < NODES_COUNT; ++i) {
            if (layer->getNode(names[i]).value() != nodes[i]) { throw std::runtime_error("Bug"); }
        }
        for (size_t i = 0; i < NODES_COUNT; ++i) { layer->destroyNode(nodes[i]); }
        // ----------------------------------------------------------------------------------------
    }

    // Cleanup ------------------------------------------------------------------------------------
    nodes.clear();
    names.clear();
    scene->destroyLayer(layer);
    store->destroyScene(scene);
    delete store;
    delete registry;
    delete engine;
    // --------------------------------------------------------------------------------------------
}

static void
SCENE_DESCRIPTION_NODE_DESTRUCTION(benchmark::State& state)
{
    // Setup --------------------------------------------------------------------------------------
    auto engine                  = XP_NEW XPEngine();
    auto registry                = XP_NEW XPRegistry(engine);
    XPSDStore*             store = new XPSDStore(registry);
    XPScene*               scene = store->createScene("scene").value();
    XPLayer*               layer = scene->createLayer("layer").value();

    std::vector<std::string> names(NODES_COUNT);
    std::vector<XPNode*>     nodes(NODES_COUNT);
    for (size_t i = 0; i < NODES_COUNT; ++i) { names[i] = fmt::format("node {}", i); }
    // --------------------------------------------------------------------------------------------

    for (auto _ : state) {
        // Benchmarked code -----------------------------------------------------------------------
        for (size_t i = 0; i < NODES_COUNT; ++i) { nodes[i] = layer->createNode(names[i]).value(); }
        for (size_t i = 0; i < NODES_COUNT; ++i) { layer->destroyNode(nodes[i]); }
        // ----------------------------------------------------------------------------------------
    }

    // Cleanup ------------------------------------------------------------------------------------
    nodes.clear();
    names.clear();
    scene->destroyLayer(layer);
    store->destroyScene(scene);
    delete store;
    delete registry;
    delete engine;
    // --------------------------------------------------------------------------------------------
}

// Register the function as a benchmark
BENCHMARK(SCENE_DESCRIPTION_NODE_CREATION);
BENCHMARK(SCENE_DESCRIPTION_NODE_FETCHING);
BENCHMARK(SCENE_DESCRIPTION_NODE_DESTRUCTION);

// Run the benchmark
BENCHMARK_MAIN();