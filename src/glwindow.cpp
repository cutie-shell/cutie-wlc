#include <glwindow.h>

#include <QPainter>
#include <QMatrix4x4>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QOpenGLContext>
#include <QMouseEvent>
#include <opengl/opengl-guards.h>
#include <QGuiApplication>
#include <qpa/qplatformnativeinterface.h>
#include <qpa/qplatformscreen.h>
#include <QtWaylandCompositor/QWaylandSeat>
#include <stdexcept>
#include <QDebug>
#include <QtWaylandCompositor/QWaylandSeat>

GlWindow::GlWindow()
{
	QGuiApplication::platformNativeInterface()->nativeResourceForIntegration(
		"displayon");
	m_displayOff = false;
}

void GlWindow::setCompositor(CwlCompositor *cwlcompositor)
{
	m_cwlcompositor = cwlcompositor;
	m_gesture.reset(
		new CwlGesture(cwlcompositor, QSize(width(), height())));
}

bool GlWindow::displayOff()
{
	return m_displayOff;
}

void GlWindow::setDisplayOff(bool displayOff)
{
	if (m_displayOff == displayOff)
		return;

	QGuiApplication::primaryScreen()->handle()->setPowerState(
		displayOff ? QPlatformScreen::PowerStateOff :
			     QPlatformScreen::PowerStateOn);

	if (displayOff) {
		if (m_cwlcompositor) {
			m_cwlcompositor->setLauncherPosition(0.0);
			m_cwlcompositor->onHideKeyboard();
		}
	} else
		requestUpdate();

	m_displayOff = displayOff;
	emit displayOffChanged(m_displayOff);
}

void GlWindow::initializeGL()
{
	m_textureBlitter.reset(new OpenGLTextureBlitterGuard());
	emit glReady();
}

void GlWindow::paintGL()
{
	if (m_displayOff)
		return;

	if (!m_cwlcompositor)
		return;

	m_cwlcompositor->startRender();

	OpenGLStateGuard stateGuard;
	setupRenderingContext(&stateGuard);

	// Prepare views for rendering
	QList<CwlView *> viewList;
	if (m_cwlcompositor->m_launcherView)
		viewList = m_cwlcompositor->getViews()
			   << m_cwlcompositor->m_launcherView;
	else
		viewList = m_cwlcompositor->getViews();

	renderViews(viewList);

	if (m_textureBlitter)
		m_textureBlitter->release();
	m_cwlcompositor->endRender();
}

void GlWindow::setupRenderingContext(OpenGLStateGuard *stateGuard)
{
	stateGuard->setClearColor(.0f, .0f, .0f, 1.f);
	stateGuard->clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_currentTarget = GL_TEXTURE_2D;
	if (m_textureBlitter)
		m_textureBlitter->bind(m_currentTarget);
	stateGuard->enableAlphaBlending();
}

qreal GlWindow::calculateViewOpacity(CwlView *view) const
{
	if (!view || !m_cwlcompositor)
		return 1.0;

	QString appId;
	if (view->isToplevel())
		appId = view->getAppId();

	if (appId == "cutie-launcher") {
		// Launcher opacity based on position
		return 1.0 -
		       (m_cwlcompositor->m_launcherView->getPosition().y() *
			m_cwlcompositor->scaleFactor() / height());
	} else if (view->isToplevel()) {
		// Toplevel view opacity with launcher interaction
		if (m_cwlcompositor->launcherPosition() > 0.0) {
			return m_cwlcompositor->blur() *
			       m_cwlcompositor->m_launcherView->getPosition()
				       .y() *
			       m_cwlcompositor->scaleFactor() / height();
		} else {
			return m_cwlcompositor->blur();
		}
	} else {
		// Default opacity for other views
		return 1.0;
	}
}

void GlWindow::renderViews(const QList<CwlView *> &views)
{
	if (!m_cwlcompositor)
		return;

	for (CwlView *view : views) {
		// Skip top layer views when launcher is fully open
		if (m_cwlcompositor->launcherPosition() == 1.0 &&
		    view->layer == CwlViewLayer::TOP)
			continue;

		// Set view opacity and render
		qreal opacity = calculateViewOpacity(view);
		if (m_textureBlitter) {
			m_textureBlitter->setOpacity(opacity);
		}
		renderView(view);
	}
}

void GlWindow::renderView(CwlView *view)
{
	// Comprehensive validation before rendering
	if (!view) {
		qWarning() << "GlWindow::renderView: Null view provided";
		return;
	}

	if (!m_cwlcompositor) {
		qWarning() << "GlWindow::renderView: No compositor available";
		return;
	}

	// Validate OpenGL context
	QOpenGLContext *currentContext = QOpenGLContext::currentContext();
	if (!currentContext) {
		qWarning() << "GlWindow::renderView: No current OpenGL context";
		return;
	}

	// Get texture with comprehensive error handling
	QOpenGLTexture *texture = view->getTexture();
	if (!texture) {
		// getTexture() already logs appropriate warnings
		return;
	}

	// Validate texture before using it
	if (texture->textureId() == 0) {
		qWarning() << "GlWindow::renderView: Invalid texture ID (0)";
		return;
	}

	// Validate texture blitter
	if (!m_textureBlitter || !m_textureBlitter->isValid()) {
		qWarning() << "GlWindow::renderView: Invalid texture blitter";
		return;
	}

	try {
		// Safe texture target handling
		GLenum textureTarget = texture->target();
		if (textureTarget != GL_TEXTURE_2D) {
			qDebug()
				<< "GlWindow::renderView: Non-standard texture target:"
				<< textureTarget << "(continuing anyway)";
			// Continue anyway as Qt should handle different targets
		}

		if (textureTarget != m_currentTarget) {
			m_currentTarget = textureTarget;
			m_textureBlitter->bind(m_currentTarget);
		}

		// Validate surface before proceeding
		QWaylandSurface *surface = view->surface();
		if (!surface) {
			qWarning()
				<< "GlWindow::renderView: View has no surface";
			return;
		}

		if (!surface->hasContent()) {
			qDebug()
				<< "GlWindow::renderView: Surface has no content";
			return;
		}

		// Validate view dimensions
		QSize viewSize = view->size();
		if (viewSize.isEmpty() || !viewSize.isValid()) {
			qWarning() << "GlWindow::renderView: Invalid view size:"
				   << viewSize;
			return;
		}

		QPointF viewPosition = view->getPosition();
		auto surfaceOrigin = view->textureOrigin();

		// Validate scale factor
		int scaleFactor = m_cwlcompositor->scaleFactor();
		if (scaleFactor <= 0) {
			qWarning()
				<< "GlWindow::renderView: Invalid scale factor:"
				<< scaleFactor;
			scaleFactor = 1; // Safe fallback
		}

		QRectF targetRect(viewPosition * scaleFactor,
				  viewSize * scaleFactor);

		// Validate target rectangle
		if (targetRect.isEmpty() || !targetRect.isValid()) {
			qWarning()
				<< "GlWindow::renderView: Invalid target rectangle:"
				<< targetRect;
			return;
		}

		QMatrix4x4 targetTransform =
			QOpenGLTextureBlitter::targetTransform(
				targetRect, QRect(QPoint(), size()));

		// Perform the actual blit operation with error handling
		GLuint textureId = texture->textureId();
		if (textureId == 0) {
			qWarning()
				<< "GlWindow::renderView: Texture ID became invalid during rendering";
			return;
		}

		m_textureBlitter->blit(textureId, targetTransform,
				       surfaceOrigin);

	} catch (const std::exception &e) {
		qCritical()
			<< "GlWindow::renderView: Exception during rendering:"
			<< e.what();
		return;
	} catch (...) {
		qCritical()
			<< "GlWindow::renderView: Unknown exception during rendering";
		return;
	}

	// Recursively render child views with validation
	const QList<CwlView *> &childViews = view->getChildViews();
	if (!childViews.isEmpty()) {
		for (CwlView *childView : childViews) {
			if (childView) { // Validate each child view
				renderView(childView);
			} else {
				qWarning()
					<< "GlWindow::renderView: Null child view detected";
			}
		}
	}
}

void GlWindow::touchEvent(QTouchEvent *ev)
{
	if (!m_gesture || !m_cwlcompositor)
		return;
	m_gesture->handlePointerEvent(ev, [this](QList<QEventPoint> points) {
		m_cwlcompositor->handleTouchEvent(points);
	});
}

void GlWindow::mouseMoveEvent(QMouseEvent *ev)
{
	if (!m_gesture || !m_cwlcompositor)
		return;
	m_gesture->handlePointerEvent(ev, [this](QList<QEventPoint> points) {
		m_cwlcompositor->handleMouseMoveEvent(points);
	});
}

void GlWindow::mousePressEvent(QMouseEvent *ev)
{
	if (!m_gesture || !m_cwlcompositor)
		return;
	Qt::MouseButton btn = ev->button();
	m_gesture->handlePointerEvent(ev, [this,
					   btn](QList<QEventPoint> points) {
		m_cwlcompositor->handleMousePressEvent(points, btn);
	});
}

void GlWindow::mouseReleaseEvent(QMouseEvent *ev)
{
	if (!m_gesture || !m_cwlcompositor)
		return;
	Qt::MouseButton btn = ev->button();
	m_gesture->handlePointerEvent(ev, [this,
					   btn](QList<QEventPoint> points) {
		m_cwlcompositor->handleMouseReleaseEvent(points, btn);
	});
}

void GlWindow::keyPressEvent(QKeyEvent *event)
{
	if (!m_cwlcompositor)
		return;

	if (event->key() == Qt::Key_PowerOff)
		m_cwlcompositor->specialKey(
			CutieShell::SpecialKey::POWER_PRESS);

	if (event->key() == Qt::Key_VolumeUp)
		m_cwlcompositor->specialKey(
			CutieShell::SpecialKey::VOLUME_UP_PRESS);

	if (event->key() == Qt::Key_VolumeDown)
		m_cwlcompositor->specialKey(
			CutieShell::SpecialKey::VOLUME_DOWN_PRESS);
}

void GlWindow::keyReleaseEvent(QKeyEvent *event)
{
	if (!m_cwlcompositor)
		return;

	if (event->key() == Qt::Key_PowerOff)
		m_cwlcompositor->specialKey(
			CutieShell::SpecialKey::POWER_RELEASE);

	if (event->key() == Qt::Key_VolumeUp)
		m_cwlcompositor->specialKey(
			CutieShell::SpecialKey::VOLUME_UP_RELEASE);

	if (event->key() == Qt::Key_VolumeDown)
		m_cwlcompositor->specialKey(
			CutieShell::SpecialKey::VOLUME_DOWN_RELEASE);
}
