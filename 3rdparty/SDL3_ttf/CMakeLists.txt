cmake_minimum_required(VERSION 3.28)

set(CMAKE_C_STANDARD 23)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

if (CMAKE_CXX_COMPILER)
endif ()

set(SDLTTF_VENDORED TRUE CACHE BOOL "" FORCE)
set(SDLTTF_HARFBUZZ_VENDORED TRUE CACHE BOOL "" FORCE)
set(SDLTTF_FREETYPE_VENDORED TRUE CACHE BOOL "" FORCE)

FetchContent_Declare(
		SDL3_ttf
		GIT_REPOSITORY "https://github.com/libsdl-org/SDL_ttf.git"
		GIT_TAG "main"
		GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(SDL3_ttf)