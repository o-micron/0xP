del .\shaders\compute.cu
del .\shaders\compute.ptx

: compile slang
.\thirdparty\slang\install\bin\slangc.exe .\shaders\compute.slang -I${CMAKE_CURRENT_SOURCE_DIR}/shaders/ -stage compute -capability cuda_sm_5_0 -profile sm_5_0 -entry computeMain -target cuda -O3 -g0 -o .\shaders\compute.cu

: compile cuda
nvcc -ptx .\shaders\compute.cu -o .\shaders\compute.ptx -diag-suppress 550
