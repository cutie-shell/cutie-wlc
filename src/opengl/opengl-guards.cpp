#include "opengl-guards.h"
#include <QOpenGLContext>
#include <QImage>
#include <QDebug>

QT_BEGIN_NAMESPACE

// ============================================================================
// OpenGLFramebufferGuard Implementation
// ============================================================================

OpenGLFramebufferGuard::OpenGLFramebufferGuard(
	const QSize &size, QOpenGLFramebufferObject::Attachment attachment)
	: m_fbo(new QOpenGLFramebufferObject(size, attachment))
	, m_previousFramebuffer(0)
{
	if (!m_fbo->isValid()) {
		qWarning()
			<< "OpenGLFramebufferGuard: Failed to create framebuffer object";
		return;
	}

	// Save current framebuffer binding
	QOpenGLFunctions *functions =
		QOpenGLContext::currentContext()->functions();
	functions->glGetIntegerv(GL_FRAMEBUFFER_BINDING,
				 &m_previousFramebuffer);

	// Bind our framebuffer
	if (!m_fbo->bind()) {
		qWarning()
			<< "OpenGLFramebufferGuard: Failed to bind framebuffer";
	}
}

OpenGLFramebufferGuard::~OpenGLFramebufferGuard()
{
	if (m_fbo && m_fbo->isValid()) {
		// Restore previous framebuffer binding
		QOpenGLFunctions *functions =
			QOpenGLContext::currentContext()->functions();
		if (functions) {
			functions->glBindFramebuffer(GL_FRAMEBUFFER,
						     m_previousFramebuffer);
		}
	}
}

QOpenGLFramebufferObject *OpenGLFramebufferGuard::framebuffer() const
{
	return m_fbo.data();
}

QImage OpenGLFramebufferGuard::toImage() const
{
	if (m_fbo && m_fbo->isValid()) {
		return m_fbo->toImage();
	}
	return QImage();
}

bool OpenGLFramebufferGuard::isValid() const
{
	return m_fbo && m_fbo->isValid();
}

// ============================================================================
// OpenGLTextureBlitterGuard Implementation
// ============================================================================

OpenGLTextureBlitterGuard::OpenGLTextureBlitterGuard()
	: m_blitter(new QOpenGLTextureBlitter)
	, m_created(false)
	, m_bound(false)
{
	if (m_blitter->create()) {
		m_created = true;
	} else {
		qWarning()
			<< "OpenGLTextureBlitterGuard: Failed to create texture blitter";
	}
}

OpenGLTextureBlitterGuard::~OpenGLTextureBlitterGuard()
{
	if (m_bound) {
		release();
	}

	if (m_created && m_blitter) {
		m_blitter->destroy();
	}
}

QOpenGLTextureBlitter *OpenGLTextureBlitterGuard::blitter() const
{
	return m_blitter.data();
}

void OpenGLTextureBlitterGuard::bind(GLenum target)
{
	if (m_created && m_blitter) {
		m_blitter->bind(target);
		m_bound = true;
	}
}

void OpenGLTextureBlitterGuard::release()
{
	if (m_bound && m_blitter) {
		m_blitter->release();
		m_bound = false;
	}
}

void OpenGLTextureBlitterGuard::setOpacity(float opacity)
{
	if (m_blitter) {
		m_blitter->setOpacity(opacity);
	}
}

void OpenGLTextureBlitterGuard::blit(GLuint textureId,
				     const QMatrix4x4 &targetTransform,
				     QOpenGLTextureBlitter::Origin sourceOrigin)
{
	if (m_bound && m_blitter) {
		m_blitter->blit(textureId, targetTransform, sourceOrigin);
	}
}

bool OpenGLTextureBlitterGuard::isValid() const
{
	return m_created && m_blitter;
}

// ============================================================================
// OpenGLStateGuard Implementation
// ============================================================================

OpenGLStateGuard::OpenGLStateGuard()
	: m_functions(nullptr)
{
	QOpenGLContext *context = QOpenGLContext::currentContext();
	if (!context) {
		qWarning() << "OpenGLStateGuard: No current OpenGL context";
		return;
	}

	m_functions = context->functions();
	if (!m_functions) {
		qWarning()
			<< "OpenGLStateGuard: Failed to get OpenGL functions";
		return;
	}

	// Save current OpenGL state
	m_functions->glGetIntegerv(GL_VIEWPORT, m_savedViewport);
	m_functions->glGetFloatv(GL_COLOR_CLEAR_VALUE, m_savedClearColor);

	m_savedBlendEnabled = m_functions->glIsEnabled(GL_BLEND);
	if (m_savedBlendEnabled) {
		m_functions->glGetIntegerv(GL_BLEND_SRC_RGB,
					   &m_savedBlendSrcRgb);
		m_functions->glGetIntegerv(GL_BLEND_DST_RGB,
					   &m_savedBlendDstRgb);
		m_functions->glGetIntegerv(GL_BLEND_SRC_ALPHA,
					   &m_savedBlendSrcAlpha);
		m_functions->glGetIntegerv(GL_BLEND_DST_ALPHA,
					   &m_savedBlendDstAlpha);
	}
}

OpenGLStateGuard::~OpenGLStateGuard()
{
	if (!m_functions) {
		return;
	}

	// Restore saved OpenGL state
	m_functions->glViewport(m_savedViewport[0], m_savedViewport[1],
				m_savedViewport[2], m_savedViewport[3]);

	m_functions->glClearColor(m_savedClearColor[0], m_savedClearColor[1],
				  m_savedClearColor[2], m_savedClearColor[3]);

	if (m_savedBlendEnabled) {
		m_functions->glEnable(GL_BLEND);
		m_functions->glBlendFuncSeparate(m_savedBlendSrcRgb,
						 m_savedBlendDstRgb,
						 m_savedBlendSrcAlpha,
						 m_savedBlendDstAlpha);
	} else {
		m_functions->glDisable(GL_BLEND);
	}
}

void OpenGLStateGuard::setViewport(GLint x, GLint y, GLsizei width,
				   GLsizei height)
{
	if (m_functions) {
		m_functions->glViewport(x, y, width, height);
	}
}

void OpenGLStateGuard::setClearColor(GLfloat red, GLfloat green, GLfloat blue,
				     GLfloat alpha)
{
	if (m_functions) {
		m_functions->glClearColor(red, green, blue, alpha);
	}
}

void OpenGLStateGuard::clear(GLbitfield mask)
{
	if (m_functions) {
		m_functions->glClear(mask);
	}
}

void OpenGLStateGuard::enableAlphaBlending()
{
	if (m_functions) {
		m_functions->glEnable(GL_BLEND);
		m_functions->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
}

void OpenGLStateGuard::disableBlending()
{
	if (m_functions) {
		m_functions->glDisable(GL_BLEND);
	}
}

QT_END_NAMESPACE