#include <view.h>
#include <cutie-wlc.h>
#include <QOpenGLTexture>

CwlView::CwlView(CwlCompositor *cwlcompositor, QRect geometry)
    : m_cwlcompositor(cwlcompositor)
{
    m_availableGeometry = geometry;
}

CwlView::~CwlView()
{
    if(m_isImageBuffer)
        delete m_texture;
}

QOpenGLTexture *CwlView::getTexture()
{
    if (advance()){
        if(currentBuffer().origin() == QWaylandSurface::OriginTopLeft)
            m_origin = QOpenGLTextureBlitter::OriginTopLeft;
        else
            m_origin = QOpenGLTextureBlitter::OriginBottomLeft;
        
        QImage img = currentBuffer().image();
        if(img.size().width() < 1){
            QOpenGLTexture *texture = currentBuffer().toOpenGLTexture();
            m_texture = texture;
        } else {
            if(!m_isImageBuffer)
                m_isImageBuffer = true;
            delete m_texture;
            m_texture = new QOpenGLTexture(img);
        }
    }
    return m_texture;
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
}

QSize CwlView::size()
{
    return surface() ? surface()->destinationSize() : m_size;
}

bool CwlView::isToplevel()
{
    return m_isTopLevel;
}

QWaylandXdgToplevel *CwlView::getTopLevel()
{
    return m_toplevel;
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
    if(layer == TOP){
        if(isToplevel()){
            setPosition(geometry.topLeft());
            m_toplevel->sendMaximized(geometry.size());
        } else if(m_toplevel->xdgSurface()->popup() != nullptr){
            setPosition(m_toplevel->xdgSurface()->popup()->unconstrainedPosition() + geometry.topLeft());
        }
    }
}

QList<CwlView*> CwlView::getChildViews() {
    return m_childViewList;
}

void CwlView::removeChildView(CwlView *view)
{
	m_childViewList.removeAll(view);
}

void CwlView::addChildView(CwlView *view)
{
	m_childViewList << view;
}

CwlView *CwlView::parentView() {
    return m_parentView;
}

void CwlView::setParentView(CwlView *view) {
    m_parentView = view;
}

void CwlView::setTopLevel(QWaylandXdgToplevel *toplevel)
{
    m_toplevel = toplevel;
    m_isTopLevel = true;
    connect(m_toplevel, &QWaylandXdgToplevel::appIdChanged, this, &CwlView::onAppIdChanged);
}

void CwlView::setAppId()
{
    if(m_isTopLevel)
        m_cwlAppId = m_toplevel->appId();
    else if(m_isLayerShell)
        m_cwlAppId = m_layerSurface->ls_scope;
}

QString CwlView::getAppId()
{
    return m_cwlAppId;
}

QString CwlView::getTitle()
{
    if(!m_isTopLevel)
        return "";

    return m_toplevel->title();
}

void CwlView::setLayerSurface(LayerSurfaceV1 *surface)
{
    if(surface != nullptr){
        m_layerSurface = surface;
        m_isLayerShell = true;
        setAppId();
        connect(m_layerSurface, &LayerSurfaceV1::layerSurfaceDataChanged, this, &CwlView::onLayerSurfaceDataChanged);
        connect(this->surface(), &QWaylandSurface::destinationSizeChanged, this, &CwlView::onDestinationSizeChanged);
    }
}

void CwlView::onAppIdChanged()
{
    if(m_toplevel->appId() != getAppId()){
        setAppId();
        if(getAppId() != "cutie-launcher"){
            layer = TOP;
            if(m_toplevel->parentToplevel() == nullptr){
                m_cwlcompositor->raise(this);
            } else {
                CwlView *parent_view = m_cwlcompositor->findView(m_toplevel->parentToplevel()->xdgSurface()->surface());
                parent_view->addChildView(this);
                this->setParentView(parent_view);
                connect(m_toplevel->xdgSurface(), &QWaylandXdgSurface::windowGeometryChanged, this, &CwlView::onWindowGeometryChanged);
            }
        } else if(getAppId() == "cutie-launcher"){
            layer = OVERLAY;
            m_cwlcompositor->m_launcherView = this;
            m_cwlcompositor->m_launcherView->setPosition(m_availableGeometry.bottomLeft());
        }
    }
}

void CwlView::onWindowGeometryChanged()
{
    if(parentView()){
        m_cwlcompositor->deactivateAppSwitcher();
        QRect currentGeometry = m_toplevel->xdgSurface()->windowGeometry();
        currentGeometry.moveCenter(m_availableGeometry.center());
        this->setPosition(currentGeometry.topLeft());
    }
}

void CwlView::onLayerSurfaceDataChanged(LayerSurfaceV1 *surface)
{
    if(surface != m_layerSurface)
        return;

    if(m_layerSurface->ls_scope == "cutie-panel" && m_layerSurface->size.height() <= m_layerSurface->ls_zone)
        panelState = PANEL_FOLDED;

    if(CwlViewAnchor::ANCHOR_TOP == m_layerSurface->ls_anchor){
        this->setPosition(m_availableGeometry.topLeft());
    } else if(CwlViewAnchor::ANCHOR_BOTTOM == m_layerSurface->ls_anchor){
        this->setPosition(QPointF(m_availableGeometry.bottomLeft().x(),
                            m_availableGeometry.bottomLeft().y() - m_layerSurface->size.height()));
    }
}

void CwlView::onDestinationSizeChanged()
{
    if(m_layerSurface != nullptr && m_layerSurface->ls_scope == "cutie-panel"){
        if(this->surface()->destinationSize().height() <= m_layerSurface->ls_zone){
            panelState = PANEL_FOLDED;
        } else if (this->surface()->destinationSize().height() > m_layerSurface->ls_zone){
            panelState = PANEL_UNFOLDING;
        }
    }
}