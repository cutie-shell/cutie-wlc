cmake_minimum_required(VERSION 3.16)
project (cutie-wlc LANGUAGES C CXX)

set(CMAKE_AUTOMOC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui WaylandCompositor)

qt_standard_project_setup()

add_subdirectory(src)

target_include_directories(${PROJECT_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(${PROJECT_NAME} PUBLIC Qt6::Core Qt6::Gui Qt6::GuiPrivate Qt6::WaylandCompositor)

install(TARGETS ${PROJECT_NAME} DESTINATION /usr/bin)
