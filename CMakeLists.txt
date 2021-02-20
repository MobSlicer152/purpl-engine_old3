cmake_minimum_required(VERSION 3.10)

project(purpl-engine VERSION 3.0)

set(CMAKE_BUILD_TYPE Debug)
option(PURPL_BUILD_DEMO "Whether to build the demo" ON)
set(PURPL_GRAPHICS_API "OPENGL" CACHE STRING "The graphics API to use")

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_C_EXTENSIONS TRUE)

set(CMAKE_C_FLAGS "-O2 -Wall -DPURPL_USE_${PURPL_GRAPHICS_API}")
if (NOT MSVC)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wextra -Wno-discarded-qualifiers")
else()
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -wd4996")
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

include_directories(include deps/cglm/include deps/glew/include deps/json-c deps/stb)

add_subdirectory(deps)
add_subdirectory(include)
add_subdirectory(src)

add_library(purpl SHARED ${PURPL_HEADERS} ${PURPL_SOURCES})
target_link_libraries(purpl cglm glew json-c)

if (${PURPL_GRAPHICS_API} STREQUAL "OPENGL")
	target_link_libraries(purpl OpenGL::GL)

	if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
		target_link_libraries(purpl OpenGL::GLX)
	endif()
endif()

if (${PURPL_BUILD_DEMO})
    add_subdirectory(demo)
    add_executable(purpl-demo ${PURPL_DEMO_HEADERS} ${PURPL_DEMO_SOURCES})
    target_link_libraries(purpl-demo purpl)
endif()
