cmake_minimum_required(VERSION 3.16)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

file(GLOB_RECURSE qml_SRC CONFIGURE_DEPENDS
     "qml/*.*"
)

foreach(path IN LISTS qml_SRC)
	list(REMOVE_ITEM qml_SRC ${path})
	string(REPLACE ${CMAKE_CURRENT_SOURCE_DIR}/ "" path ${path})
    list(APPEND qml_SRC ${path})
endforeach()

set(APP_SOURCES 
		main.cpp
		launcher.cpp)

set(APP_HEADERS
		launcher.h)

qt_add_executable(${PROJECT_NAME} ${APP_SOURCES} ${APP_HEADERS})

qt_add_resources(${PROJECT_NAME} "resource"
	PREFIX /
	FILES
		${qml_SRC})