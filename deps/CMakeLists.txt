set(BUILD_SHARED_LIBS OFF)
set(BUILD_STATIC_LIBS ON)

if(${PURPL_GRAPHICS_API} STREQUAL "OPENGL")
	set(GLEW_SOURCES glew/src/glew.c)
	add_library(glew STATIC ${GLEW_SOURCES})
endif()

add_subdirectory(cglm)
add_subdirectory(json-c)
