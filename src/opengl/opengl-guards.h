#pragma once

#include <QOpenGLFramebufferObject>
#include <QOpenGLTextureBlitter>
#include <QOpenGLFunctions>
#include <QOpenGLContext>
#include <QScopedPointer>
#include <QImage>

QT_BEGIN_NAMESPACE

/**
 * @brief RAII wrapper for QOpenGLFramebufferObject
 * 
 * Ensures proper binding and automatic cleanup of OpenGL framebuffers.
 * The framebuffer is automatically bound on construction and unbound
 * (restored to default) on destruction.
 */
class OpenGLFramebufferGuard {
    public:
	/**
     * @brief Construct and bind an OpenGL framebuffer
     * @param size Size of the framebuffer
     * @param format Internal format for the framebuffer (optional)
     */
	explicit OpenGLFramebufferGuard(
		const QSize &size,
		QOpenGLFramebufferObject::Attachment attachment =
			QOpenGLFramebufferObject::CombinedDepthStencil);

	/**
     * @brief Destructor - automatically unbinds the framebuffer
     */
	~OpenGLFramebufferGuard();

	/**
     * @brief Get the underlying framebuffer object
     * @return Pointer to the QOpenGLFramebufferObject
     */
	QOpenGLFramebufferObject *framebuffer() const;

	/**
     * @brief Get the framebuffer as an image
     * @return QImage representation of the framebuffer contents
     */
	QImage toImage() const;

	/**
     * @brief Check if the framebuffer is valid
     * @return true if the framebuffer was created successfully
     */
	bool isValid() const;

    private:
	QScopedPointer<QOpenGLFramebufferObject> m_fbo;
	GLint m_previousFramebuffer;

	// Non-copyable
	Q_DISABLE_COPY(OpenGLFramebufferGuard)
};

/**
 * @brief RAII wrapper for QOpenGLTextureBlitter
 * 
 * Ensures proper lifecycle management of texture blitters with
 * automatic creation and cleanup.
 */
class OpenGLTextureBlitterGuard {
    public:
	/**
     * @brief Construct and create a texture blitter
     */
	OpenGLTextureBlitterGuard();

	/**
     * @brief Destructor - automatically releases the blitter
     */
	~OpenGLTextureBlitterGuard();

	/**
     * @brief Get the underlying texture blitter
     * @return Pointer to the QOpenGLTextureBlitter
     */
	QOpenGLTextureBlitter *blitter() const;

	/**
     * @brief Bind the blitter with a specific target
     * @param target OpenGL texture target (e.g., GL_TEXTURE_2D)
     */
	void bind(GLenum target = GL_TEXTURE_2D);

	/**
     * @brief Release the blitter
     */
	void release();

	/**
     * @brief Set the opacity for blitting operations
     * @param opacity Opacity value (0.0 to 1.0)
     */
	void setOpacity(float opacity);

	/**
     * @brief Blit a texture
     * @param textureId OpenGL texture ID
     * @param targetTransform Transformation matrix
     * @param sourceOrigin Origin of the source texture
     */
	void blit(GLuint textureId, const QMatrix4x4 &targetTransform,
		  QOpenGLTextureBlitter::Origin sourceOrigin);

	/**
     * @brief Check if the blitter is valid
     * @return true if the blitter was created successfully
     */
	bool isValid() const;

    private:
	QScopedPointer<QOpenGLTextureBlitter> m_blitter;
	bool m_created;
	bool m_bound;

	// Non-copyable
	Q_DISABLE_COPY(OpenGLTextureBlitterGuard)
};

/**
 * @brief RAII wrapper for OpenGL state management
 * 
 * Saves and restores OpenGL state automatically, ensuring that
 * temporary state changes don't affect the global OpenGL context.
 */
class OpenGLStateGuard {
    public:
	/**
     * @brief Construct and save current OpenGL state
     */
	OpenGLStateGuard();

	/**
     * @brief Destructor - automatically restores saved state
     */
	~OpenGLStateGuard();

	/**
     * @brief Set viewport
     * @param x X coordinate
     * @param y Y coordinate  
     * @param width Viewport width
     * @param height Viewport height
     */
	void setViewport(GLint x, GLint y, GLsizei width, GLsizei height);

	/**
     * @brief Set clear color
     * @param red Red component (0.0-1.0)
     * @param green Green component (0.0-1.0)
     * @param blue Blue component (0.0-1.0)
     * @param alpha Alpha component (0.0-1.0)
     */
	void setClearColor(GLfloat red, GLfloat green, GLfloat blue,
			   GLfloat alpha);

	/**
     * @brief Clear the framebuffer
     * @param mask Clear mask (e.g., GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
     */
	void clear(GLbitfield mask);

	/**
     * @brief Enable blending with alpha
     */
	void enableAlphaBlending();

	/**
     * @brief Disable blending
     */
	void disableBlending();

    private:
	QOpenGLFunctions *m_functions;

	// Saved state
	GLint m_savedViewport[4];
	GLfloat m_savedClearColor[4];
	GLboolean m_savedBlendEnabled;
	GLint m_savedBlendSrcRgb;
	GLint m_savedBlendDstRgb;
	GLint m_savedBlendSrcAlpha;
	GLint m_savedBlendDstAlpha;

	// Non-copyable
	Q_DISABLE_COPY(OpenGLStateGuard)
};

QT_END_NAMESPACE