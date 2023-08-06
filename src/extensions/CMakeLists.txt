cmake_minimum_required(VERSION 3.16)

set(CWL_SOURCES
		${CWL_SOURCES} 
		${CMAKE_CURRENT_SOURCE_DIR}/layer-shell.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/cutie-shell.cpp
		PARENT_SCOPE)

set(CWL_HEADERS
		${CWL_HEADERS}
		${CMAKE_CURRENT_SOURCE_DIR}/cutie-shell.h
		${CMAKE_CURRENT_SOURCE_DIR}/layer-shell.h
		PARENT_SCOPE)