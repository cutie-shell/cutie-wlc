#ifndef CWL_COMPOSITOR_H
#define CWL_COMPOSITOR_H

#include <view.h>
#include <layer-shell.h>
#include <workspace.h>
#include <cutie-shell.h>
#include <QEventPoint>
#include <QProcess>
#include <QPropertyAnimation>
#include <QWaylandXdgDecorationManagerV1>

QT_BEGIN_NAMESPACE

class GlWindow;
class CwlWorkspace;
class CutieShell;
class OutputManagerV1;
class OutputPowerManagerV1;
class ScreencopyManagerV1;
class ForeignToplevelManagerV1;
class ForeignToplevelHandleV1;
class InputMethodManagerV2;

class CwlCompositor : public QWaylandCompositor
{
    Q_OBJECT
    Q_PROPERTY(double blur READ blur WRITE setBlur NOTIFY blurChanged)
    Q_PROPERTY(double launcherPostion READ launcherPostion WRITE setLauncherPosition NOTIFY launcherPostionChanged)
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

    double blur();
    void setBlur(double blur);

    double launcherPostion();
    void setLauncherPosition(double position);

signals:
    void scaleFactorChanged(int scaleFactor);
    void blurChanged(double blur);
    void launcherPostionChanged(double launcherPostion);

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
    OutputManagerV1 *m_outputManager = nullptr;
    OutputPowerManagerV1 *m_outputPowerManager = nullptr;
    ScreencopyManagerV1 *m_screencopyManager = nullptr;
    ForeignToplevelManagerV1 *m_foreignTlManagerV1 = nullptr;
    QWaylandOutput *m_output = nullptr;
    QWaylandXdgDecorationManagerV1 *m_xdgdecoration = nullptr;
    InputMethodManagerV2 *m_inputMngr = nullptr;
    CwlView *m_homeView = nullptr;
    CwlView *m_panelView = nullptr;

    QPropertyAnimation *blurAnim = new QPropertyAnimation(this, "blur", this);
    QPropertyAnimation *unblurAnim = new QPropertyAnimation(this, "blur", this);

    QPropertyAnimation *launcherCloseAnim = new QPropertyAnimation(this, "launcherPostion", this);
    QPropertyAnimation *launcherOpenAnim = new QPropertyAnimation(this, "launcherPostion", this);

    int m_scaleFactor = 1;
    double m_blur = 0.0;
    double m_launcherPosition = 1.0;
    bool m_homeOpen = true;
    QString launcher = "cutie-launcher";
};

QT_END_NAMESPACE

#endif // CWL_COMPOSITOR_H
