#ifndef CWL_COMPOSITOR_H
#define CWL_COMPOSITOR_H

#include <view.h>
#include <layer-shell.h>
#include <workspace.h>
#include <appswitcher.h>
#include <cutie-shell.h>
#include <QEventPoint>

QT_BEGIN_NAMESPACE

class GlWindow;
class CwlWorkspace;
class CwlAppswitcher;

class CwlCompositor : public QWaylandCompositor
{
    Q_OBJECT
public:
    CwlCompositor(GlWindow *glwindow);
    ~CwlCompositor() override;
    void create() override;

    QList<CwlView*> getViews() const;
    CwlView *viewAt(const QPoint &position);
    void raise(CwlView *cwlview);

    void handleTouchEvent(QTouchEvent *ev);
    void handleTouchPointEvent(QEventPoint *evP);

    void handleKeyPress(quint32 nativeScanCode);
    void handleKeyRelease(quint32 nativeScanCode);

    void startRender();
    void endRender();

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
    CwlWorkspace *m_workspace = nullptr;
    CwlAppswitcher *m_appswitcher = nullptr;
    QPointF *m_appPointStart = nullptr;
    CwlView *m_appView = nullptr;
    CutieShell *m_cutieshell = nullptr;
};

QT_END_NAMESPACE

#endif // CWL_COMPOSITOR_H