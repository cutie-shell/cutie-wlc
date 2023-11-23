#include <cutie-wlc.h>
#include <glwindow.h>
#include <screencopy.h>
#include <foreign-toplevel-management.h>
#include <input-method-v2.h>

#include <QtWaylandCompositor/QWaylandSeat>
#include <QWaylandPointer>
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
    m_cutieshell = new CutieShell(this);
    m_screencopyManager = new ScreencopyManagerV1(this);

    m_foreignTlManagerV1 = new ForeignToplevelManagerV1(this);
    connect(m_workspace, &CwlWorkspace::toplevelCreated, m_foreignTlManagerV1, &ForeignToplevelManagerV1::onToplevelCreated);
    connect(m_workspace, &CwlWorkspace::toplevelDestroyed, m_foreignTlManagerV1, &ForeignToplevelManagerV1::onToplevelDestroyed);

    initInputMethod();

    qputenv("CUTIE_SHELL", QByteArray("true"));
    qputenv("QT_QPA_PLATFORM", QByteArray("wayland"));
    qputenv("EGL_PLATFORM", QByteArray("wayland"));
    qunsetenv("QT_QPA_GENERIC_PLUGINS");
    qunsetenv("QT_SCALE_FACTOR");
    qputenv("WAYLAND_DISPLAY", socketName());

    QStringList args = QStringList();
    args.append("-c");
    args.append("cutie-home");
    if (!QProcess::startDetached("bash", args))
        qDebug() << "Failed to run";

    args = QStringList();
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
    args.append("cutie-keyboard");
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
        if (geom.contains(position / scaleFactor())){
            if(view->getChildViews().size()>0){
                for (CwlView *childView : view->getChildViews()) {
                    if(!childView->isToplevel())
                        continue;
                    QRectF geom(childView->getPosition(), childView->size() * scaleFactor());
                    if (geom.contains(position / scaleFactor())){
                        ret = childView;
                        return ret;
                    }
                }
            }
            ret = view;
            return ret;
        }
    }
    return ret;
}

CwlView* CwlCompositor::findView(QWaylandSurface *s)
{
    for (CwlView* view : getViews()) {
        if (view->surface() == s)
            return view;
    }
    return nullptr;
}

void CwlCompositor::onXdgToplevelCreated(QWaylandXdgToplevel *toplevel, QWaylandXdgSurface *xdgSurface)
{
    CwlView *view = new CwlView(this, m_workspace->availableGeometry());
    view->setOutput(outputFor(m_glwindow));
    view->setSurface(xdgSurface->surface());

    view->setTopLevel(toplevel);

    launcherClosed = true;
    launcherOpened = false;
    if(m_launcherView != nullptr)
        m_launcherView->setPosition(m_workspace->availableGeometry().bottomLeft());

    connect(m_workspace, &CwlWorkspace::availableGeometryChanged, view, &CwlView::onAvailableGeometryChanged);
    connect(view->surface(), &QWaylandSurface::redraw, this, &CwlCompositor::triggerRender);
    connect(view, &QWaylandView::surfaceDestroyed, this, &CwlCompositor::viewSurfaceDestroyed);
}

void CwlCompositor::initInputMethod()
{
    if(m_inputMngr != nullptr)
        delete m_inputMngr;

    m_inputMngr = new InputMethodManagerV2(this);
    connect(m_inputMngr, &InputMethodManagerV2::imDestroyed, this, &CwlCompositor::initInputMethod);
}

void CwlCompositor::onXdgPopupCreated(QWaylandXdgPopup *popup, QWaylandXdgSurface *xdgSurface)
{
    CwlView *view = new CwlView(this, m_workspace->availableGeometry());
    view->setOutput(outputFor(m_glwindow));
    view->setSurface(xdgSurface->surface());
    view->m_xdgPopup = popup;

    CwlView *parent_view = findView(popup->parentXdgSurface()->surface());

    qDebug()<<parent_view;

    view->setPosition(popup->unconstrainedPosition()+m_workspace->availableGeometry().topLeft());

    qDebug()<<"Popup "<<(uint64_t)view<<" created";

    view->layer = TOP;
    if (parent_view) {
        parent_view->addChildView(view);
        view->setParentView(parent_view);
        qDebug()<<"Popup "<<(uint64_t)view << " parented to "<<(uint64_t)parent_view;
        connect(view->surface(), &QWaylandSurface::redraw, this, &CwlCompositor::triggerRender);
        connect(view, &QWaylandView::surfaceDestroyed, this, &CwlCompositor::viewSurfaceDestroyed);
    } else m_workspace->addView(view);
}

void CwlCompositor::onLayerShellSurfaceCreated(LayerSurfaceV1 *layerSurface)
{
    CwlView *view = new CwlView(this, m_workspace->availableGeometry());
    view->setOutput(outputFor(m_glwindow));
    view->setSurface(layerSurface->surface);

    view->setLayerSurface(layerSurface);
    view->setPosition(QPoint(0, 0));
    layerSurface->send_configure(0, 0, 0);

    view->layer = (CwlViewLayer) layerSurface->ls_layer;
    m_workspace->addView(view);

    if(layerSurface->ls_scope == "cutie-home")
        m_homeView = view;

    if(layerSurface->ls_scope == "cutie-panel")
        m_panelView = view;
    connect(view->surface(), &QWaylandSurface::redraw, this, &CwlCompositor::triggerRender);
    connect(view, &QWaylandView::surfaceDestroyed, this, &CwlCompositor::viewSurfaceDestroyed);
    connect(view->m_layerSurface, &LayerSurfaceV1::layerSurfaceDataChanged, m_workspace, &CwlWorkspace::onLayerSurfaceDataChanged);
}

CwlView* CwlCompositor::getHomeView()
{
    return m_homeView;
}

CwlView* CwlCompositor::getTopPanel()
{
    return m_panelView;
}

void CwlCompositor::raise(CwlView *view)
{
    m_workspace->addView(view);

    if(view->isHidden())
        view->setHidden(false);

    defaultSeat()->setKeyboardFocus(view->surface());

    if (view == m_homeView) {
        m_workspace->showDesktop(true);
        homeOpen = 1.0;
    } else {
        homeOpen = 0.0;
    }

    triggerRender();
}

void CwlCompositor::handleTouchEvent(QTouchEvent *ev)
{
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
}

void CwlCompositor::handleMouseMoveEvent(QMouseEvent *ev)
{
    CwlView *view = m_launcherView;
    if (!launcherOpened) view = viewAt(ev->position().toPoint());
    if(view == nullptr)
        return;
    seatFor(ev)->sendMouseMoveEvent(
        view,
        ev->position().toPoint() / scaleFactor() - view->getPosition()
    );
}

void CwlCompositor::handleMousePressEvent(QMouseEvent *ev)
{
    handleMouseMoveEvent(ev);
    seatFor(ev)->sendMousePressEvent(ev->button());
}

void CwlCompositor::handleMouseReleaseEvent(QMouseEvent *ev)
{
    handleMouseMoveEvent(ev);
    seatFor(ev)->sendMouseReleaseEvent(ev->button());
}

bool CwlCompositor::handleGesture(QPointerEvent *ev, int edge, int corner)
{
    if(m_inputMngr->getInputMethod() != nullptr)
        m_inputMngr->getInputMethod()->hidePanel();
    
    if (edge == EDGE_LEFT) {
        if(ev->isBeginEvent()){
            return launcherClosed && (homeOpen != 1.0);
        }

        if(ev->isUpdateEvent()){
            homeOpen = 1.0 * ev->points().first().globalPosition().x() / m_glwindow->width();
            triggerRender();
            return true;
        }

        if(ev->isEndEvent()) { 
            if (ev->points().first().globalPosition().x() > GESTURE_ACCEPT_THRESHOLD) {
                raise(m_homeView);
                return true;
            } 
            homeOpen = 0.0;
            triggerRender();
            return true;
        }
    }
    
    if (edge == EDGE_RIGHT) {
        if(ev->isBeginEvent()){
            return launcherClosed && (homeOpen != 1.0);
        }

        if(ev->isUpdateEvent()){
            homeOpen = 1.0 - 1.0 * ev->points().first().globalPosition().x() / m_glwindow->width();
            triggerRender();
            return true;
        }

        if(ev->isEndEvent()) { 
            if (ev->points().first().globalPosition().x() < m_glwindow->width() - GESTURE_ACCEPT_THRESHOLD) {
                raise(m_homeView);
                return true;
            } 
            homeOpen = 0.0;
            triggerRender();
            return true;
        }
    }

    if(edge == EDGE_BOTTOM){
        if(ev->isBeginEvent() || ev->isUpdateEvent()){
            if(m_panelView != nullptr){
                if(m_panelView->panelState > 1){
                    return false;
                }
            }
            launcherClosed = false;
            QPointF newPos = m_launcherView->getPosition();
            newPos.setY(ev->points().first().globalPosition().y() / scaleFactor());
            m_launcherView->setPosition(newPos);
            triggerRender();
            return true;
        }

        if(ev->isEndEvent()){
            if(m_panelView != nullptr){
                if(m_panelView->panelState > 1){
                    return false;
                }
            }
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

void CwlCompositor::viewSurfaceDestroyed()
{
    CwlView *view = qobject_cast<CwlView*>(sender());

    if (view->parentView()){
        view->parentView()->removeChildView(view);
    } else if (view == m_launcherView){
        m_launcherView = nullptr;
    } else {
        m_workspace->removeView(view);
    }

    delete view;
    triggerRender();
}

void CwlCompositor::triggerRender()
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
