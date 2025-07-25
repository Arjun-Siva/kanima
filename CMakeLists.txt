cmake_minimum_required(VERSION 3.16)
project(kanima LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ==== Library ======
add_library(kanima)

target_sources(kanima
    PRIVATE
        src/linalg/vec3.cpp
        src/linalg/mat3.cpp
        src/core/camera.cpp
        src/core/color.cpp
        src/core/light.cpp
        src/core/material.cpp
        src/core/mesh.cpp
        src/core/scene.cpp
        src/core/triangle.cpp
        src/shader/constantShader.cpp
        src/shader/diffuseShader.cpp
        src/shader/recursiveShader.cpp
        src/shader/reflectiveShader.cpp
        src/shader/refractiveShader.cpp
        src/2dShapes/shapes.cpp
        src/accTree/bvhnode.cpp
        src/stb_image/stb_image.cpp
)

# Only expose public headers
target_include_directories(kanima
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# === Tests ===
enable_testing()
add_executable(kanima_test_import
    sandbox/importTest.cpp
)
target_link_libraries(kanima_test_import PRIVATE kanima)

add_executable(kanima_test_refraction
    sandbox/refractionTest.cpp
)
target_link_libraries(kanima_test_refraction PRIVATE kanima)
add_test(NAME Import COMMAND kanima_test_import)
add_test(NAME Refraction COMMAND kanima_test_refraction)
