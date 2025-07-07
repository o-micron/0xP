#!/bin/bash

mkdir $3

rm -rf $3/$2.surface.spv
rm -rf $3/$2.vert.spv
rm -rf $3/$2.frag.spv

glslc -c -fshader-stage=vertex $1/$2.vert -o $3/$2.vert.spv
glslc -c -fshader-stage=fragment $1/$2.frag -o $3/$2.frag.spv
spirv-link $3/$2.vert.spv $3/$2.frag.spv -o $3/$2.surface.spv

# rm -rf $2.vert.spv
# rm -rf $2.frag.spv