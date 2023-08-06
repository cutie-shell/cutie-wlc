cmake_minimum_required(VERSION 3.16)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CWL_SOURCES 
		main.cpp
		view.cpp
		cutie-wlc.cpp
		workspace.cpp
		appswitcher.cpp
		glwindow.cpp)

set(CWL_HEADERS
		view.h
		cutie-wlc.h
		workspace.h
		appswitcher.h
		glwindow.h)

set(QML_RESOURCES)

add_subdirectory(extensions)

qt_add_executable(${PROJECT_NAME} ${CWL_SOURCES} ${CWL_HEADERS})

target_include_directories(${PROJECT_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/extensions)

qt6_generate_wayland_protocol_server_sources(
${PROJECT_NAME}
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/protocols/wlr-layer-shell-unstable-v1.xml
        ${CMAKE_CURRENT_SOURCE_DIR}/protocols/cutie-shell-private.xml
)