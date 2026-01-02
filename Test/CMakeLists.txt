cmake_minimum_required(VERSION 3.28)

set(CMAKE_C_STANDARD 23)

if (CMAKE_CXX_COMPILER)
endif ()

add_executable(Test
		main.c
		3rdparty/clayray.c
		3rdparty/clay.h
		3rdparty/clayray.h
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
		raylib
)

set(RESOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/resources")

add_custom_command(TARGET Test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${RESOURCE_DIR}"
        "$<TARGET_FILE_DIR:Test>/resources"
)