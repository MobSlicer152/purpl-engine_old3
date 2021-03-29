set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(CGLM_SHARED OFF CACHE BOOL "" FORCE)
set(CGLM_STATIC ON CACHE BOOL "" FORCE)

if(${PURPL_GRAPHICS_API} STREQUAL "OPENGL")
	set(PURPL_GLEW_SOURCE ${CMAKE_CURRENT_BINARY_DIR}/glew-${PURPL_GLEW_VER}/src/glew.c)
	if (NOT EXISTS ${PURPL_GLEW_SOURCE})
		add_custom_command(OUTPUT ${PURPL_GLEW_SOURCE}
				   COMMAND curl -fsGL https://github.com/nigels-com/glew/releases/download/glew-${PURPL_GLEW_VER}/glew-${PURPL_GLEW_VER}.zip -o ${CMAKE_CURRENT_BINARY_DIR}/glew.zip
				   COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_COMMAND} -E tar xvf ${CMAKE_CURRENT_BINARY_DIR}/glew.zip --format=zip
				   VERBATIM)
	endif()
endif()

set(ENABLE_CAT OFF CACHE BOOL "Disable bsdcat" FORCE)
set(ENABLE_COVERAGE OFF CACHE BOOL "Disable test coverage" FORCE)
set(ENABLE_CPIO OFF CACHE BOOL "Disable bsdcpio" FORCE)
set(ENABLE_TAR OFF CACHE BOOL "Disable bsdtar" FORCE)
set(ENABLE_TEST OFF CACHE BOOL "Disable tests" FORCE)

add_subdirectory(cglm)
add_subdirectory(json-c)
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/glew-${PURPL_GLEW_VER}/build/cmake ${CMAKE_CURRENT_BINARY_DIR}/glew) # Goes after json-c to fix uninstall target duplicate
set_target_properties(glew PROPERTIES EXCLUDE_FROM_ALL 1 EXCLUDE_FROM_DEFAULT_BUILD 1) # Disable any and all shared libraries
add_subdirectory(libarchive)
set_target_properties(archive PROPERTIES EXCLUDE_FROM_ALL 1 EXCLUDE_FROM_DEFAULT_BUILD 1)
add_subdirectory(sdl2)
