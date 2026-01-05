cmake_minimum_required(VERSION 3.28)

set(CMAKE_C_STANDARD 23)

if (CMAKE_CXX_COMPILER)
endif ()

add_subdirectory(../3rdparty/Raylib ${CMAKE_BINARY_DIR}/Raylib)

add_executable(TestRaylib
		main.c
		3rdparty/clay.h
		renderer/raylib/clayray.h
		renderer/raylib/clayray.c
		constants/Colors.h
		components/Scrollbar.c
		components/Scrollbar.h
		components/Blob4Floating2.c
		components/Blob4Floating2.h
		components/Sidebar.c
		components/Sidebar.h
		ResourcesLoad.c
		ResourcesLoad.h
		components/RightPanel.c
		components/RightPanel.h
)

target_link_libraries(TestRaylib
		PRIVATE
		BFramework
		raylib
)

set(RESOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/resources")

add_custom_command(TARGET TestRaylib POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${RESOURCE_DIR}"
		"$<TARGET_FILE_DIR:TestRaylib>/resources"
)

add_custom_command(TARGET TestRaylib POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"$<TARGET_FILE:SDL3::SDL3-shared>"
		"$<TARGET_FILE_DIR:TestRaylib>"
)