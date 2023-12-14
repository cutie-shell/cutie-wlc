cmake_minimum_required(VERSION 3.16)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CWL_SOURCES 
		main.cpp
		view.cpp
		cutie-wlc.cpp
		workspace.cpp
		glwindow.cpp
		gesture.cpp)

set(CWL_HEADERS
		view.h
		cutie-wlc.h
		workspace.h
		glwindow.h
		gesture.h)

set(QML_RESOURCES)

add_subdirectory(extensions)

qt_add_executable(${PROJECT_NAME} ${CWL_SOURCES} ${CWL_HEADERS})

target_include_directories(${PROJECT_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/extensions)

qt6_generate_wayland_protocol_server_sources(
${PROJECT_NAME}
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/protocols/wlr-layer-shell-unstable-v1.xml
        ${CMAKE_CURRENT_SOURCE_DIR}/protocols/wlr-output-management-unstable-v1.xml
        ${CMAKE_CURRENT_SOURCE_DIR}/protocols/wlr-output-power-management-unstable-v1.xml
        ${CMAKE_CURRENT_SOURCE_DIR}/protocols/wlr-screencopy-unstable-v1.xml
        ${CMAKE_CURRENT_SOURCE_DIR}/protocols/cutie-shell-private.xml
        ${CMAKE_CURRENT_SOURCE_DIR}/protocols/text-input-unstable-v1.xml
        ${CMAKE_CURRENT_SOURCE_DIR}/protocols/text-input-unstable-v2.xml
        ${CMAKE_CURRENT_SOURCE_DIR}/protocols/text-input-unstable-v3.xml
        ${CMAKE_CURRENT_SOURCE_DIR}/protocols/wlr-foreign-toplevel-management-unstable-v1.xml
        ${CMAKE_CURRENT_SOURCE_DIR}/protocols/input-method-unstable-v2.xml
)