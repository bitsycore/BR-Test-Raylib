cmake_minimum_required(VERSION 3.28)

set(CMAKE_C_STANDARD 23)

if (CMAKE_CXX_COMPILER)
endif ()

set(RENDERER "RAYLIB")

if (RENDERER STREQUAL "RAYLIB")
	set(RENDERER_SOURCES
			renderer/raylib/clayray.h
			renderer/raylib/clayray.c
	)
	set(RENDERER_LIBS
			raylib
	)
	add_subdirectory(../Raylib ${CMAKE_BINARY_DIR}/Raylib)
elseif (RENDERER STREQUAL "SDL3")
	set(RENDERER_SOURCES
			renderer/sdl3/claysdl3.h
			renderer/sdl3/claysdl3.c
	)
	set(RENDERER_LIBS
			SDL3::SDL3-static
	)
	add_subdirectory(../SDL3 ${CMAKE_BINARY_DIR}/SDL3)
	add_subdirectory(../SDL3_ttf ${CMAKE_BINARY_DIR}/SDL3_ttf)
	add_subdirectory(../SDL3_image ${CMAKE_BINARY_DIR}/SDL3_image)
else ()
	message(FATAL_ERROR "Unknown renderer: ${RENDERER}. Please choose RAYLIB or SDL3.")
endif ()

add_executable(Test
		main.c
		3rdparty/clay.h
		${RENDERER_SOURCES}
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

target_link_libraries(Test
		PRIVATE
		BFramework
		${RENDERER_LIBS}
)

set(RESOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/resources")

add_custom_command(TARGET Test POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${RESOURCE_DIR}"
		"$<TARGET_FILE_DIR:Test>/resources"
)

add_custom_command(TARGET Test POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"$<TARGET_FILE:SDL3::SDL3-shared>"
		"$<TARGET_FILE_DIR:Test>"
)