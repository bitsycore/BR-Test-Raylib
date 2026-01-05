cmake_minimum_required(VERSION 3.28)

set(CMAKE_C_STANDARD 23)

if (CMAKE_CXX_COMPILER)
endif ()

set(RENDERER "renderer")

add_executable(TestSdl3
		main.c
		clay/clay.h
		clay/claysdl3.h
		clay/claysdl3.c
)

target_link_libraries(TestSdl3
		PRIVATE
		SDL3::SDL3
		SDL3_ttf::SDL3_ttf
		SDL3_image::SDL3_image
)

add_custom_command(TARGET TestSdl3 POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SDL3_BINARY_DIR}/SDL3.dll" $<TARGET_FILE_DIR:TestSdl3>
		COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SDL3_ttf_BINARY_DIR}/SDL3_ttf.dll" $<TARGET_FILE_DIR:TestSdl3>
		COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SDL3_image_BINARY_DIR}/SDL3_image.dll" $<TARGET_FILE_DIR:TestSdl3>
)