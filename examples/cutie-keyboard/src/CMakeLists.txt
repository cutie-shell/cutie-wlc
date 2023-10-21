cmake_minimum_required(VERSION 3.16)

set(CMAKE_AUTOMOC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

file(GLOB_RECURSE qml_SRC CONFIGURE_DEPENDS
     "qml/*.*"
)

set(APP_SOURCES 
		main.cpp
		cutiekeyboard.cpp)

set(APP_HEADERS
		cutiekeyboard.cpp)

add_subdirectory(extensions)

foreach(path IN LISTS qml_SRC)
	list(REMOVE_ITEM qml_SRC ${path})
	string(REPLACE ${CMAKE_CURRENT_SOURCE_DIR}/ "" path ${path})
    list(APPEND qml_SRC ${path})
endforeach()

add_executable(${PROJECT_NAME} ${APP_SOURCES} ${APP_HEADERS})

qt_add_resources(${PROJECT_NAME} "resource"
	PREFIX "/"
	FILES
		${qml_SRC})

target_include_directories(${PROJECT_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/extensions)

qt6_generate_wayland_protocol_client_sources(
${PROJECT_NAME}
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/protocols/input-method-unstable-v2.xml
)

qt6_add_qml_module(${PROJECT_NAME}
  URI "CutieIM"
  VERSION 1.0
)
