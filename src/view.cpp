#include <view.h>
#include <cutie-wlc.h>
#include <opengl/opengl-guards.h>
#include <QOpenGLTexture>
#include <QOpenGLContext>
#include <QtWaylandCompositor/QWaylandSeat>
#include <stdexcept>
#include <QDebug>

CwlView::CwlView(CwlCompositor *cwlcompositor, QRect geometry)
	: m_cwlcompositor(cwlcompositor)
	, m_availableGeometry(geometry)
{
	connect(this, &CwlView::surfaceChanged, this,
		&CwlView::onSurfaceChanged);
}

CwlView::~CwlView()
{
	m_imageTexture.reset();
}

QOpenGLTexture *CwlView::getTexture()
{
	// Validate OpenGL context first
	QOpenGLContext *currentContext = QOpenGLContext::currentContext();
	if (!currentContext) {
		qWarning() << "CwlView::getTexture: No current OpenGL context";
		return nullptr;
	}

	// Use RAII for automatic OpenGL state management during texture operations
	OpenGLStateGuard stateGuard;

	// Validate surface before processing
	if (!surface()) {
		qWarning() << "CwlView::getTexture: No surface available";
		return nullptr;
	}

	if (advance()) {
		QWaylandBufferRef bufRef = currentBuffer();

		// Validate buffer reference
		if (!bufRef.hasBuffer()) {
			qDebug() << "CwlView::getTexture: No buffer available";
			return nullptr;
		}

		// Set texture origin based on buffer
		if (bufRef.origin() == QWaylandSurface::OriginTopLeft)
			m_origin = QOpenGLTextureBlitter::OriginTopLeft;
		else
			m_origin = QOpenGLTextureBlitter::OriginBottomLeft;

		try {
			if (bufRef.bufferType() ==
			    QWaylandBufferRef::BufferType::BufferType_Egl) {
				// Handle EGL texture creation with error checking
				m_eglTexture = bufRef.toOpenGLTexture();
				if (!m_eglTexture) {
					qWarning()
						<< "CwlView::getTexture: Failed to create EGL texture";
					return nullptr;
				}
			} else if (bufRef.bufferType() ==
				   QWaylandBufferRef::BufferType::
					   BufferType_SharedMemory) {
				// Handle shared memory texture creation with validation
				QImage image = bufRef.image();
				if (image.isNull()) {
					qWarning()
						<< "CwlView::getTexture: Invalid image from shared memory buffer";
					return nullptr;
				}

				m_isImageBuffer = true;
				m_imageTexture.reset(new QOpenGLTexture(image));

				// Validate texture creation
				if (!m_imageTexture ||
				    !m_imageTexture->isCreated()) {
					qWarning()
						<< "CwlView::getTexture: Failed to create texture from image";
					m_imageTexture.reset();
					m_isImageBuffer = false;
					return nullptr;
				}
			} else {
				qWarning()
					<< "CwlView::getTexture: Unsupported buffer type:"
					<< bufRef.bufferType();
				return nullptr;
			}
		} catch (const std::exception &e) {
			qCritical()
				<< "CwlView::getTexture: Exception during texture creation:"
				<< e.what();
			return nullptr;
		} catch (...) {
			qCritical()
				<< "CwlView::getTexture: Unknown exception during texture creation";
			return nullptr;
		}
	}

	// Return appropriate texture with validation
	if (m_isImageBuffer) {
		if (m_imageTexture && m_imageTexture->isCreated()) {
			return m_imageTexture.data();
		} else {
			qWarning()
				<< "CwlView::getTexture: Image texture not valid";
			return nullptr;
		}
	} else {
		if (m_eglTexture) {
			// Additional validation for EGL textures
			if (m_eglTexture->textureId() == 0) {
				qWarning()
					<< "CwlView::getTexture: EGL texture has invalid ID";
				return nullptr;
			}
			return m_eglTexture;
		} else {
			qDebug()
				<< "CwlView::getTexture: No EGL texture available";
			return nullptr;
		}
	}
}

QOpenGLTextureBlitter::Origin CwlView::textureOrigin()
{
	return m_origin;
}

QPointF CwlView::getPosition()
{
	return m_position;
}

void CwlView::setPosition(const QPointF &pos)
{
	m_position = pos;
	m_cwlcompositor->triggerRender();
}

QSize CwlView::size()
{
	return surface() ? surface()->destinationSize() : m_size;
}

bool CwlView::isToplevel()
{
	return m_isTopLevel;
}

bool CwlView::isPopup()
{
	return m_isPopup;
}

QWaylandXdgToplevel *CwlView::getTopLevel()
{
	return m_toplevel;
}

QWaylandXdgPopup *CwlView::getPopup()
{
	return m_xdgPopup;
}

LayerSurfaceV1 *CwlView::getLayerSurface()
{
	return m_layerSurface;
}

bool CwlView::isHidden()
{
	return m_hidden;
}

void CwlView::setHidden(bool hide)
{
	m_hidden = hide;
}

void CwlView::onAvailableGeometryChanged(QRect geometry)
{
	m_availableGeometry = geometry;
	if (layer == TOP) {
		if (isToplevel()) {
			setPosition(geometry.topLeft());
			m_toplevel->sendMaximized(geometry.size());
		} else if (m_toplevel->xdgSurface()->popup()) {
			setPosition(m_toplevel->xdgSurface()
					    ->popup()
					    ->unconstrainedPosition() +
				    geometry.topLeft());
		}
	}
}

QList<CwlView *> CwlView::getChildViews()
{
	return m_childViewList;
}

void CwlView::removeChildView(CwlView *view)
{
	m_childViewList.removeAll(view);
	emit m_cwlcompositor->m_workspace->toplevelDestroyed(view);
}

void CwlView::addChildView(CwlView *view)
{
	m_childViewList << view;
}

CwlView *CwlView::parentView()
{
	return m_parentView.data();
}

void CwlView::setParentView(CwlView *view)
{
	m_parentView = view;
}

void CwlView::setTopLevel(QWaylandXdgToplevel *toplevel)
{
	m_toplevel = toplevel;
	m_isTopLevel = true;
	connect(m_toplevel, &QWaylandXdgToplevel::appIdChanged, this,
		&CwlView::onAppIdChanged);
}

void CwlView::setPopUp(QWaylandXdgPopup *popup)
{
	m_xdgPopup = popup;
	m_isPopup = true;
}

void CwlView::setAppId()
{
	if (m_isTopLevel)
		m_cwlAppId = m_toplevel->appId();
	else if (m_isLayerShell)
		m_cwlAppId = m_layerSurface->ls_scope;
}

QString CwlView::getAppId()
{
	return m_cwlAppId;
}

QString CwlView::getTitle()
{
	if (!m_isTopLevel)
		return "";
	return m_toplevel->title();
}

void CwlView::setLayerSurface(LayerSurfaceV1 *surface)
{
	if (!surface)
		return;
	m_layerSurface = surface;
	m_isLayerShell = true;
	setAppId();
	connect(m_layerSurface, &LayerSurfaceV1::layerSurfaceDataChanged, this,
		&CwlView::onLayerSurfaceDataChanged);
	connect(this->surface(), &QWaylandSurface::destinationSizeChanged, this,
		&CwlView::onDestinationSizeChanged);
}

void CwlView::onAppIdChanged()
{
	if (m_toplevel->appId() == getAppId())
		return;
	setAppId();
	if (getAppId() == "cutie-launcher") {
		layer = OVERLAY;
		m_cwlcompositor->m_launcherView = this;
		m_toplevel->sendMaximized(m_availableGeometry.size());
		m_cwlcompositor->setLauncherPosition(0.0);
	} else {
		layer = TOP;
		if (m_toplevel->parentToplevel()) {
			CwlView *parent_view = m_cwlcompositor->findTlView(
				m_toplevel->parentToplevel()
					->xdgSurface()
					->surface());
			parent_view->addChildView(this);
			this->setParentView(parent_view);
			m_toplevel->sendMaximized(m_availableGeometry.size());
			this->setPosition(m_availableGeometry.topLeft());
			connect(m_toplevel->xdgSurface(),
				&QWaylandXdgSurface::windowGeometryChanged,
				this, &CwlView::onWindowGeometryChanged);
			m_cwlcompositor->raise(parent_view);
			emit m_cwlcompositor->m_workspace->toplevelCreated(
				this);
			m_cwlcompositor->defaultSeat()->setKeyboardFocus(
				this->surface());
		} else {
			m_cwlcompositor->m_workspace->addView(this);
			emit m_cwlcompositor->m_workspace->toplevelCreated(
				this);
			m_toplevel->sendMaximized(m_availableGeometry.size());
			this->setPosition(m_availableGeometry.topLeft());
			m_cwlcompositor->raise(this);
		}
	}
}

void CwlView::onWindowGeometryChanged()
{
	if (parentView()) {
		QRect parentGeometry = parentView()
					       ->getTopLevel()
					       ->xdgSurface()
					       ->windowGeometry();
		QRect currentGeometry =
			m_toplevel->xdgSurface()->windowGeometry();
		if (currentGeometry.size().height() <=
			    m_availableGeometry.size().height() ||
		    currentGeometry.size().width() <=
			    m_availableGeometry.size().width()) {
			currentGeometry.moveCenter(
				m_availableGeometry.center());
		}
		this->setPosition(currentGeometry.topLeft());
	}
}

void CwlView::onLayerSurfaceDataChanged(LayerSurfaceV1 *surface)
{
	if (surface != m_layerSurface)
		return;

	// Handle panel-specific state
	if (m_layerSurface->ls_scope == "cutie-panel" &&
	    m_layerSurface->size.height() <= m_layerSurface->ls_zone)
		panelState = PANEL_FOLDED;

	// Calculate position using helper methods
	QPointF position = calculateInitialPosition();
	calculateVerticalPosition(position);
	calculateHorizontalPosition(position);

	this->setPosition(position);
}

QPointF CwlView::calculateInitialPosition() const
{
	// Start with centered position as default
	return QPointF(m_availableGeometry.center().x() -
			       m_layerSurface->size.width() / 2,
		       m_availableGeometry.center().y() -
			       m_layerSurface->size.height() / 2);
}

void CwlView::calculateVerticalPosition(QPointF &position) const
{
	if (CwlViewAnchor::ANCHOR_TOP & m_layerSurface->ls_anchor &&
	    !(CwlViewAnchor::ANCHOR_BOTTOM & m_layerSurface->ls_anchor)) {
		// Top anchor only
		position.setY(m_layerSurface->ls_zone < 0 ?
				      0 :
				      m_availableGeometry.top());
	} else if (CwlViewAnchor::ANCHOR_BOTTOM & m_layerSurface->ls_anchor &&
		   !(CwlViewAnchor::ANCHOR_TOP & m_layerSurface->ls_anchor)) {
		// Bottom anchor only
		position.setY(
			(m_layerSurface->ls_zone < 0 ?
				 m_cwlcompositor->m_workspace->outputGeometry()
					 .bottom() :
				 m_availableGeometry.bottom()) -
			m_layerSurface->ls_zone);
	} else if (m_layerSurface->ls_zone < 0) {
		// Center vertically when no specific anchor or conflicting anchors
		position.setY(m_cwlcompositor->m_workspace->outputGeometry()
				      .center()
				      .y() -
			      m_layerSurface->size.height() / 2);
	}
}

void CwlView::calculateHorizontalPosition(QPointF &position) const
{
	if (CwlViewAnchor::ANCHOR_LEFT & m_layerSurface->ls_anchor &&
	    !(CwlViewAnchor::ANCHOR_RIGHT & m_layerSurface->ls_anchor)) {
		// Left anchor only
		position.setX(m_layerSurface->ls_zone < 0 ?
				      0 :
				      m_availableGeometry.left());
	} else if (CwlViewAnchor::ANCHOR_RIGHT & m_layerSurface->ls_anchor &&
		   !(CwlViewAnchor::ANCHOR_LEFT & m_layerSurface->ls_anchor)) {
		// Right anchor only
		position.setX(
			(m_layerSurface->ls_zone < 0 ?
				 m_cwlcompositor->m_workspace->outputGeometry()
					 .right() :
				 m_availableGeometry.right()) -
			m_layerSurface->size.width());
	} else if (m_layerSurface->ls_zone < 0) {
		// Center horizontally when no specific anchor or conflicting anchors
		position.setX(m_cwlcompositor->m_workspace->outputGeometry()
				      .center()
				      .x() -
			      m_layerSurface->size.width() / 2);
	}
}

void CwlView::onDestinationSizeChanged()
{
	if (m_layerSurface && m_layerSurface->ls_scope == "cutie-panel") {
		if (this->surface()->destinationSize().height() <=
		    m_layerSurface->ls_zone) {
			panelState = PANEL_FOLDED;
		} else if (this->surface()->destinationSize().height() >
			   m_layerSurface->ls_zone) {
			panelState = PANEL_UNFOLDING;
		}
	}
}

void CwlView::onRedraw()
{
	m_cwlcompositor->triggerRender();
	if (isToplevel())
		;
	m_cwlcompositor->onToplevelDamaged(this);
}

QWaylandSurfaceGrabber *CwlView::grabber()
{
	return m_grabber.data();
}

void CwlView::onSurfaceChanged()
{
	m_grabber.reset(new QWaylandSurfaceGrabber(surface()));
}

void CwlView::onPopUpGeometryChanged()
{
	if (!m_availableGeometry.contains(m_xdgPopup->configuredGeometry())) {
		QRect intersected =
			m_xdgPopup->configuredGeometry().intersected(
				m_availableGeometry);
		if (intersected.topLeft() ==
		    m_xdgPopup->configuredGeometry().topLeft()) {
			QRect newRect = m_xdgPopup->configuredGeometry();
			newRect.translate(
				intersected.size().width() -
					m_xdgPopup->configuredGeometry()
						.size()
						.width(),
				intersected.size().height() -
					m_xdgPopup->configuredGeometry()
						.size()
						.height());
			this->setPosition(newRect.topLeft());
		}
	}
}