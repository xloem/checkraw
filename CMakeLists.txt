cmake_minimum_required (VERSION 2.6)
project (checkraw)

add_custom_command(
	OUTPUT submodule-setup
	COMMAND git -C "${PROJECT_SOURCE_DIR}" submodule update --init
	COMMAND make -C "${PROJECT_SOURCE_DIR}/submodules/sunxi-tools" version.h
)
set_property(SOURCE submodule-setup PROPERTY SYMBOLIC 1)
include_directories ("${PROJECT_SOURCE_DIR}/submodules/sunxi-tools/include")

include_directories ("${PROJECT_SOURCE_DIR}/include")
add_subdirectory ("${CMAKE_CURRENT_SOURCE_DIR}/src")
add_library (checkraw submodule-setup ${SOURCE})
