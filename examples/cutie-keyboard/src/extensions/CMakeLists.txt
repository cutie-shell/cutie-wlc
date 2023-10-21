cmake_minimum_required(VERSION 3.16)

set(APP_SOURCES
		${APP_SOURCES}
		${CMAKE_CURRENT_SOURCE_DIR}/input-method-v2.cpp
		PARENT_SCOPE)

set(APP_HEADERS
		${APP_HEADERS}
		${CMAKE_CURRENT_SOURCE_DIR}/input-method-v2.h
		PARENT_SCOPE)