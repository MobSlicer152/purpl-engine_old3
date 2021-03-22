set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(CGLM_SHARED OFF CACHE BOOL "" FORCE)
set(CGLM_STATIC ON CACHE BOOL "" FORCE)

if(${PURPL_GRAPHICS_API} STREQUAL "OPENGL")
	set(GLEW_SOURCE ${CMAKE_SOURCE_DIR}/deps/glew/src/glew.c)
	if (NOT EXISTS ${GLEW_SOURCE})
		add_custom_command(OUTPUT ${GLEW_SOURCE}
				   COMMAND ${PURPL_MAKE_PROG} -C ${CMAKE_SOURCE_DIR}/deps/glew/auto
				   VERBATIM)
	endif()
	add_library(glew STATIC ${GLEW_SOURCE})
endif()

set(ENABLE_CAT OFF CACHE BOOL "Disable bsdcat" FORCE)
set(ENABLE_COVERAGE OFF CACHE BOOL "Disable test coverage" FORCE)
set(ENABLE_CPIO OFF CACHE BOOL "Disable bsdcpio" FORCE)
set(ENABLE_TAR OFF CACHE BOOL "Disable bsdtar" FORCE)
set(ENABLE_TEST OFF CACHE BOOL "Disable tests" FORCE)

add_subdirectory(cglm)
add_subdirectory(json-c)
add_subdirectory(libarchive)
set_target_properties(archive PROPERTIES EXCLUDE_FROM_ALL 1 EXCLUDE_FROM_DEFAULT_BUILD 1) # Disable any and all shared libraries
add_subdirectory(sdl2)
