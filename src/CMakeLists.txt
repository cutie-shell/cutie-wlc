cmake_minimum_required(VERSION 3.16)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CWL_SOURCES 
		main.cpp
		view.cpp
		cutie-wlc.cpp
		workspace.cpp
		glwindow.cpp
		animation-controller.cpp
		gesture/gesture.cpp
		gesture/gesture-manager.cpp
		gesture/gesture-action.cpp
		process-manager.cpp
		opengl/opengl-guards.cpp)

set(CWL_HEADERS
		view.h
		cutie-wlc.h
		workspace.h
		glwindow.h
		animation-controller.h
		gesture/gesture.h
		gesture/gesture-manager.h
		gesture/gesture-action.h
		process-manager.h
		opengl/opengl-guards.h)

set(QML_RESOURCES)

add_subdirectory(extensions)
add_subdirectory(rendering)
add_subdirectory(input)
add_subdirectory(surface)

# Prepend rendering directory path to rendering sources
foreach(source ${RENDERING_SOURCES})
    list(APPEND CWL_RENDERING_SOURCES ${RENDERING_DIR}/${source})
endforeach()

# Prepend input directory path to input sources
foreach(source ${INPUT_SOURCES})
    list(APPEND CWL_INPUT_SOURCES input/${source})
endforeach()

# Prepend surface directory path to surface sources
foreach(source ${SURFACE_SOURCES})
    list(APPEND CWL_SURFACE_SOURCES surface/${source})
endforeach()

qt_add_executable(${PROJECT_NAME} ${CWL_SOURCES} ${CWL_HEADERS} ${CWL_RENDERING_SOURCES} ${CWL_INPUT_SOURCES} ${CWL_SURFACE_SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/extensions
	${CMAKE_CURRENT_SOURCE_DIR}/gesture
	${CMAKE_CURRENT_SOURCE_DIR}/opengl
	${CMAKE_CURRENT_SOURCE_DIR}/rendering
	${CMAKE_CURRENT_SOURCE_DIR}/input
	${CMAKE_CURRENT_SOURCE_DIR}/surface
	${GLES2_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PUBLIC 
	Qt6::Core 
	Qt6::Gui 
	Qt6::GuiPrivate 
	Qt6::WaylandCompositor
	${GLES2_LIBRARIES})

qt6_generate_wayland_protocol_server_sources(
${PROJECT_NAME}
    FILES
        ${WAYLAND_PROTOCOLS_DATADIR}/stable/xdg-shell/xdg-shell.xml
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

# Install target
install(TARGETS ${PROJECT_NAME} DESTINATION /usr/bin)