if ( NOT DEFINED CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE Release CACHE STRING "Build type" )
endif ()

project (glew C)

cmake_minimum_required (VERSION 2.8.12)

include(GNUInstallDirs)

if(POLICY CMP0003)
  cmake_policy (SET CMP0003 NEW)
endif()

if(POLICY CMP0042)
  cmake_policy (SET CMP0042 NEW)
endif()

set(CMAKE_DEBUG_POSTFIX d)

option (GLEW_REGAL "Regal mode" OFF)
option (GLEW_OSMESA "OSMesa mode" OFF)
if (APPLE)
    option (BUILD_FRAMEWORK "Build Framework bundle for OSX" OFF)
endif ()

set (GLEW_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)

# get version from config/version
file (STRINGS ${GLEW_DIR}/config/version  _VERSION_MAJOR_STRING REGEX "GLEW_MAJOR[ ]*=[ ]*[0-9]+.*")
string (REGEX REPLACE "GLEW_MAJOR[ ]*=[ ]*([0-9]+)" "\\1" CPACK_PACKAGE_VERSION_MAJOR ${_VERSION_MAJOR_STRING})
file (STRINGS ${GLEW_DIR}/config/version  _VERSION_MINOR_STRING REGEX "GLEW_MINOR[ ]*=[ ]*[0-9]+.*")
string (REGEX REPLACE "GLEW_MINOR[ ]*=[ ]*([0-9]+)" "\\1" CPACK_PACKAGE_VERSION_MINOR ${_VERSION_MINOR_STRING})
file (STRINGS ${GLEW_DIR}/config/version  _VERSION_PATCH_STRING REGEX "GLEW_MICRO[ ]*=[ ]*[0-9]+.*")
string (REGEX REPLACE "GLEW_MICRO[ ]*=[ ]*([0-9]+)" "\\1" CPACK_PACKAGE_VERSION_PATCH ${_VERSION_PATCH_STRING})
set (GLEW_VERSION ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH})

set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package (OpenGL REQUIRED)

# X11 required except for Windows and Apple OSX platforms
if (NOT WIN32 AND NOT APPLE)
  find_package (X11)
endif()

if (WIN32)
  set (GLEW_LIB_NAME glew32)
else ()
  set (GLEW_LIB_NAME GLEW)
  set (DLL_PREFIX lib)
endif ()

set (GLEW_LIBRARIES ${OPENGL_LIBRARIES} ${X11_LIBRARIES})

add_definitions (-DGLEW_NO_GLU)

#### Regal mode ####

if (GLEW_REGAL)
  if (WIN32)
    set (REGAL_LIB_NAME regal32)
  else ()
    set (REGAL_LIB_NAME Regal)
  endif ()
  add_definitions (-DGLEW_REGAL)
  set (GLEW_LIBRARIES ${REGAL_LIB_NAME})
endif ()

#### OSMesa mode ####

if (GLEW_OSMESA)
  if (WIN32)
    set (OSMESA_LIB_NAME osmesa)
  else ()
    set (OSMESA_LIB_NAME OSMesa)
  endif ()
  add_definitions (-DGLEW_OSMESA)
  set (GLEW_LIBRARIES ${OSMESA_LIB_NAME} ${OPENGL_LIBRARIES})
  set (X11_LIBRARIES)
endif ()

#### EGL ####

if (GLEW_EGL AND UNIX)
  add_definitions (-DGLEW_EGL)
  if (OpenGL::EGL)
    message (FATAL_ERROR "EGL library set but not found.")
  endif()
  set (GLEW_LIBRARIES ${OPENGL_LIBRARIES} ${OPENGL_egl_LIBRARY})
endif ()

#### GLEW ####

include_directories (${GLEW_DIR}/include ${X11_INCLUDE_DIR})

set (GLEW_PUBLIC_HEADERS_FILES ${GLEW_DIR}/include/GL/wglew.h ${GLEW_DIR}/include/GL/glew.h ${GLEW_DIR}/include/GL/glxew.h)
set (GLEW_SRC_FILES ${GLEW_DIR}/src/glew.c)

if (WIN32)
  list (APPEND GLEW_SRC_FILES ${GLEW_DIR}/build/glew.rc)
endif ()

add_library (glew STATIC ${GLEW_PUBLIC_HEADERS_FILES} ${GLEW_SRC_FILES})
set_target_properties (glew PROPERTIES COMPILE_DEFINITIONS "GLEW_STATIC" OUTPUT_NAME "${GLEW_LIB_NAME}" PREFIX lib)

if (MSVC)
  # add options from visual studio project
  target_compile_definitions (glew PRIVATE "GLEW_STATIC;VC_EXTRALEAN")
  # kill security checks which are dependent on stdlib
  target_compile_options (glew PRIVATE -GS-)
  string(REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})
elseif (WIN32 AND ((CMAKE_C_COMPILER_ID MATCHES "GNU") OR (CMAKE_C_COMPILER_ID MATCHES "Clang")))
  # remove stdlib dependency on windows with GCC and Clang (for similar reasons
  # as to MSVC - to allow it to be used with any Windows compiler)
  target_compile_options (glew PRIVATE -fno-builtin -fno-stack-protector)
endif ()

target_link_libraries (glew ${GLEW_LIBRARIES})
