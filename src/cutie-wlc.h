#ifndef CWL_COMPOSITOR_H
#define CWL_COMPOSITOR_H

#include <view.h>
#include <layer-shell.h>
#include <workspace.h>
#include <cutie-shell.h>
#include <gesture.h>
#include <QEventPoint>
#include <QProcess>
#include <QWaylandXdgDecorationManagerV1>

QT_BEGIN_NAMESPACE

class GlWindow;
class CwlWorkspace;
class ScreencopyManagerV1;
class ForeignToplevelManagerV1;
class ForeignToplevelHandleV1;
class InputMethodManagerV2;

class CwlCompositor : public QWaylandCompositor
{
    Q_OBJECT
public:
    CwlCompositor(GlWindow *glwindow);
    ~CwlCompositor() override;
    void create() override;

    QList<CwlView*> getViews() const;
    QList<CwlView*> getToplevelViews();
    CwlView *viewAt(const QPoint &position);
    CwlView* findView(QWaylandSurface *s);
    void raise(CwlView *cwlview);

    void handleTouchEvent(QTouchEvent *ev);
    void handleTouchPointEvent(QEventPoint *evP);

    void handleMouseMoveEvent(QMouseEvent *ev);
    void handleMousePressEvent(QMouseEvent *ev);
    void handleMouseReleaseEvent(QMouseEvent *ev);

    bool handleGesture(QPointerEvent *ev, int edge, int corner);

    void handleKeyPress(quint32 nativeScanCode);
    void handleKeyRelease(quint32 nativeScanCode);

    void startRender();
    void endRender();

    int scaleFactor();
    void setScaleFactor(int scale);

    CwlView *getTopPanel();
    GlWindow *glWindow();

    bool launcherClosed = true;
    bool launcherOpened = false;
    CwlView *m_launcherView = nullptr;
    CwlWorkspace *m_workspace = nullptr;

    CwlView* getHomeView();

public slots:
    void triggerRender();

private slots:
    void onXdgToplevelCreated(QWaylandXdgToplevel *toplevel, QWaylandXdgSurface *xdgSurface);
    void onXdgPopupCreated(QWaylandXdgPopup *popup, QWaylandXdgSurface *xdgSurface);
    void onLayerShellSurfaceCreated(LayerSurfaceV1 *layerSurface);
    void viewSurfaceDestroyed();
    void initInputMethod();
    
private:
    CwlView *findView(const QWaylandSurface *s) const;
    GlWindow *m_glwindow = nullptr;
    QWaylandXdgShell *m_xdgShell = nullptr;
    QPointer<CwlView> m_mouseView;
    LayerShellV1 *m_layerShell = nullptr;
    QPointF *m_appPointStart = nullptr;
    CwlView *m_appView = nullptr;
    CutieShell *m_cutieshell = nullptr;
    ScreencopyManagerV1 *m_screencopyManager = nullptr;
    ForeignToplevelManagerV1 *m_foreignTlManagerV1 = nullptr;
    QWaylandOutput *m_output = nullptr;
    QWaylandXdgDecorationManagerV1 *m_xdgdecoration = nullptr;
    InputMethodManagerV2 *m_inputMngr = nullptr;
    CwlView *m_homeView = nullptr;
    CwlView *m_panelView = nullptr;

    int m_scaleFactor = 1;
    QString launcher = "cutie-launcher";
};

QT_END_NAMESPACE

#endif // CWL_COMPOSITOR_H
