cmake_minimum_required(VERSION 3.10)

set(PURPL_DEMO_EMBED_FILES
	ffmpeg.txt
)

add_custom_target(embed.c
		  COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_LIST_DIR}/embed ${CMAKE_COMMAND} -E tar cvJf ${CMAKE_CURRENT_BINARY_DIR}/embed.bin ${PURPL_DEMO_EMBED_FILES}
		  COMMAND ${CMAKE_BINARY_DIR}/tools/mkembed ${CMAKE_CURRENT_BINARY_DIR}/embed.bin embed_bin ${CMAKE_CURRENT_BINARY_DIR}/embed.c
		  DEPENDS mkembed
		  BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/embed.bin ${CMAKE_CURRENT_BINARY_DIR}/embed.c
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
