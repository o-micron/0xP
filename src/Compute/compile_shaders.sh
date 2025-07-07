#!/bin/bash

rm -rf ./shaders/compute.metal
rm -rf ./shaders/compute.metallib

# compile slang
./thirdparty/slang/install/bin/slangc -I./shaders/ -stage compute -entry computeMain -target metal ./shaders/compute.slang -o ./shaders/compute.metal

# compile metal
xcrun -sdk macosx metal ./shaders/compute.metal -o ./shaders/compute.metallib
