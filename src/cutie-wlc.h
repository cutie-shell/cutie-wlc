#ifndef CWL_COMPOSITOR_H
#define CWL_COMPOSITOR_H

#include <view.h>
#include <layer-shell.h>
#include <workspace.h>
#include <appswitcher.h>
#include <cutie-shell.h>
#include <gesture.h>
#include <QEventPoint>
#include <QProcess>
#include <QWaylandXdgDecorationManagerV1>

QT_BEGIN_NAMESPACE

class GlWindow;
class CwlWorkspace;
class CwlAppswitcher;
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
    void raise(CwlView *cwlview);

    void handleTouchEvent(QTouchEvent *ev);
    void handleTouchPointEvent(QEventPoint *evP);

    bool handleGesture(QTouchEvent *ev, int edge, int corner);

    void handleKeyPress(quint32 nativeScanCode);
    void handleKeyRelease(quint32 nativeScanCode);

    void startRender();
    void endRender();

    int scaleFactor();
    void setScaleFactor(int scale);

    GlWindow *glWindow();

    bool launcherClosed = true;
    bool launcherOpened = false;
    CwlView *m_launcherView = nullptr;
    CwlWorkspace *m_workspace = nullptr;

public slots:
    void appSwitcherAnimate();
    void triggerRender();

private slots:
    void surfaceHasContentChanged();
    void onSurfaceCreated(QWaylandSurface *surface);
    void onXdgSurfaceCreated(QWaylandXdgSurface *xdgSurface);
    void onXdgToplevelCreated(QWaylandXdgToplevel *toplevel, QWaylandXdgSurface *xdgSurface);
    void onXdgPopupCreated(QWaylandXdgPopup *popup, QWaylandXdgSurface *xdgSurface);
    void onLayerShellSurfaceCreated(LayerSurfaceV1 *layerSurface);
    void onSurfaceDestroyed();
    void surfaceDestroyed();
    void viewSurfaceDestroyed();
    void onTlAppIdChanged();
    
private:
    CwlView *findView(const QWaylandSurface *s) const;
    GlWindow *m_glwindow = nullptr;
    QWaylandXdgShell *m_xdgShell = nullptr;
    QPointer<CwlView> m_mouseView;
    LayerShellV1 *m_layerShell = nullptr;
    CwlAppswitcher *m_appswitcher = nullptr;
    QPointF *m_appPointStart = nullptr;
    CwlView *m_appView = nullptr;
    CutieShell *m_cutieshell = nullptr;
    ScreencopyManagerV1 *m_screencopyManager = nullptr;
    ForeignToplevelManagerV1 *m_foreignTlManagerV1 = nullptr;
    QWaylandOutput *m_output = nullptr;
    QWaylandXdgDecorationManagerV1 *m_xdgdecoration = nullptr;
    InputMethodManagerV2 *m_inputmethod = nullptr;
    
    int m_scaleFactor = 1;
    QString launcher = "cutie-launcher";
};

QT_END_NAMESPACE

#endif // CWL_COMPOSITOR_H