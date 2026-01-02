cmake_minimum_required(VERSION 3.28)

set(CMAKE_C_STANDARD 23)

if (CMAKE_CXX_COMPILER)
endif ()

add_executable(Test
		main.c
		clayray.c
		clay.h
		clayray.h
)

target_link_libraries(Test
		PRIVATE
		BFramework
		raylib
)

set(RESOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/resources")

add_custom_command(TARGET Test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${RESOURCE_DIR}"
        "$<TARGET_FILE_DIR:Test>/resources"
)