cmake_minimum_required(VERSION 3.28)

set(CMAKE_C_STANDARD 23)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

if (CMAKE_CXX_COMPILER)
endif ()

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

set(ENABLE_SDL_BACKEND OFF)

if (ENABLE_SDL_BACKEND)
    set(PLATFORM "SDL" CACHE STRING "" FORCE)
    FetchContent_Declare(
            SDL2
            GIT_REPOSITORY "https://github.com/libsdl-org/SDL.git"
            GIT_TAG "SDL2"
            GIT_PROGRESS TRUE
    )
    FetchContent_MakeAvailable(SDL2)
else ()
    set(PLATFORM "Desktop" CACHE STRING "" FORCE)
endif ()

FetchContent_Declare(
        raylib
        GIT_REPOSITORY "https://github.com/raysan5/raylib.git"
        GIT_TAG "master"
        GIT_PROGRESS TRUE
)


FetchContent_MakeAvailable(raylib)

target_include_directories(raylib PRIVATE ${BCore_SOURCE_DIR})

if (MSVC)
    target_compile_options(raylib PRIVATE "/FI${BCore_SOURCE_DIR}/Memory/BC_Memory.h")
else ()
    target_compile_options(raylib PRIVATE "-include${BCore_SOURCE_DIR}/Memory/BC_Memory.h")
endif ()

target_compile_definitions(raylib PRIVATE
        RL_MALLOC=BC_Malloc
        RL_CALLOC=BC_Calloc
        RL_REALLOC=BC_Realloc
        RL_FREE=BC_Free
)