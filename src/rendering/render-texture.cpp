// SPDX-FileCopyrightText: 2026 Cutie Shell Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include "render-texture.h"

// OpenGLRenderTexture implementation

OpenGLRenderTexture::OpenGLRenderTexture(GLuint textureId, const QSize &size,
					 GLenum target)
	: m_textureId(textureId)
	, m_size(size)
	, m_target(target)
{
}

OpenGLRenderTexture::OpenGLRenderTexture()
	: m_textureId(0)
	, m_size(QSize())
	, m_target(GL_TEXTURE_2D)
{
}

void *OpenGLRenderTexture::nativeHandle() const
{
	return const_cast<GLuint *>(&m_textureId);
}

QSize OpenGLRenderTexture::size() const
{
	return m_size;
}

bool OpenGLRenderTexture::isValid() const
{
	return m_textureId != 0 && !m_size.isEmpty();
}
