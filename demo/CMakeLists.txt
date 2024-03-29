cmake_minimum_required(VERSION 3.10)

set(PURPL_DEMO_EMBED_BASENAME embed)
set(PURPL_DEMO_EMBED_FOLDER ${CMAKE_CURRENT_LIST_DIR}/${PURPL_DEMO_EMBED_BASENAME})
set(PURPL_DEMO_EMBED_ARCHIVE ${CMAKE_CURRENT_BINARY_DIR}/${PURPL_DEMO_EMBED_BASENAME}.bin)
set(PURPL_DEMO_EMBED_FILES
	app_linux.json
	app_win32.json
)

# This is an example of a CMake-integrated way of creating a C source file from a set of files
add_custom_target(embed
		  COMMAND ${CMAKE_COMMAND} -E chdir ${PURPL_DEMO_EMBED_FOLDER} ${CMAKE_COMMAND} -E tar cvf ${PURPL_DEMO_EMBED_ARCHIVE} ${PURPL_DEMO_EMBED_FILES}
		  BYPRODUCTS ${PURPL_DEMO_EMBED_ARCHIVE}
		  COMMENT "Packaging demo's embed files"
)
add_custom_target(embed_src
		  COMMAND $<TARGET_FILE:mkembed> ${PURPL_DEMO_EMBED_ARCHIVE} ${PURPL_DEMO_EMBED_BASENAME} ${CMAKE_CURRENT_BINARY_DIR}/${PURPL_DEMO_EMBED_BASENAME}.c
		  DEPENDS mkembed embed
		  BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/${PURPL_DEMO_EMBED_BASENAME}.c
		  COMMENT "Generating C source file from ${PURPL_DEMO_EMBED_ARCHIVE}"
		  VERBATIM
		  USES_TERMINAL
)

set(PURPL_DEMO_HEADERS)
set(PURPL_DEMO_SOURCES
	main.c
	${CMAKE_CURRENT_BINARY_DIR}/${PURPL_DEMO_EMBED_BASENAME}.c
)

add_executable(purpl-demo ${PURPL_DEMO_HEADERS} ${PURPL_DEMO_SOURCES})
target_link_libraries(purpl-demo purpl SDL2::SDL2main)

# Set the demo as the Visual Studio startup project
if (MSVC)
	set(VS_STARTUP_PROJECT purpl-demo)
	set_target_properties(purpl-demo PROPERTIES VS_USER_PROPS "${CMAKE_CURRENT_LIST_DIR}/purpl-demo.vcxproj.user")
endif()

# Copy SDL's output to this directory
add_custom_command(TARGET purpl-demo POST_BUILD
				   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:SDL2::SDL2> $<TARGET_FILE_DIR:purpl-demo>
)
