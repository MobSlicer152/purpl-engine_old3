set(BUILD_SHARED_LIBS OFF)

if(${PURPL_GRAPHICS_API} STREQUAL "OPENGL")
	set(GLEW_SOURCES glew/src/glew.c)
	add_library(glew STATIC ${GLEW_SOURCES})
endif()

set(ENABLE_COVERAGE OFF)
set(ENABLE_CPIO OFF)
set(ENABLE_TAR OFF)
set(ENABLE_TEST OFF)

add_subdirectory(cglm)
add_subdirectory(json-c)
add_subdirectory(libarchive)
