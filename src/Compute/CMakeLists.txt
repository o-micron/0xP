cmake_minimum_required(VERSION 4.0)
project(XPCompute VERSION 0.1.0 LANGUAGES C)

set(ENV{PATH} "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/slang/install/bin/:$ENV{PATH}")
set(SLANG_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/slang/install/")

if(XP_USE_COMPUTE_CUDA)
    # Find required packages
    find_package(CUDAToolkit REQUIRED)
endif()

# Find Slang (you may need to adjust paths based on your Slang installation)
find_path(SLANG_INCLUDE_DIR
    NAMES slang.h
    PATHS
        ${SLANG_ROOT}/include
        $ENV{SLANG_ROOT}/include
        /usr/local/include/slang
        /usr/include/slang
    DOC "Slang include directory"
)

find_library(SLANG_LIBRARY
    NAMES slang libslang
    PATHS
        ${SLANG_ROOT}/lib
        $ENV{SLANG_ROOT}/lib
        /usr/local/lib
        /usr/lib
    DOC "Slang library"
)

# Check if Slang was found
if(NOT SLANG_INCLUDE_DIR OR NOT SLANG_LIBRARY)
    message(FATAL_ERROR "Slang not found. Please set SLANG_ROOT to your Slang installation directory.")
endif()

message(STATUS "Found Slang include: ${SLANG_INCLUDE_DIR}")
message(STATUS "Found Slang library: ${SLANG_LIBRARY}")

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# COMPUTE
# ---------------------------------------------------------------------------------------------------------------------------------------------------
set(XP_COMPUTE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/XPCompute.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/XPComputeElfLoader.c
)

set(XP_COMPUTE_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Compute/XPCompute.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Compute/XPComputeElfLoader.h
)

set(XP_SLANG_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/compute.slang
)

add_library(XPCompute STATIC ${XP_COMPUTE_SOURCES} ${XP_COMPUTE_HEADERS})
target_include_directories(XPCompute PUBLIC ${CMAKE_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/include)
if (WIN32)
    set_property(TARGET XPCompute PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif()
if(XP_USE_COMPUTE_CUDA)
    set_target_properties(XPCompute PROPERTIES CUDA_ARCHITECTURES "75;80;86;89")
    target_include_directories(XPCompute PRIVATE ${SLANG_INCLUDE_DIR} ${CUDAToolkit_INCLUDE_DIRS})
    target_link_libraries(XPCompute PUBLIC ${SLANG_LIBRARY} CUDA::cudart CUDA::cuda_driver)
elseif(XP_USE_COMPUTE_METAL)
    target_include_directories(XPCompute PRIVATE ${SLANG_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/thirdparty/metal-cpp/)
    target_link_libraries(XPCompute PRIVATE ${SLANG_LIBRARY} "-framework Metal")
endif()
# ---------------------------------------------------------------------------------------------------------------------------------------------------
