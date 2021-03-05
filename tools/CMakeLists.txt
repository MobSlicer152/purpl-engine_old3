cmake_minimum_required(VERSION 3.10)
x
project(frankentar VERSION 0.0.1)

set(MKEMBED_SOURCES
	mkembed.c
)

add_executable(mkembed ${MKEMBED_SOURCES})
target_link_libraries(mkembed purpl)
