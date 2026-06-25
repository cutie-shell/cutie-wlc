# SPDX-FileCopyrightText: 2026 Cutie Shell Contributors
# SPDX-License-Identifier: GPL-3.0-or-later

# Rendering backend sources
set(RENDERING_SOURCES
    rendering-api.h
    rendering-api.cpp
    render-texture.h
    render-texture.cpp
    opengl-rendering-api.h
    opengl-rendering-api.cpp
)

# Add sources to parent scope so they can be included in the main target
set(RENDERING_SOURCES ${RENDERING_SOURCES} PARENT_SCOPE)

# Make the rendering directory path available
set(RENDERING_DIR ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
