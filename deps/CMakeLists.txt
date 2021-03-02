set(BUILD_SHARED_LIBS OFF CACHE BOOL "Disable shared libraries" FORCE)

if(${PURPL_GRAPHICS_API} STREQUAL "OPENGL")
	set(GLEW_SOURCES glew/src/glew.c)
	add_library(glew STATIC ${GLEW_SOURCES})
endif()

set(LIBZIP_DO_INSTALL OFF CACHE BOOL "Disable libzip install" FORCE)

add_subdirectory(cglm)
add_subdirectory(json-c)
add_subdirectory(libzip)
