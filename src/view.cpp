#include <view.h>
#include <cutie-wlc.h>

CwlView::CwlView(CwlCompositor *cwlcompositor)
    : m_cwlcompositor(cwlcompositor)
{}

CwlView::~CwlView()
{
}

QOpenGLTexture *CwlView::getTexture()
{
    bool hasNewContent = advance();
    QWaylandBufferRef buffer = currentBuffer();

    if (!buffer.hasContent())
        m_texture = nullptr;

    if (hasNewContent) {
        m_texture = buffer.toOpenGLTexture();
        if (surface()) {
            m_size = surface()->destinationSize();

            if(buffer.origin() == QWaylandSurface::OriginTopLeft)
                m_origin = QOpenGLTextureBlitter::OriginTopLeft;
            else
                m_origin = QOpenGLTextureBlitter::OriginBottomLeft;
        }
    }

    // QImage image = currentBuffer().image();
    // image.save("/home/droidian/copy.jpeg", 0);

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
    if(m_xdgSurface != nullptr && m_xdgSurface->toplevel() != nullptr)
        return true;
    else
        return false;
}

QWaylandXdgToplevel *CwlView::getTopLevel()
{
    if(isToplevel())
        return m_xdgSurface->toplevel();
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
    if(layer == TOP){
        if(m_xdgSurface->toplevel() != nullptr){
            setPosition(geometry.topLeft());
            m_xdgSurface->toplevel()->sendMaximized(geometry.size());
        } else if(m_xdgSurface->popup() != nullptr){
            setPosition(m_xdgSurface->popup()->unconstrainedPosition() + geometry.topLeft());
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

QString CwlView::getAppId()
{
    if(!isToplevel())
        return "";

    return m_toplevel->appId();
}

QString CwlView::getTitle()
{
    if(!isToplevel())
        return "";

    return m_toplevel->title();
}