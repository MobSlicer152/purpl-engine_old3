cmake_minimum_required(VERSION 3.10)

project(purpl-engine VERSION 3.0)

set(CMAKE_BUILD_TYPE Debug)
option(PURPL_BUILD_DEMO "Whether to build the demo" ON)
option(PURPL_BUILD_TOOLS "Whether to build the engine tools" ON)
set(PURPL_GRAPHICS_API "OPENGL" CACHE STRING "The graphics API to use")

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_C_EXTENSIONS TRUE)

set(CMAKE_C_FLAGS "-O2 -DPURPL_USE_${PURPL_GRAPHICS_API}")

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

include_directories(include deps/cglm/include deps/glew/include deps/json-c deps/libarchive/libarchive deps/stb)

# Necessary because MSVC is incabable of sane warnings and Werror therefore doesn't work
if (MSVC)
	set(CMAKE_C_FLAGS "-w -TC -wd4996") # If you add POSIX functions, don't rename them just to fuck up our code
endif()

# GLEW needs this
if (MINGW)
	set(PURPL_MAKE_PROG mingw32-make)
else()
	set(PURPL_MAKE_PROG make)
endif()

add_subdirectory(deps)

# Set our compiler flags
if (NOT MSVC)
	set(CMAKE_C_FLAGS "-Wall -Wextra -Wno-deprecated-declarations") # It's 2 warnings disabled out of tons of other warnings, chill
	if (${CMAKE_C_COMPILER_ID} MATCHES "Clang")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-ignored-qualifiers -Wno-incompatible-pointer-types-discards-qualifiers")
	else()
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-discarded-qualifiers")
	endif()
endif()

add_subdirectory(include)
add_subdirectory(src)

if (NOT ${PURPL_BUILD_TOOLS} AND ${PURPL_BUILD_DEMO})
	set(PURPL_BUILD_TOOLS ON CACHE BOOL "" FORCE)
endif()

if (${PURPL_BUILD_TOOLS})
	add_subdirectory(tools)
endif()

if (${PURPL_BUILD_DEMO})
    add_subdirectory(demo)
	if (MSVC_IDE)
		set(VS_STARTUP_PROJECT purpl-demo)
	endif()
endif()

add_library(purpl STATIC ${PURPL_HEADERS} ${PURPL_SOURCES} "src/stb.c")
target_link_libraries(purpl archive_static cglm glew json-c SDL2::SDL2-static)

if (${PURPL_GRAPHICS_API} STREQUAL "OPENGL")
	target_link_libraries(purpl OpenGL::GL)

	if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
		target_link_libraries(purpl OpenGL::GLX)
	endif()
endif()
