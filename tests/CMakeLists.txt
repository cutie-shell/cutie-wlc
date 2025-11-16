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
        ${GLES2_INCLUDE_DIRS}
    )
    
    # Add as a test
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
    
    # Set test properties
    set_tests_properties(${TEST_NAME} PROPERTIES
        TIMEOUT 30
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
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
