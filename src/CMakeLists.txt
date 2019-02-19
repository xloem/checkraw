add_subdirectory ("${CMAKE_CURRENT_SOURCE_DIR}/drivers")
set(SOURCE
	${SOURCE}
	"${CMAKE_CURRENT_SOURCE_DIR}/bitbang.c"
	PARENT_SCOPE
)
