#!/bin/bash

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    rm -rf artifacts/linux/
elif [[ "$OSTYPE" == "darwin"* ]]; then
    rm -rf artifacts/macOS/
fi

mkdir artifacts
cd artifacts

# -------------------------------------------------------------------------------------------------
# Download artifacts
# -------------------------------------------------------------------------------------------------
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    gh release download v0.0.1 --repo o-micron/0xTP --pattern Assimp-linux-x64.tar.gz
    mkdir "./assimp"
    tar -xzf "Assimp-linux-x64.tar.gz" -C "./assimp"
    gh release download v0.0.1 --repo o-micron/0xTP --pattern Boost-linux-x64.tar.gz
    mkdir "./boost"
    tar -xzf "Boost-linux-x64.tar.gz" -C "./boost"
    gh release download v0.0.1 --repo o-micron/0xTP --pattern FMT-linux-x64.tar.gz
    mkdir "./fmt"
    tar -xzf "FMT-linux-x64.tar.gz" -C "./fmt"
    gh release download v0.0.1 --repo o-micron/0xTP --pattern FSWatch-linux-x64.tar.gz
    mkdir "./fswatch"
    tar -xzf "FSWatch-linux-x64.tar.gz" -C "./fswatch"
    gh release download v0.0.1 --repo o-micron/0xTP --pattern GfxReconstruct-linux-x64.tar.gz
    mkdir "./gfxreconstruct"
    tar -xzf "GfxReconstruct-linux-x64.tar.gz" -C "./gfxreconstruct"
    gh release download v0.0.1 --repo o-micron/0xTP --pattern GLM-linux-x64.tar.gz
    mkdir "./glm"
    tar -xzf "GLM-linux-x64.tar.gz" -C "./glm"
    gh release download v0.0.1 --repo o-micron/0xTP --pattern ImGui-linux-x64.tar.gz
    mkdir "./imgui"
    tar -xzf "ImGui-linux-x64.tar.gz" -C "./imgui"
    gh release download v0.0.1 --repo o-micron/0xTP --pattern Implot-linux-x64.tar.gz
    mkdir "./implot"
    tar -xzf "Implot-linux-x64.tar.gz" -C "./implot"
    gh release download v0.0.1 --repo o-micron/0xTP --pattern Inja-linux-x64.tar.gz
    mkdir "./inja"
    tar -xzf "Inja-linux-x64.tar.gz" -C "./inja"
    gh release download v0.0.1 --repo o-micron/0xTP --pattern JoltPhysics-linux-x64.tar.gz
    mkdir "./joltphysics"
    tar -xzf "JoltPhysics-linux-x64.tar.gz" -C "./joltphysics"
    gh release download v0.0.1 --repo o-micron/0xTP --pattern Json-linux-x64.tar.gz
    mkdir "./json"
    tar -xzf "Json-linux-x64.tar.gz" -C "./json"
    gh release download v0.0.1 --repo o-micron/0xTP --pattern Kaguya-linux-x64.tar.gz
    mkdir "./kaguya"
    tar -xzf "Kaguya-linux-x64.tar.gz" -C "./kaguya"
    gh release download v0.0.1 --repo o-micron/0xTP --pattern LLVM-linux-x64.tar.gz
    mkdir "./llvm"
    tar -xzf "LLVM-linux-x64.tar.gz" -C "./llvm"
    gh release download v0.0.1 --repo o-micron/0xTP --pattern Lua-linux-x64.tar.gz
    mkdir "./lua"
    tar -xzf "Lua-linux-x64.tar.gz" -C "./lua"
    gh release download v0.0.1 --repo o-micron/0xTP --pattern MeshOptimizer-linux-x64.tar.gz
    mkdir "./meshoptimizer"
    tar -xzf "MeshOptimizer-linux-x64.tar.gz" -C "./meshoptimizer"
    gh release download v0.0.1 --repo o-micron/0xTP --pattern SDL2-linux-x64.tar.gz
    mkdir "./SDL"
    tar -xzf "SDL2-linux-x64.tar.gz" -C "./SDL"
    gh release download v0.0.1 --repo o-micron/0xTP --pattern SpirvCross-linux-x64.tar.gz
    mkdir "./SPIRV-Cross"
    tar -xzf "SpirvCross-linux-x64.tar.gz" -C "./SPIRV-Cross"
    gh release download v0.0.1 --repo o-micron/0xTP --pattern Stb-linux-x64.tar.gz
    mkdir "./stb"
    tar -xzf "Stb-linux-x64.tar.gz" -C "./stb"
    gh release download v0.0.1 --repo o-micron/0xTP --pattern TinyExr-linux-x64.tar.gz 
    mkdir "./tinyexr"
    tar -xzf "TinyExr-linux-x64.tar.gz" -C "./tinyexr"
    gh release download v0.0.1 --repo o-micron/0xTP --pattern Tracy-linux-x64.tar.gz 
    mkdir "./tracy"
    tar -xzf "Tracy-linux-x64.tar.gz" -C "./tracy"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    gh release download v0.0.1 --repo o-micron/0xTP --pattern Assimp-macos-arm64.tar.gz
    mkdir "./assimp"
    tar -xzf "Assimp-macos-arm64.tar.gz" -C "./assimp"
    gh release download v0.0.1 --repo o-micron/0xTP --pattern Boost-macos-arm64.tar.gz
    mkdir "./boost"
    tar -xzf "Boost-macos-arm64.tar.gz" -C "./boost"
    gh release download v0.0.1 --repo o-micron/0xTP --pattern FMT-macos-arm64.tar.gz
    mkdir "./fmt"
    tar -xzf "FMT-macos-arm64.tar.gz" -C "./fmt"
    gh release download v0.0.1 --repo o-micron/0xTP --pattern FSWatch-macos-arm64.tar.gz
    mkdir "./fswatch"
    tar -xzf "FSWatch-macos-arm64.tar.gz" -C "./fswatch"
    gh release download v0.0.1 --repo o-micron/0xTP --pattern GfxReconstruct-macos-arm64.tar.gz
    mkdir "./gfxreconstruct"
    tar -xzf "GfxReconstruct-macos-arm64.tar.gz" -C "./gfxreconstruct"
    gh release download v0.0.1 --repo o-micron/0xTP --pattern GLM-macos-arm64.tar.gz
    mkdir "./glm"
    tar -xzf "GLM-macos-arm64.tar.gz" -C "./glm"
    gh release download v0.0.1 --repo o-micron/0xTP --pattern ImGui-macos-arm64.tar.gz
    mkdir "./imgui"
    tar -xzf "ImGui-macos-arm64.tar.gz" -C "./imgui"
    gh release download v0.0.1 --repo o-micron/0xTP --pattern Implot-macos-arm64.tar.gz
    mkdir "./implot"
    tar -xzf "Implot-macos-arm64.tar.gz" -C "./implot"
    gh release download v0.0.1 --repo o-micron/0xTP --pattern Inja-macos-arm64.tar.gz
    mkdir "./inja"
    tar -xzf "Inja-macos-arm64.tar.gz" -C "./inja"
    gh release download v0.0.1 --repo o-micron/0xTP --pattern JoltPhysics-macos-arm64.tar.gz
    mkdir "./joltphysics"
    tar -xzf "JoltPhysics-macos-arm64.tar.gz" -C "./joltphysics"
    gh release download v0.0.1 --repo o-micron/0xTP --pattern Json-macos-arm64.tar.gz
    mkdir "./json"
    tar -xzf "Json-macos-arm64.tar.gz" -C "./json"
    gh release download v0.0.1 --repo o-micron/0xTP --pattern Kaguya-macos-arm64.tar.gz
    mkdir "./kaguya"
    tar -xzf "Kaguya-macos-arm64.tar.gz" -C "./kaguya"
    gh release download v0.0.1 --repo o-micron/0xTP --pattern LLVM-macos-arm64.tar.gz
    mkdir "./llvm"
    tar -xzf "LLVM-macos-arm64.tar.gz" -C "./llvm"
    gh release download v0.0.1 --repo o-micron/0xTP --pattern Lua-macos-arm64.tar.gz
    mkdir "./lua"
    tar -xzf "Lua-macos-arm64.tar.gz" -C "./lua"
    gh release download v0.0.1 --repo o-micron/0xTP --pattern MeshOptimizer-macos-arm64.tar.gz
    mkdir "./meshoptimizer"
    tar -xzf "MeshOptimizer-macos-arm64.tar.gz" -C "./meshoptimizer"
    gh release download v0.0.1 --repo o-micron/0xTP --pattern SDL2-macos-arm64.tar.gz
    mkdir "./SDL"
    tar -xzf "SDL2-macos-arm64.tar.gz" -C "./SDL"
    gh release download v0.0.1 --repo o-micron/0xTP --pattern SpirvCross-macos-arm64.tar.gz
    mkdir "./SPIRV-Cross"
    tar -xzf "SpirvCross-macos-arm64.tar.gz" -C "./SPIRV-Cross"
    gh release download v0.0.1 --repo o-micron/0xTP --pattern Stb-macos-arm64.tar.gz
    mkdir "./stb"
    tar -xzf "Stb-macos-arm64.tar.gz" -C "./stb"
    gh release download v0.0.1 --repo o-micron/0xTP --pattern TinyExr-macos-arm64.tar.gz 
    mkdir "./tinyexr"
    tar -xzf "TinyExr-macos-arm64.tar.gz" -C "./tinyexr"
    gh release download v0.0.1 --repo o-micron/0xTP --pattern Tracy-macos-arm64.tar.gz 
    mkdir "./tracy"
    tar -xzf "Tracy-macos-arm64.tar.gz" -C "./tracy"
fi
# -------------------------------------------------------------------------------------------------


# -------------------------------------------------------------------------------------------------
# Copy artifacts
# -------------------------------------------------------------------------------------------------
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    rm -rf ./linux/
    mkdir -p ./linux/compiler/
    mkdir -p ./linux/bin/
    mkdir -p ./linux/include/
    mkdir -p ./linux/lib/
    mkdir -p ./linux/src/

    cp -rf "./assimp/" "./linux/"
    cp -rf "./boost/" "./linux/"
    cp -rf "./fmt/" "./linux/"
    cp -rf "./fswatch/" "./linux/"
    cp -rf "./gfxreconstruct/" "./linux/"
    cp -rf "./glm/" "./linux/"
    cp -rf "./imgui/" "./linux/"
    cp -rf "./implot/" "./linux/"
    cp -rf "./inja/" "./linux/"
    cp -rf "./joltphysics/" "./linux/"
    cp -rf "./json/" "./linux/"
    cp -rf "./kaguya/" "./linux/"
    cp -rf "./llvm/" "./linux/compiler/"
    cp -rf "./lua/" "./linux/"
    cp -rf "./meshoptimizer/" "./linux/"
    cp -rf "./SDL/" "./linux/"
    cp -rf "./SPIRV-Cross/" "./linux/"
    cp -rf "./stb/" "./linux/"
    cp -rf "./tinyexr/" "./linux/"
    cp -rf "./tracy/" "./linux/"

    rm -rf "assimp"
    rm -rf "boost"
    rm -rf "fmt"
    rm -rf "fswatch"
    rm -rf "gfxreconstruct"
    rm -rf "glm"
    rm -rf "imgui"
    rm -rf "implot"
    rm -rf "inja"
    rm -rf "joltphysics"
    rm -rf "json"
    rm -rf "kaguya"
    rm -rf "llvm"
    rm -rf "lua"
    rm -rf "meshoptimizer"
    rm -rf "SDL"
    rm -rf "SPIRV-Cross"
    rm -rf "stb"
    rm -rf "tinyexr"
    rm -rf "tracy"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    rm -f ./macOS/
    mkdir -p ./macOS/compiler/
    mkdir -p ./macOS/bin/
    mkdir -p ./macOS/include/
    mkdir -p ./macOS/lib/
    mkdir -p ./macOS/src/
    
    cp -rf "./assimp/" "./macOS/"
    cp -rf "./boost/" "./macOS/"
    cp -rf "./fmt/" "./macOS/"
    cp -rf "./fswatch/" "./macOS/"
    cp -rf "./gfxreconstruct/" "./macOS/"
    cp -rf "./glm/" "./macOS/"
    cp -rf "./imgui/" "./macOS/"
    cp -rf "./implot/" "./macOS/"
    cp -rf "./inja/" "./macOS/"
    cp -rf "./joltphysics/" "./macOS/"
    cp -rf "./json/" "./macOS/"
    cp -rf "./kaguya/" "./macOS/"
    cp -rf "./llvm/" "./macOS/compiler/"
    cp -rf "./lua/" "./macOS/"
    cp -rf "./meshoptimizer/" "./macOS/"
    cp -rf "./SDL/" "./macOS/"
    cp -rf "./SPIRV-Cross/" "./macOS/"
    cp -rf "./stb/" "./macOS/"
    cp -rf "./tinyexr/" "./macOS/"
    cp -rf "./tracy/" "./macOS/"

    rm -rf "assimp"
    rm -rf "boost"
    rm -rf "fmt"
    rm -rf "fswatch"
    rm -rf "gfxreconstruct"
    rm -rf "glm"
    rm -rf "imgui"
    rm -rf "implot"
    rm -rf "inja"
    rm -rf "joltphysics"
    rm -rf "json"
    rm -rf "kaguya"
    rm -rf "llvm"
    rm -rf "lua"
    rm -rf "meshoptimizer"
    rm -rf "SDL"
    rm -rf "SPIRV-Cross"
    rm -rf "stb"
    rm -rf "tinyexr"
    rm -rf "tracy"
fi
# -------------------------------------------------------------------------------------------------

cd ..