cmake_minimum_required(VERSION 3.16)

set(CWL_SOURCES
		${CWL_SOURCES} 
		${CMAKE_CURRENT_SOURCE_DIR}/layer-shell.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/screencopy.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/cutie-shell.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/text-input-v1.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/text-input-v2.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/text-input-v3.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/virtual-keyboard-v1.cpp
		PARENT_SCOPE)

set(CWL_HEADERS
		${CWL_HEADERS}
		${CMAKE_CURRENT_SOURCE_DIR}/cutie-shell.h
		${CMAKE_CURRENT_SOURCE_DIR}/layer-shell.h
		${CMAKE_CURRENT_SOURCE_DIR}/screencopy.h
		${CMAKE_CURRENT_SOURCE_DIR}/text-input-v1.h
		${CMAKE_CURRENT_SOURCE_DIR}/text-input-v2.h
		${CMAKE_CURRENT_SOURCE_DIR}/text-input-v3.h
		${CMAKE_CURRENT_SOURCE_DIR}/virtual-keyboard-v1.h
		PARENT_SCOPE)