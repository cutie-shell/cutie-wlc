#include <glwindow.h>

#include <QPainter>
#include <QMatrix4x4>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QMouseEvent>
#include <QGuiApplication>
#include <qpa/qplatformnativeinterface.h>
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
	if (m_gesture)
		delete m_gesture;
	m_gesture = new CwlGesture(cwlcompositor, QSize(width(), height()));
}

bool GlWindow::displayOff()
{
	return m_displayOff;
}

void GlWindow::setDisplayOff(bool displayOff)
{
	if (m_displayOff == displayOff)
		return;
	if (m_displayOff) {
		QGuiApplication::platformNativeInterface()
			->nativeResourceForIntegration("displayon");
		requestUpdate();
	} else {
		QGuiApplication::platformNativeInterface()
			->nativeResourceForIntegration("displayoff");
		m_cwlcompositor->setLauncherPosition(0.0);
	}
	m_displayOff = displayOff;
	emit displayOffChanged(m_displayOff);
}

void GlWindow::initializeGL()
{
	m_textureBlitter.create();
	emit glReady();
}

void GlWindow::paintGL()
{
	if (m_displayOff)
		return;
	m_cwlcompositor->startRender();

	QOpenGLFunctions *functions = context()->functions();
	functions->glClearColor(.0f, .0f, .0f, 1.f);
	functions->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_currentTarget = GL_TEXTURE_2D;
	m_textureBlitter.bind(m_currentTarget);
	functions->glEnable(GL_BLEND);
	functions->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	QList<CwlView *> renderViews;
	if (m_cwlcompositor->m_launcherView)
		renderViews = m_cwlcompositor->getViews()
			      << m_cwlcompositor->m_launcherView;
	else
		renderViews = m_cwlcompositor->getViews();

	for (CwlView *view : renderViews) {
		QString appId;
		if (view && view->isToplevel())
			appId = view->getAppId();

		if (appId == "cutie-launcher")
			m_textureBlitter.setOpacity(
				1.0 -
				(m_cwlcompositor->m_launcherView->getPosition()
					 .y() *
				 m_cwlcompositor->scaleFactor() / height()));
		else if (view->isToplevel())
			if (m_cwlcompositor->launcherPostion() > 0.0)
				m_textureBlitter.setOpacity(
					m_cwlcompositor->blur() *
					m_cwlcompositor->m_launcherView
						->getPosition()
						.y() *
					m_cwlcompositor->scaleFactor() /
					height());
			else
				m_textureBlitter.setOpacity(
					m_cwlcompositor->blur());
		else
			m_textureBlitter.setOpacity(1.0);

		if (m_cwlcompositor->launcherPostion() == 1.0 &&
		    view->layer == CwlViewLayer::TOP)
			continue;

		renderView(view);
		for (CwlView *childView : view->getChildViews())
			renderView(childView);
	}

	m_textureBlitter.release();
	m_cwlcompositor->endRender();
}

void GlWindow::renderView(CwlView *view)
{
	QOpenGLTexture *texture = view->getTexture();
	if (!texture)
		return;
	if (texture->target() != m_currentTarget) {
		m_currentTarget = texture->target();
		m_textureBlitter.bind(m_currentTarget);
	}

	QWaylandSurface *surface = view->surface();
	if (surface && surface->hasContent()) {
		QSize viewSize = view->size();
		QPointF viewPosition = view->getPosition();
		auto surfaceOrigin = view->textureOrigin();
		QRectF targetRect(viewPosition * m_cwlcompositor->scaleFactor(),
				  viewSize * m_cwlcompositor->scaleFactor());

		QMatrix4x4 targetTransform =
			QOpenGLTextureBlitter::targetTransform(
				targetRect, QRect(QPoint(), size()));
		m_textureBlitter.blit(texture->textureId(), targetTransform,
				      surfaceOrigin);
	}
}

void GlWindow::touchEvent(QTouchEvent *ev)
{
	m_gesture->handlePointerEvent(ev, [this](QList<QEventPoint> points) {
		m_cwlcompositor->handleTouchEvent(points);
	});
}

void GlWindow::mouseMoveEvent(QMouseEvent *ev)
{
	m_gesture->handlePointerEvent(ev, [this](QList<QEventPoint> points) {
		m_cwlcompositor->handleMouseMoveEvent(points);
	});
}

void GlWindow::mousePressEvent(QMouseEvent *ev)
{
	Qt::MouseButton btn = ev->button();
	m_gesture->handlePointerEvent(ev, [this,
					   btn](QList<QEventPoint> points) {
		m_cwlcompositor->handleMousePressEvent(points, btn);
	});
}

void GlWindow::mouseReleaseEvent(QMouseEvent *ev)
{
	Qt::MouseButton btn = ev->button();
	m_gesture->handlePointerEvent(ev, [this,
					   btn](QList<QEventPoint> points) {
		m_cwlcompositor->handleMouseReleaseEvent(points, btn);
	});
}

void GlWindow::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_PowerOff)
		m_cwlcompositor->specialKey(
			CutieShell::SpecialKey::POWER_PRESS);
}

void GlWindow::keyReleaseEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_PowerOff)
		m_cwlcompositor->specialKey(
			CutieShell::SpecialKey::POWER_RELEASE);
}
