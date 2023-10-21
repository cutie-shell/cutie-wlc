cmake_minimum_required(VERSION 3.16)
project (cutie-keyboard LANGUAGES C CXX)

set(CMAKE_AUTOMOC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt6 REQUIRED COMPONENTS Core  Quick Qml WaylandClient)
find_package(LayerShellQt6 REQUIRED)

qt_standard_project_setup()

add_subdirectory(src)

target_include_directories(${PROJECT_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(${PROJECT_NAME} PUBLIC Qt6::Core Qt6::Quick Qt6::Qml Qt6::WaylandClient LayerShellQtInterface6)

install(TARGETS ${PROJECT_NAME} DESTINATION /usr/bin)

#[[cmake_minimum_required(VERSION 3.16)
project (cutie-keyboard LANGUAGES C CXX)

set(CMAKE_AUTOMOC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Quick Qml WaylandClient)

qt_standard_project_setup()

file(GLOB_RECURSE qml_SRC CONFIGURE_DEPENDS
     "src/qml/*.*"
)

add_executable(
	${PROJECT_NAME}
	src/main.cpp
	src/cutiekeyboard.h
	src/cutiekeyboard.cpp
	src/extensions/virtualkeyboard.h
	src/extensions/virtualkeyboard.cpp
	src/extensions/input-method-v2.h
	src/extensions/input-method-v2.cpp
)

foreach(path IN LISTS qml_SRC)
	list(REMOVE_ITEM qml_SRC ${path})
	string(REPLACE ${CMAKE_CURRENT_SOURCE_DIR}/ "" path ${path})
    list(APPEND qml_SRC ${path})
endforeach()

qt_add_resources(${PROJECT_NAME} "resource"
	PREFIX "/"
	FILES
		${qml_SRC})

qt6_generate_wayland_protocol_client_sources(
${PROJECT_NAME}
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/protocols/virtual-keyboard-unstable-v1.xml
        ${CMAKE_CURRENT_SOURCE_DIR}/protocols/input-method-unstable-v2.xml
)

target_include_directories(${PROJECT_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/src/extensions)

target_link_libraries(${PROJECT_NAME} PUBLIC LayerShellQtInterface6 Qt6::Core Qt6::Widgets Qt6::Quick Qt6::Qml Qt::WaylandClient)

qt6_add_qml_module(${PROJECT_NAME}
  URI "CutieKeyboard"
  VERSION 1.0
)

install(TARGETS ${PROJECT_NAME} DESTINATION /usr/bin)
]]