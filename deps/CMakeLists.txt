set(BUILD_SHARED_LIBS OFF CACHE BOOL "Disable shared libraries" FORCE)

if(${PURPL_GRAPHICS_API} STREQUAL "OPENGL")
	set(GLEW_SOURCES glew/src/glew.c)
	add_library(glew STATIC ${GLEW_SOURCES})
endif()

set(ENABLE_COVERAGE OFF CACHE BOOL "Disable test coverage" FORCE)
set(ENABLE_CPIO OFF CACHE BOOL "Disable bsdcpio" FORCE)
set(ENABLE_TAR OFF CACHE BOOL "Disable bsdtar" FORCE)
set(ENABLE_TEST OFF CACHE BOOL "Disable tests" FORCE)

add_subdirectory(cglm)
add_subdirectory(json-c)
add_subdirectory(libarchive)
