cmake_minimum_required(VERSION 3.10)

set(PURPL_COMMON_HEADERS
	${CMAKE_CURRENT_LIST_DIR}/purpl/app_info.h
	${CMAKE_CURRENT_LIST_DIR}/purpl/asset.h
	${CMAKE_CURRENT_LIST_DIR}/purpl/log.h
	${CMAKE_CURRENT_LIST_DIR}/purpl/purpl.h
	${CMAKE_CURRENT_LIST_DIR}/purpl/types.h
	${CMAKE_CURRENT_LIST_DIR}/purpl/util.h
	${CMAKE_CURRENT_LIST_DIR}/purpl/window.h
)

set(PURPL_HEADERS ${PURPL_COMMON_HEADERS} PARENT_SCOPE)
