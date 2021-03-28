cmake_minimum_required(VERSION 3.10)

set(MKEMBED_SOURCES
	mkembed.c
)

add_executable(mkembed ${MKEMBED_SOURCES})
target_link_libraries(mkembed purpl)
