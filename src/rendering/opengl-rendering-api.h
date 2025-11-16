// SPDX-FileCopyrightText: 2024 Cutie Shell Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef OPENGL_RENDERING_API_H
#define OPENGL_RENDERING_API_H

#include "rendering-api.h"
#include <GLES2/gl2.h>

/**
 * @brief OpenGL ES implementation of the rendering API
 * 
 * Provides OpenGL ES 2.0/3.0 rendering implementation.
 * This is a stub implementation for Phase 1 - actual OpenGL
 * operations will be added in later phases.
 * 
 * The implementation will eventually handle:
 * - OpenGL context management
 * - Texture creation and management
 * - Surface rendering with shaders
 * - Viewport and scissor management
 */
class OpenGLRenderingAPI : public IRenderingAPI {
	Q_OBJECT
    public:
	explicit OpenGLRenderingAPI(QObject *parent = nullptr);
	~OpenGLRenderingAPI() override;

	// IRenderingAPI interface
	bool initialize(void *platformContext) override;
	void shutdown() override;
	void beginRendering() override;
	void endRendering() override;
	void clearScreen(const QColor &color) override;

    private:
	// Platform context (will be EGLDisplay* when implemented)
	void *m_platformContext;

	// OpenGL state tracking
	bool m_initialized;
	QColor m_clearColor;

	// Will be added in later phases:
	// - Shader programs
	// - Texture cache
	// - FBO management
	// - Vertex buffers
};

#endif // OPENGL_RENDERING_API_H
