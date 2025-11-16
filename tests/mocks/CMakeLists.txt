# Mock objects for testing
# These provide controllable implementations of interfaces for unit tests

# Mock library that can be linked by tests
add_library(cutie-wlc-mocks STATIC
    mock-rendering-api.cpp
    mock-rendering-api.h
    mock-surface-compositor.cpp
    mock-surface-compositor.h
)

# Link against required libraries
target_link_libraries(cutie-wlc-mocks
    Qt6::Core
    Qt6::Gui
)

# Include directories
target_include_directories(cutie-wlc-mocks PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/rendering
    ${CMAKE_SOURCE_DIR}/src/surface
)

# Enable automoc for Qt meta-object compilation
set_target_properties(cutie-wlc-mocks PROPERTIES
    AUTOMOC ON
)
