cmake_minimum_required(VERSION 3.28)
project(BR-Test-Raylib C)

set(CMAKE_C_STANDARD 23)

if (CMAKE_CXX_COMPILER)
endif ()

add_subdirectory(../BRuntime/BCore ${CMAKE_BINARY_DIR}/BCore)
add_subdirectory(../BRuntime/BFramework ${CMAKE_BINARY_DIR}/BFramework)
add_subdirectory(Raylib)

add_subdirectory(Test)