cmake_minimum_required(VERSION 3.10)

project(purpl-engine VERSION 3.0)

set(CMAKE_BUILD_TYPE Debug)
option(PURPL_BUILD_DEMO "Whether to build the demo" ON)
option(PURPL_BUILD_TOOLS "Whether to build the engine tools" ON)
set(PURPL_GRAPHICS_API "OPENGL" CACHE STRING "The graphics API to use")

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_C_EXTENSIONS TRUE)

set(CMAKE_C_FLAGS "-O2 -DPURPL_USE_${PURPL_GRAPHICS_API}")
if (NOT MSVC)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wno-discarded-qualifiers -Wno-deprecated-declarations") # It's 2 warnings disabled out of tons of other warnings, chill
else()
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -W1 -TC -wd4996") # If you add POSIX functions, don't rename them just to fuck up our code
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

include_directories(include deps/cglm/include deps/glew/include deps/json-c deps/libzip/lib deps/stb)

add_subdirectory(deps)
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
endif()

add_library(purpl STATIC ${PURPL_HEADERS} ${PURPL_SOURCES})
target_link_libraries(purpl cglm glew json-c zip)

if (${PURPL_GRAPHICS_API} STREQUAL "OPENGL")
	target_link_libraries(purpl OpenGL::GL)

	if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
		target_link_libraries(purpl OpenGL::GLX)
	endif()
endif()
