#include <cutie-wlc.h>
#include <glwindow.h>

#include <QtWaylandCompositor/QWaylandSeat>
#include <QWaylandTouch>
#include <QTouchEvent>

CwlCompositor::CwlCompositor(GlWindow *glwindow)
    : m_glwindow(glwindow)
    , m_xdgShell(new QWaylandXdgShell(this))
    , m_layerShell(new LayerShellV1(this))
{
    m_glwindow->setCompositor(this);
    connect(m_glwindow, &GlWindow::glReady, this, &CwlCompositor::create);

    connect(m_xdgShell, &QWaylandXdgShell::xdgSurfaceCreated, this, &CwlCompositor::onXdgSurfaceCreated);
    connect(m_xdgShell, &QWaylandXdgShell::toplevelCreated, this, &CwlCompositor::onXdgToplevelCreated);
    connect(m_xdgShell, &QWaylandXdgShell::popupCreated, this, &CwlCompositor::onXdgPopupCreated);

    connect(m_layerShell, &LayerShellV1::layerShellSurfaceCreated, this, &CwlCompositor::onLayerShellSurfaceCreated);
}

CwlCompositor::~CwlCompositor()
{
}

void CwlCompositor::create()
{
    QWaylandOutput *output = new QWaylandOutput(this, m_glwindow);
    QWaylandOutputMode mode(m_glwindow->size(), 60000);
    output->addMode(mode, true);
    QWaylandCompositor::create();
    output->setCurrentMode(mode);

    m_workspace = new CwlWorkspace(this);
    m_appswitcher = new CwlAppswitcher(m_workspace);
    m_cutieshell = new CutieShell(this);
    
    m_glwindow->setAppswitcher(m_appswitcher);

    connect(this, &QWaylandCompositor::surfaceCreated, this, &CwlCompositor::onSurfaceCreated);

    qputenv("CUTIE_SHELL", QByteArray("true"));
    qputenv("QT_QPA_PLATFORM", QByteArray("wayland"));
    qputenv("EGL_PLATFORM", QByteArray("wayland"));
    qunsetenv("QT_QPA_GENERIC_PLUGINS");
    qunsetenv("QT_SCALE_FACTOR");
    qputenv("WAYLAND_DISPLAY", socketName());

    QStringList args = QStringList();
    args.append("-c");
    args.append(launcher);
    if (!QProcess::startDetached("bash", args))
        qDebug() << "Failed to run";

    args = QStringList();
    args.append("-c");
    args.append("examples/cutie-panel/cutie-panel");
    if (!QProcess::startDetached("bash", args))
        qDebug() << "Failed to run";

    args = QStringList();
    args.append("-c");
    args.append("examples/cutie-shell/cutie-ui-io");
    if (!QProcess::startDetached("bash", args))
        qDebug() << "Failed to run";
}

QList<CwlView*> CwlCompositor::getViews() const
{
    return m_workspace->getViews();
}

CwlView *CwlCompositor::viewAt(const QPoint &position)
{
    CwlView *ret = nullptr;
    for (auto it = getViews().crbegin(); it != getViews().crend(); ++it) {
        CwlView *view = *it;
        QRectF geom(view->getPosition(), view->size());
        if (geom.contains(position)){
            ret = view;
            return ret;
        }
    }
    return ret;
}

CwlView *CwlCompositor::findView(const QWaylandSurface *s) const
{
    for (CwlView* view : getViews()) {
        if (view->surface() == s)
            return view;
    }
    return nullptr;
}

void CwlCompositor::onXdgSurfaceCreated(QWaylandXdgSurface *xdgSurface)
{
    CwlView *view = findView(xdgSurface->surface());
    Q_ASSERT(view);
    view->m_xdgSurface = xdgSurface;

    triggerRender();
}

void CwlCompositor::onXdgToplevelCreated(QWaylandXdgToplevel *toplevel, QWaylandXdgSurface *xdgSurface)
{
    CwlView *view = findView(xdgSurface->surface());
    Q_ASSERT(view);
    
    toplevel->sendMaximized(m_workspace->availableGeometry().size());

    view->setPosition(m_workspace->availableGeometry().topLeft());

    view->m_toplevel = toplevel;

    connect(view->m_toplevel, &QWaylandXdgToplevel::appIdChanged, this, &CwlCompositor::onTlAppIdChanged);

    launcherClosed = true;
    launcherOpened = false;
    if(m_launcherView != nullptr)
        m_launcherView->setPosition(m_workspace->availableGeometry().bottomLeft());
}

void CwlCompositor::onTlAppIdChanged()
{
    QWaylandXdgToplevel *tl = qobject_cast<QWaylandXdgToplevel*>(sender());
    qDebug()<<"TopLevel with AppId: "<<tl->appId();

    CwlView* v;

    for (CwlView* view : getViews()) {
        if (view->m_toplevel == tl)
            v = view;
    }

    m_workspace->removeView(v);

    if(v->m_toplevel->appId() != "cutie-launcher"){
        v->layer = TOP;
        m_workspace->addView(v);

        defaultSeat()->setKeyboardFocus(v->surface());
    } else if(v->m_toplevel->appId() == "cutie-launcher"){
        m_launcherView = v;
        m_launcherView->setPosition(m_workspace->availableGeometry().bottomLeft());
    }
}

void CwlCompositor::onXdgPopupCreated(QWaylandXdgPopup *popup, QWaylandXdgSurface *xdgSurface)
{
    CwlView *view = findView(xdgSurface->surface());
    Q_ASSERT(view);

    CwlView *parent_view = findView(popup->parentXdgSurface()->surface());

    view->setPosition(popup->unconstrainedPosition()+m_workspace->availableGeometry().topLeft());

    qDebug()<<"Popup "<<(uint64_t)view<<" created";

    m_workspace->removeView(view);
    view->layer = TOP;
    if (parent_view) {
        parent_view->addChildView(view);
        view->setParentView(parent_view);
        qDebug()<<"Popup "<<(uint64_t)view << " parented to "<<(uint64_t)parent_view;
    } else m_workspace->addView(view);
}

void CwlCompositor::onLayerShellSurfaceCreated(LayerSurfaceV1 *layerSurface)
{
    CwlView *view = findView(layerSurface->surface);
    Q_ASSERT(view);

    view->m_layerSurface = layerSurface;
    view->setPosition(QPoint(0, 0));
    layerSurface->send_configure(0, 0, 0);

    m_workspace->removeView(view);
    view->layer = (CwlViewLayer) layerSurface->ls_layer;
    m_workspace->addView(view);

    connect(view->m_layerSurface, &LayerSurfaceV1::layerSurfaceDataChanged, m_workspace, &CwlWorkspace::onLayerSurfaceDataChanged);
}

void CwlCompositor::raise(CwlView *view)
{
    m_workspace->removeView(view);
    m_workspace->addView(view);

    if(view->isHidden())
        view->setHidden(false);

    defaultSeat()->setKeyboardFocus(view->surface());
    triggerRender();
}

void CwlCompositor::handleTouchEvent(QTouchEvent *ev)
{
    if(!m_appswitcher->isActive()){
        if(launcherOpened){
            defaultSeat()->sendFullTouchEvent(m_launcherView->surface(), ev);
        } else {
            CwlView *view = viewAt(ev->points().first().globalPosition().toPoint());
            if(view == nullptr)
                return;
            foreach (QEventPoint point, ev->points()) {
                seatFor(ev)->touch()->sendTouchPointEvent(
                    view->surface(),
                    point.id(),
                    point.position() - view->getPosition(),
                    (Qt::TouchPointState) point.state()
                );
            }
            seatFor(ev)->touch()->sendFrameEvent(view->surface()->client());
        }       
    } else {
        if(ev->points().first().state() == QEventPoint::Pressed){
            CwlView *view  = m_appswitcher->findViewAt(ev->points().first().globalPosition());
            if(view != nullptr){
                m_appView = view;
                m_appPointStart = new QPointF(ev->points().first().globalPosition());
            }
        } else if(ev->points().first().state() == QEventPoint::Released){
            CwlView *view = m_appswitcher->findViewAt(ev->points().first().globalPosition());
            if(view != nullptr){
                if(view == m_appView && m_appPointStart->y() - ev->points().first().globalPosition().y() > 150){

                    if(view->m_xdgSurface->toplevel() != nullptr){
                        view->m_xdgSurface->toplevel()->sendClose();
                        m_appView = nullptr;
                        m_appPointStart = nullptr;
                    }
                } else if(view == m_appView){
                    raise(view);
                    m_appView = nullptr;
                    m_appPointStart = nullptr;
                    m_appswitcher->deactivate();
                }
            } else {
                m_workspace->hideAllTopLevel();
                m_appswitcher->deactivate();
            }
        }
    }
}

void CwlCompositor::handleGesture(QTouchEvent *ev, int edge, int corner)
{
    qDebug()<<"EDGE: "<<edge;

    if(ev->isEndEvent() && edge == EDGE_RIGHT && 
            ev->points().first().globalPosition().x() < m_workspace->availableGeometry().size().width() * 0.8 && 
            !m_appswitcher->isActive()){
        if(launcherOpened){
            launcherClosed = true;
            launcherOpened = false;
            m_launcherView->setPosition(m_workspace->availableGeometry().bottomLeft());
            triggerRender();
        }
        m_appswitcher->activate();
        triggerRender();
    }

    if(edge == EDGE_BOTTOM){
        if(ev->isBeginEvent() || ev->isUpdateEvent()){
            launcherClosed = false;
            QPointF newPos = m_launcherView->getPosition();
            newPos.setY(ev->points().first().globalPosition().y());
            m_launcherView->setPosition(newPos);
            triggerRender();
        }

        if(ev->isEndEvent()){
            if(ev->points().first().globalPosition().y() < m_workspace->availableGeometry().size().height() * 0.8){
                m_launcherView->setPosition(m_workspace->availableGeometry().topLeft());
                launcherOpened = true;
                triggerRender();
            } else {
                launcherClosed = true;
                launcherOpened = false;
                m_launcherView->setPosition(m_workspace->availableGeometry().bottomLeft());
                triggerRender();
            }
        }
    }
}

void CwlCompositor::onSurfaceCreated(QWaylandSurface *surface)
{
    connect(surface, &QWaylandSurface::surfaceDestroyed, this, &CwlCompositor::surfaceDestroyed);
    connect(surface, &QWaylandSurface::hasContentChanged, this, &CwlCompositor::surfaceHasContentChanged);
    connect(surface, &QWaylandSurface::redraw, this, &CwlCompositor::triggerRender);

    CwlView *view = new CwlView(this);
    view->setSurface(surface);
    view->setOutput(outputFor(m_glwindow));

    m_workspace->addView(view);

    connect(view, &QWaylandView::surfaceDestroyed, this, &CwlCompositor::viewSurfaceDestroyed);
    connect(m_workspace, &CwlWorkspace::availableGeometryChanged, view, &CwlView::onAvailableGeometryChanged);
}

void CwlCompositor::surfaceDestroyed()
{
    triggerRender();
}

void CwlCompositor::surfaceHasContentChanged()
{
    triggerRender();
}

void CwlCompositor::onSurfaceDestroyed()
{
    triggerRender();
}

void CwlCompositor::viewSurfaceDestroyed()
{
    CwlView *view = qobject_cast<CwlView*>(sender());

    if (view->parentView()) view->parentView()->removeChildView(view);
    else m_workspace->removeView(view);
    delete view;
    //m_appswitcher->updateViewMap();
    triggerRender();
}

void CwlCompositor::triggerRender()
{
    if(!m_appswitcher->animationRunning)
        m_glwindow->requestUpdate();
}

void CwlCompositor::appSwitcherAnimate()
{
    m_glwindow->requestUpdate();
}

void CwlCompositor::startRender()
{
    QWaylandOutput *out = defaultOutput();
    if (out)
        out->frameStarted();
}

void CwlCompositor::endRender()
{
    QWaylandOutput *out = defaultOutput();
    if (out)
        out->sendFrameCallbacks();
}