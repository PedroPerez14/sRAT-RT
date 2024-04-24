include(FetchContent)
cmake_minimum_required(VERSION 3.17)

project(sRAT-RT)

set(CMAKE_CXX_STANDARD 17)

FetchContent_Declare(
        imgui-glfw-glad-glm
        GIT_REPOSITORY https://github.com/cmmw/imgui-glfw-glad-glm.git
)

FetchContent_Declare(
        assimp
        GIT_REPOSITORY https://github.com/assimp/assimp.git
        GIT_TAG master
)

FetchContent_MakeAvailable(
        imgui-glfw-glad-glm 
        assimp
)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INJECT_DEBUG_POSTFIX OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)

add_compile_definitions(
__SSE4_2__ 
__AVX__ 
__AVX512F__
)

add_executable(sRAT-RT 

        # Header files
        src/include/mINI/mINI.h
        src/include/rgb2spec/rgb2spec.h
        src/include/tinydir/tinydir.h
        src/include/sRAT-RT/camera.h
        src/include/sRAT-RT/shader.h
        src/include/sRAT-RT/stb_image.h
        src/include/sRAT-RT/mesh.h
        src/include/sRAT-RT/model.h
        src/include/sRAT-RT/colorspace.h
        src/include/sRAT-RT/settings.h
        src/include/sRAT-RT/input.h
        src/include/sRAT-RT/app.h
        src/include/sRAT-RT/scene.h
        src/include/sRAT-RT/framebuffer.h
        src/include/sRAT-RT/gl_check.h
        src/include/sRAT-RT/renderer.h
        src/include/sRAT-RT/renderer_test_uplifting.h
        src/include/sRAT-RT/response_curve.h
        src/include/sRAT-RT/render_passes.h
        src/include/sRAT-RT/material.h
        src/include/sRAT-RT/pbr_material.h
        src/include/sRAT-RT/renderer_pbr_test.h

        # Source files
        src/include/rgb2spec/rgb2spec.c
        src/include/sRAT-RT/stb_image.cpp
        src/mesh.cpp
        src/model.cpp
        src/settings.cpp
        src/app.cpp
        src/scene.cpp
        src/renderer_test_uplifting.cpp
        src/response_curve.cpp
        src/pbr_material.cpp
        src/renderer_pbr_test.cpp
        src/main.cpp
)

include_directories(
        # sRAT-RT include files
        src/include
)

target_include_directories(
        sRAT-RT PRIVATE libs/assimp-src/include
)

target_link_libraries(
        sRAT-RT
	assimp
        imgui
        glfw
        glad
        glm
)
