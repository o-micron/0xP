del .\shaders\compute.cu
del .\shaders\compute.ptx

: compile slang
..\..\artifacts\windows\bin\slangc.exe .\shaders\compute.slang -I.\shaders\ -stage compute -capability cuda_sm_5_0 -profile sm_5_0 -entry computeMain -target cuda -O3 -g0 -o .\shaders\compute.cu

: compile cuda
nvcc -ptx .\shaders\compute.cu -o .\shaders\compute.ptx -diag-suppress 550
