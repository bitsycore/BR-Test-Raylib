cmake_minimum_required(VERSION 3.28)

set(CMAKE_C_STANDARD 23)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

if (CMAKE_CXX_COMPILER)
endif ()

FetchContent_Declare(
		SDL3_image
		GIT_REPOSITORY "https://github.com/libsdl-org/SDL_image.git"
		GIT_TAG "main"
		GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(SDL3_image)