// SPDX-FileCopyrightText: 2026 Cutie Shell Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include "opengl-rendering-api.h"
#include <QDebug>

OpenGLRenderingAPI::OpenGLRenderingAPI(QObject *parent)
	: IRenderingAPI(parent)
	, m_platformContext(nullptr)
	, m_initialized(false)
	, m_clearColor(Qt::black)
{
	qDebug() << "OpenGLRenderingAPI: Created";
}

OpenGLRenderingAPI::~OpenGLRenderingAPI()
{
	if (m_initialized) {
		shutdown();
	}
	qDebug() << "OpenGLRenderingAPI: Destroyed";
}

bool OpenGLRenderingAPI::initialize(void *platformContext)
{
	qDebug() << "OpenGLRenderingAPI: Initialize (stub)";

	if (m_initialized) {
		qWarning() << "OpenGLRenderingAPI: Already initialized";
		return true;
	}

	m_platformContext = platformContext;

	// TODO: In Phase 2, this will:
	// - Create EGL context
	// - Initialize OpenGL state
	// - Compile shaders
	// - Set up vertex buffers

	m_initialized = true;
	return true;
}

void OpenGLRenderingAPI::shutdown()
{
	qDebug() << "OpenGLRenderingAPI: Shutdown (stub)";

	if (!m_initialized) {
		return;
	}

	// TODO: In Phase 2, this will:
	// - Destroy OpenGL resources
	// - Release EGL context
	// - Clean up shaders and buffers

	m_initialized = false;
	m_platformContext = nullptr;
}

void OpenGLRenderingAPI::beginRendering()
{
	// Stub implementation - no actual OpenGL calls yet
	// TODO: In Phase 2, this will:
	// - Make context current
	// - Set up viewport
	// - Bind default framebuffer
}

void OpenGLRenderingAPI::endRendering()
{
	// Stub implementation - no actual OpenGL calls yet
	// TODO: In Phase 2, this will:
	// - Flush OpenGL commands
	// - Maybe swap buffers (or defer to platform backend)
}

void OpenGLRenderingAPI::clearScreen(const QColor &color)
{
	m_clearColor = color;

	// Stub implementation - no actual OpenGL calls yet
	// TODO: In Phase 2, this will:
	// - glClearColor(r, g, b, a)
	// - glClear(GL_COLOR_BUFFER_BIT)
}
