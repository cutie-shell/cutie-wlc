// SPDX-FileCopyrightText: 2026 Cutie Shell Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef RENDER_TEXTURE_H
#define RENDER_TEXTURE_H

#include <QSize>
#include <GLES2/gl2.h>

/**
 * @brief Abstract texture handle for rendering
 * 
 * Provides an API-agnostic interface to texture resources.
 * Concrete implementations (OpenGL, Vulkan) provide the actual texture data.
 * 
 * The texture abstraction allows the compositor to work with textures
 * without knowing the underlying rendering API details.
 */
class RenderTexture {
    public:
	virtual ~RenderTexture() = default;

	/**
     * @brief Get the native texture handle
     * @return Opaque pointer to the native texture (e.g., GLuint* for OpenGL)
     */
	virtual void *nativeHandle() const = 0;

	/**
     * @brief Get the texture size
     * @return Size of the texture in pixels
     */
	virtual QSize size() const = 0;

	/**
     * @brief Check if the texture is valid
     * @return true if the texture is valid and can be used for rendering
     */
	virtual bool isValid() const = 0;
};

/**
 * @brief OpenGL ES implementation of RenderTexture
 * 
 * Wraps an OpenGL texture ID and provides texture metadata.
 * Supports both GL_TEXTURE_2D and GL_TEXTURE_EXTERNAL_OES targets.
 */
class OpenGLRenderTexture : public RenderTexture {
    public:
	/**
     * @brief Construct an OpenGL texture wrapper
     * @param textureId OpenGL texture ID (0 for invalid texture)
     * @param size Size of the texture in pixels
     * @param target GL texture target (GL_TEXTURE_2D or GL_TEXTURE_EXTERNAL_OES)
     */
	OpenGLRenderTexture(GLuint textureId, const QSize &size, GLenum target);

	/**
     * @brief Construct an invalid texture
     */
	OpenGLRenderTexture();

	~OpenGLRenderTexture() override = default;

	void *nativeHandle() const override;
	QSize size() const override;
	bool isValid() const override;

	/**
     * @brief Get the OpenGL texture ID
     * @return OpenGL texture ID (0 if invalid)
     */
	GLuint textureId() const
	{
		return m_textureId;
	}

	/**
     * @brief Get the OpenGL texture target
     * @return GL_TEXTURE_2D or GL_TEXTURE_EXTERNAL_OES
     */
	GLenum target() const
	{
		return m_target;
	}

    private:
	GLuint m_textureId;
	QSize m_size;
	GLenum m_target;
};

#endif // RENDER_TEXTURE_H
