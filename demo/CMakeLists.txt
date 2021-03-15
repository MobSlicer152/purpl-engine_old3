cmake_minimum_required(VERSION 3.10)

set(PURPL_DEMO_EMBED_FILES
	app.json
)

# This is an example of a CMake-integrated way of creating a C source file from a set of files
add_custom_target(embed.c
		  COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_LIST_DIR}/embed ${CMAKE_COMMAND} -E tar cvf ${CMAKE_CURRENT_BINARY_DIR}/embed.tar ${PURPL_DEMO_EMBED_FILES}
		  COMMAND $<TARGET_FILE:mkembed> ${CMAKE_CURRENT_BINARY_DIR}/embed.tar embed ${CMAKE_CURRENT_BINARY_DIR}/embed.c
		  DEPENDS mkembed
		  BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/embed.c ${CMAKE_CURRENT_BINARY_DIR}/embed.tar
		  VERBATIM
		  USES_TERMINAL
)

set(PURPL_DEMO_HEADERS)
set(PURPL_DEMO_SOURCES
	main.c
	${CMAKE_CURRENT_BINARY_DIR}/embed.c
)

add_executable(purpl-demo ${PURPL_DEMO_HEADERS} ${PURPL_DEMO_SOURCES})
target_link_libraries(purpl-demo purpl)
