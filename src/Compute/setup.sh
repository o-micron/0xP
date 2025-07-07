#!/bin/bash

mkdir thirdparty
cd thirdparty

git clone --depth 1 --single-branch -b v2025.10.5 https://github.com/shader-slang/slang.git
cd slang
git submodule update --init --recursive
rm -rf build/
cmake --preset slang-llvm
cmake --build --preset releaseWithDebugInfo # or --preset debug, or --preset release
cd ..

cd ..