cmake_minimum_required(VERSION 3.10)

set(PURPL_DEMO_HEADERS PARENT_SCOPE)
set(PURPL_DEMO_SOURCES
	${CMAKE_CURRENT_LIST_DIR}/main.c
	${CMAKE_CURRENT_LIST_DIR}/embed.o # I really hope this works with MSVC
PARENT_SCOPE)
