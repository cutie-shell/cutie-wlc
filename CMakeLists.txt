cmake_minimum_required(VERSION 3.16)
project (cutie-wlc LANGUAGES C CXX)

set(CMAKE_AUTOMOC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui WaylandCompositor)
find_package(PkgConfig REQUIRED)
pkg_check_modules(WAYLAND_PROTOCOLS REQUIRED wayland-protocols)
pkg_get_variable(WAYLAND_PROTOCOLS_DATADIR wayland-protocols pkgdatadir)
pkg_check_modules(GLES2 REQUIRED glesv2)

qt_standard_project_setup()

add_subdirectory(src)

# Add tests subdirectory (use option to enable/disable)
option(BUILD_TESTING "Build tests" ON)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

target_include_directories(${PROJECT_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${GLES2_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PUBLIC 
	Qt6::Core 
	Qt6::Gui 
	Qt6::GuiPrivate 
	Qt6::WaylandCompositor
	${GLES2_LIBRARIES})

install(TARGETS ${PROJECT_NAME} DESTINATION /usr/bin)
