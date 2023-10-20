#include <cutie-wlc.h>
#include <glwindow.h>
#include <screencopy.h>
#include <foreign-toplevel-management.h>
#include <input-method-v2.h>

#include <QtWaylandCompositor/QWaylandSeat>
#include <QWaylandTouch>
#include <QTouchEvent>

CwlCompositor::CwlCompositor(GlWindow *glwindow)
    : m_glwindow(glwindow)
    , m_xdgShell(new QWaylandXdgShell(this))
    , m_layerShell(new LayerShellV1(this))
    , m_xdgdecoration(new QWaylandXdgDecorationManagerV1())
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
    m_output = new QWaylandOutput(this, m_glwindow);
    QWaylandOutputMode mode(m_glwindow->size(), 60000);
    m_output->addMode(mode, true);
    QWaylandCompositor::create();
    m_output->setCurrentMode(mode);
    m_output->setScaleFactor(m_scaleFactor);

    m_xdgdecoration->setExtensionContainer(this);
    m_xdgdecoration->initialize();
    m_xdgdecoration->setPreferredMode(QWaylandXdgToplevel::ServerSideDecoration);

    m_workspace = new CwlWorkspace(this);
    m_appswitcher = new CwlAppswitcher(m_workspace);
    m_cutieshell = new CutieShell(this);
    m_screencopyManager = new ScreencopyManagerV1(this);

    m_inputmethod = new InputMethodManagerV2(this);

    m_foreignTlManagerV1 = new ForeignToplevelManagerV1(this);
    connect(m_workspace, &CwlWorkspace::toplevelCreated, m_foreignTlManagerV1, &ForeignToplevelManagerV1::onToplevelCreated);
    connect(m_workspace, &CwlWorkspace::toplevelDestroyed, m_foreignTlManagerV1, &ForeignToplevelManagerV1::onToplevelDestroyed);

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
    args.append("cutie-panel");
    if (!QProcess::startDetached("bash", args))
        qDebug() << "Failed to run";

    args = QStringList();
    args.append("-c");
    args.append("cutie-home");
    if (!QProcess::startDetached("bash", args))
        qDebug() << "Failed to run";
}

QList<CwlView*> CwlCompositor::getViews() const
{
    return m_workspace->getViews();
}

QList<CwlView*> CwlCompositor::getToplevelViews()
{
    return m_workspace->getToplevelViews();
}

CwlView *CwlCompositor::viewAt(const QPoint &position)
{
    CwlView *ret = nullptr;
    for (auto it = getViews().crbegin(); it != getViews().crend(); ++it) {
        CwlView *view = *it;
        QRectF geom(view->getPosition(), view->size() * scaleFactor());
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
    qDebug()<<"New TopLevel with AppId: "<<tl->appId();

    CwlView* v;

    for (CwlView* view : getViews()) {
        if (view->m_toplevel == tl)
            v = view;
    }

    if(v->getAppId() != "cutie-launcher"){
        v->layer = TOP;
        m_workspace->addView(v);

        defaultSeat()->setKeyboardFocus(v->surface());
    } else if(v->getAppId() == "cutie-launcher"){
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

    view->setLayerSurface(layerSurface);
    view->setPosition(QPoint(0, 0));
    layerSurface->send_configure(0, 0, 0);

    view->layer = (CwlViewLayer) layerSurface->ls_layer;
    m_workspace->addView(view);

    connect(view->m_layerSurface, &LayerSurfaceV1::layerSurfaceDataChanged, m_workspace, &CwlWorkspace::onLayerSurfaceDataChanged);
}

void CwlCompositor::raise(CwlView *view)
{
    m_workspace->addView(view);

    if(view->isHidden())
        view->setHidden(false);

    defaultSeat()->setKeyboardFocus(view->surface());
    triggerRender();
}

void CwlCompositor::handleTouchEvent(QTouchEvent *ev)
{
    if(!m_appswitcher->isActive()){
        CwlView *view = m_launcherView;
        if (!launcherOpened) view = viewAt(ev->points().first().globalPosition().toPoint());
        if(view == nullptr)
            return;
        foreach (QEventPoint point, ev->points()) {
            seatFor(ev)->touch()->sendTouchPointEvent(
                view->surface(),
                point.id(),
                point.position() / scaleFactor() - view->getPosition(),
                (Qt::TouchPointState) point.state()
            );
        }
        seatFor(ev)->touch()->sendFrameEvent(view->surface()->client()); 
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

bool CwlCompositor::handleGesture(QTouchEvent *ev, int edge, int corner)
{
    if (edge == EDGE_RIGHT) {
        if(ev->isBeginEvent() || ev->isUpdateEvent()){
            return !m_appswitcher->isActive() && launcherClosed;
        }

        if(ev->isEndEvent() && 
            ev->points().first().globalPosition().x() < m_glwindow->width() * 0.8){
            m_appswitcher->activate();
            triggerRender();
            return true;
        }
    }

    if(edge == EDGE_BOTTOM){
        if(ev->isBeginEvent() || ev->isUpdateEvent()){
            launcherClosed = false;
            QPointF newPos = m_launcherView->getPosition();
            newPos.setY(ev->points().first().globalPosition().y() / scaleFactor());
            m_launcherView->setPosition(newPos);
            triggerRender();
            return true;
        }

        if(ev->isEndEvent()){
            if(ev->points().first().globalPosition().y() < m_glwindow->height() * 0.8){
                m_launcherView->setPosition(m_workspace->availableGeometry().topLeft());
                launcherOpened = true;
                triggerRender();
            } else {
                launcherClosed = true;
                launcherOpened = false;
                m_launcherView->setPosition(m_workspace->availableGeometry().bottomLeft());
                triggerRender();
            }
            return true;
        }
    }

    if(edge == EDGE_TOP){
        if(!launcherClosed){
            if(ev->isBeginEvent() || ev->isUpdateEvent()){
                launcherOpened = false;
                QPointF newPos = m_launcherView->getPosition();
                newPos.setY(ev->points().first().globalPosition().y());
                m_launcherView->setPosition(newPos);
                triggerRender();
            }

            if(ev->isEndEvent()){
                if(ev->points().first().globalPosition().y() < m_glwindow->height() * 0.2){
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

            return true;
        }
    }
    
    return false;
}

void CwlCompositor::onSurfaceCreated(QWaylandSurface *surface)
{
    connect(surface, &QWaylandSurface::surfaceDestroyed, this, &CwlCompositor::surfaceDestroyed);
    connect(surface, &QWaylandSurface::hasContentChanged, this, &CwlCompositor::surfaceHasContentChanged);
    connect(surface, &QWaylandSurface::redraw, this, &CwlCompositor::triggerRender);

    CwlView *view = new CwlView(this, m_workspace->availableGeometry());
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
    else if (view == m_launcherView) m_launcherView = nullptr;
    else m_workspace->removeView(view);
    
    delete view;
    m_appswitcher->update();
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

int CwlCompositor::scaleFactor() {
    return m_scaleFactor;
}

void CwlCompositor::setScaleFactor(int scale) {
    m_scaleFactor = scale;
    if (m_output)
        m_output->setScaleFactor(m_scaleFactor);
}

GlWindow *CwlCompositor::glWindow() {
    return m_glwindow;
}
