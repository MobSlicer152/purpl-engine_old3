# Most of these libraries can't build as dynamic libraries on Linux on some
#  filesystems (mainly exFAT, which is the FS my development drive uses),
#  disable shared libraries
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(CGLM_SHARED OFF CACHE BOOL "" FORCE)
set(CGLM_STATIC ON CACHE BOOL "" FORCE)

if (${PURPL_GRAPHICS_API} STREQUAL "OPENGL")
	set(GLEW_ARCHIVE ${CMAKE_CURRENT_BINARY_DIR}/glew.tar.gz)
	file(DOWNLOAD "https://github.com/nigels-com/glew/releases/download/glew-${PURPL_GLEW_VER}/glew-${PURPL_GLEW_VER}.tgz" ${GLEW_ARCHIVE})
	file(REMOVE ${CMAKE_CURRENT_LIST_DIR}/glew-${PURPL_GLEW_VER})
	file(ARCHIVE_EXTRACT INPUT ${GLEW_ARCHIVE} DESTINATION ${CMAKE_CURRENT_LIST_DIR})
	file(REMOVE ${GLEW_ARCHIVE})
	file(REMOVE ${CMAKE_CURRENT_LIST_DIR}/glew-${PURPL_GLEW_VER}/build/cmake/CMakeLists.txt)
	file(COPY ${CMAKE_CURRENT_LIST_DIR}/glew_cmlists.cmake DESTINATION ${CMAKE_CURRENT_LIST_DIR}/glew-${PURPL_GLEW_VER}/build/cmake)
	file(RENAME ${CMAKE_CURRENT_LIST_DIR}/glew-${PURPL_GLEW_VER}/build/cmake/glew_cmlists.cmake ${CMAKE_CURRENT_LIST_DIR}/glew-${PURPL_GLEW_VER}/build/cmake/CMakeLists.txt)
	add_subdirectory(glew-${PURPL_GLEW_VER}/build/cmake ${CMAKE_CURRENT_BINARY_DIR}/glew)
endif()

set(ENABLE_CAT OFF CACHE BOOL "Disable bsdcat" FORCE)
set(ENABLE_COVERAGE OFF CACHE BOOL "Disable test coverage" FORCE)
set(ENABLE_CPIO OFF CACHE BOOL "Disable bsdcpio" FORCE)
set(ENABLE_TAR OFF CACHE BOOL "Disable bsdtar" FORCE)
set(ENABLE_TEST OFF CACHE BOOL "Disable tests" FORCE)

add_subdirectory(cglm)
add_subdirectory(json-c)
add_subdirectory(libarchive)
set_target_properties(archive PROPERTIES EXCLUDE_FROM_ALL 1 EXCLUDE_FROM_DEFAULT_BUILD 1)

set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)

# SDL doesn't pull any symlinking shit when it builds as shared
#  and advises against statically linking, so there we have it
add_subdirectory(sdl2)
