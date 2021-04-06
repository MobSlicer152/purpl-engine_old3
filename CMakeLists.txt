cmake_minimum_required(VERSION 3.10)

project(purpl-engine VERSION 3.0)

set(CMAKE_BUILD_TYPE Debug)
option(PURPL_BUILD_DEMO "Whether to build the demo" ON)
option(PURPL_BUILD_TOOLS "Whether to build the engine tools" ON)
set(PURPL_GRAPHICS_API "OPENGL" CACHE STRING "The graphics API to use")

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_C_EXTENSIONS TRUE)

# Validate the graphics API selection
if (NOT ${PURPL_GRAPHICS_API} STREQUAL "OPENGL")
	message(FATAL_ERROR "PURPL_GRAPHICS_API must be one of the following:\nOPENGL")
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	find_package(X11)

	if (${PURPL_GRAPHICS_API} STREQUAL "OPENGL")
		set(OpenGL_GL_PREFERENCE GLVND)
	endif()
endif()

if (${PURPL_GRAPHICS_API} STREQUAL "OPENGL")
	find_package(OpenGL REQUIRED)

	include_directories(${OPENGL_INCLUDE_DIR})
endif()

# Necessary because MSVC is incabable of sane warnings and Werror therefore doesn't work
if (MSVC)
	set(CMAKE_C_FLAGS "-w -TC -wd4996") # If you add POSIX functions, don't rename them just to fuck up our code
	if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -MTd")
	else()
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -MT")
	endif()

	if (${PURPL_GRAPHICS_API} STREQUAL "OPENGL")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DGLEW_STATIC")
	endif()
endif()

set(PURPL_GLEW_VER 2.2.0)

add_subdirectory(deps)
include_directories(include deps/cglm/include deps/glew-${PURPL_GLEW_VER}/include deps/json-c deps/libarchive/libarchive deps/stb)

# Set our compiler flags
if (NOT MSVC)
	set(CMAKE_C_FLAGS "-Wall -Wextra -Wno-deprecated-declarations -Wno-switch -Wno-pointer-sign") # It's 4 warnings disabled out of tons of other warnings, chill
	if (${CMAKE_C_COMPILER_ID} MATCHES "Clang")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-ignored-qualifiers -Wno-incompatible-pointer-types-discards-qualifiers")
	else()
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-discarded-qualifiers")
	endif()
endif()
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DPURPL_USE_${PURPL_GRAPHICS_API}_GFX=1 -O2")

message(STATUS "Purpl Engine's CFLAGS: ${CMAKE_C_FLAGS}")

add_subdirectory(include)
add_subdirectory(src)

if (NOT ${PURPL_BUILD_TOOLS} AND ${PURPL_BUILD_DEMO})
	set(PURPL_BUILD_TOOLS ON CACHE BOOL "" FORCE)
endif()

if (${PURPL_BUILD_TOOLS})
	add_subdirectory(tools)
endif()

add_library(purpl_util STATIC ${PURPL_UTIL_HEADERS} ${PURPL_UTIL_SOURCES})
add_library(purpl STATIC ${PURPL_HEADERS} ${PURPL_SOURCES})
target_link_libraries(purpl archive_static cglm json-c purpl_util SDL2::SDL2)

if (${PURPL_BUILD_DEMO})
    add_subdirectory(demo)
endif()

if (${PURPL_GRAPHICS_API} STREQUAL "OPENGL")
	target_link_libraries(purpl glew OpenGL::GL)

	if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
		target_link_libraries(purpl OpenGL::GLX)
	endif()
endif()
