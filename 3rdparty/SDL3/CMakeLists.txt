cmake_minimum_required(VERSION 3.28)

set(CMAKE_C_STANDARD 23)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

if (CMAKE_CXX_COMPILER)
endif ()

FetchContent_Declare(
		SDL3
		GIT_REPOSITORY "https://github.com/libsdl-org/SDL.git"
		GIT_TAG "release-3.4.x"
		GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(SDL3)