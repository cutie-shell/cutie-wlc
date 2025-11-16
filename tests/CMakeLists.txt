# Test infrastructure for cutie-wlc
# Uses Qt Test framework for unit and integration testing

cmake_minimum_required(VERSION 3.16)

# Find Qt Test module
find_package(Qt6 REQUIRED COMPONENTS Test)

# Enable testing
enable_testing()

# Set common test properties
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Include directories from main project
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/rendering
    ${CMAKE_SOURCE_DIR}/src/input
    ${CMAKE_SOURCE_DIR}/src/surface
    ${CMAKE_SOURCE_DIR}/src/extensions
    ${CMAKE_BINARY_DIR}/src  # For generated Wayland protocol headers
)

# Helper function to add a test executable
function(add_cutie_test TEST_NAME)
    # Create test executable
    add_executable(${TEST_NAME} ${ARGN})
    
    # Link against Qt Test and required libraries
    target_link_libraries(${TEST_NAME}
        Qt6::Test
        Qt6::Core
        Qt6::Gui
        Qt6::WaylandCompositor
        ${GLES2_LIBRARIES}
    )
    
    # Include directories
    target_include_directories(${TEST_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/extensions
        ${CMAKE_BINARY_DIR}/src  # For generated Wayland protocol headers
        ${GLES2_INCLUDE_DIRS}
    )
endfunction()

# Add subdirectories for test organization
add_subdirectory(mocks)

# Individual test files
add_cutie_test(test-render-texture 
    test-render-texture.cpp
    ${CMAKE_SOURCE_DIR}/src/rendering/render-texture.cpp
)

add_cutie_test(test-input-events
    test-input-events.cpp
)

add_cutie_test(test-input-device-manager
    test-input-device-manager.cpp
    ${CMAKE_SOURCE_DIR}/src/input/input-device-manager.cpp
)

add_cutie_test(test-input-router
    test-input-router.cpp
    ${CMAKE_SOURCE_DIR}/src/input/input-router.cpp
    ${CMAKE_SOURCE_DIR}/src/surface/surface-compositor.cpp
)

# Link against mocks library for CwlView meta-object
target_link_libraries(test-input-router cutie-wlc-mocks)

add_cutie_test(test-surface-compositor
    test-surface-compositor.cpp
    ${CMAKE_SOURCE_DIR}/src/surface/surface-compositor.cpp
)

# Link against mocks library for CwlView meta-object
target_link_libraries(test-surface-compositor cutie-wlc-mocks)

# Register tests with CTest
add_test(NAME test-render-texture COMMAND test-render-texture)
add_test(NAME test-input-events COMMAND test-input-events)
add_test(NAME test-input-device-manager COMMAND test-input-device-manager)
add_test(NAME test-input-router COMMAND test-input-router)
add_test(NAME test-surface-compositor COMMAND test-surface-compositor)

# Set test properties
set_tests_properties(
    test-render-texture
    test-input-events
    test-input-device-manager
    test-input-router
    test-surface-compositor
    PROPERTIES
        TIMEOUT 30
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# Custom target to run all tests
add_custom_target(run-tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure --verbose
    DEPENDS 
        test-render-texture
        test-input-events
        test-input-device-manager
        test-input-router
        test-surface-compositor
    COMMENT "Running all cutie-wlc tests..."
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
