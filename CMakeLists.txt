cmake_minimum_required(VERSION 3.28)
project(BR-Test-Raylib C)

set(CMAKE_C_STANDARD 23)

if (CMAKE_CXX_COMPILER)
endif ()

add_subdirectory(../BRuntime/BCore ${CMAKE_BINARY_DIR}/BCore)
add_subdirectory(../BRuntime/BFramework ${CMAKE_BINARY_DIR}/BFramework)
add_subdirectory(3rdparty/SDL3 ${CMAKE_BINARY_DIR}/SDL3)
add_subdirectory(3rdparty/SDL3_ttf ${CMAKE_BINARY_DIR}/SDL3_ttf)
add_subdirectory(3rdparty/SDL3_image ${CMAKE_BINARY_DIR}/SDL3_image)

add_subdirectory(TestRaylib)
add_subdirectory(TestSdl3)