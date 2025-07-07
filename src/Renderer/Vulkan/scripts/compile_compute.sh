#!/bin/bash

mkdir $3

rm -rf $3/$2.compute.spv

glslc -c -fshader-stage=compute $1/$2.comp -o $3/$2.compute.spv