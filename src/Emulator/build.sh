#!/bin/bash

# rm -rf build
# cmake -S . -Bbuild -G "Ninja Multi-Config"
# cmake --build ./build/ --config Debug
# cmake --build ./build/ --config Release
# cmake --build ./build/ --config RelWithDebInfo

cd test
rm -rf build
cmake -S . -Bbuild -DCMAKE_TOOLCHAIN_FILE=./cmake/riscv32i.toolchain.cmake -G "Ninja Multi-Config"
cmake --build ./build/ --config Debug
cmake --build ./build/ --config Release
cmake --build ./build/ --config RelWithDebInfo
cd ..
