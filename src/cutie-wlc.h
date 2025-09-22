#ifndef CWL_COMPOSITOR_H
#define CWL_COMPOSITOR_H

#include <view.h>
#include <layer-shell.h>
#include <workspace.h>
#include <cutie-shell.h>
#include <QEventPoint>
#include <QProcess>
#include <QWaylandXdgDecorationManagerV1>
#include <QScopedPointer>
#include <memory>

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
class CwlProcessManager;
class CwlGestureManager;
class CwlAnimationController;

class CwlCompositor : public QWaylandCompositor {
	Q_OBJECT
	Q_PROPERTY(double blur READ blur WRITE setBlur NOTIFY blurChanged)
	Q_PROPERTY(double launcherPosition READ launcherPosition WRITE
			   setLauncherPosition NOTIFY launcherPositionChanged)
    public:
	CwlCompositor(GlWindow *glwindow);
	~CwlCompositor() override;
	void create() override;

	QList<CwlView *> getViews() const;
	QList<CwlView *> getToplevelViews();
	CwlView *viewAt(const QPoint &position);
	CwlView *findView(QWaylandSurface *s);
	CwlView *findTlView(QWaylandSurface *s);
	CwlView *findTreeView(QWaylandSurface *s, CwlView *rootView);

	void raise(CwlView *cwlview);

	void handleTouchEvent(QList<QEventPoint> points);
	void handleMouseMoveEvent(QList<QEventPoint> points);
	void handleMousePressEvent(QList<QEventPoint> points,
				   Qt::MouseButton btn);
	void handleMouseReleaseEvent(QList<QEventPoint> points,
				     Qt::MouseButton btn);

	void handleKeyPress(quint32 nativeScanCode);
	void handleKeyRelease(quint32 nativeScanCode);

	void startRender();
	void endRender();

	int scaleFactor();
	void setScaleFactor(int scale);

	CwlView *getTopPanel();
	GlWindow *glWindow();
	ForeignToplevelManagerV1 *foreignTlManagerV1();
	CwlGestureManager *gestureManager();
	CwlAnimationController *animationController();

	CwlView *m_launcherView = nullptr;
	CwlWorkspace *m_workspace = nullptr;

	CwlView *getHomeView();

	double blur();
	void setBlur(double blur);

	double launcherPosition();
	void setLauncherPosition(double position);

	// Animation control methods for gesture manager
	void startBlurAnimation();
	void startUnblurAnimation();
	void startLauncherOpenAnimation();
	void startLauncherCloseAnimation();

	// Home state management
	bool isHomeOpen() const;
	void setHomeOpen(bool open);

	// Panel and input method access
	CwlView *getPanelView() const;
	InputMethodManagerV2 *getInputMethodManager() const;

	void grabSurface(QWaylandSurfaceGrabber *grabber,
			 const QWaylandBufferRef &buffer) override;

    signals:
	void scaleFactorChanged(int scaleFactor);
	void blurChanged(double blur);
	void launcherPositionChanged(double launcherPosition);
	void specialKey(CutieShell::SpecialKey key);

    public slots:
	void triggerRender();
	void onToplevelDamaged(CwlView *view);
	void onHideKeyboard();

    private slots:
	void onXdgToplevelCreated(QWaylandXdgToplevel *toplevel,
				  QWaylandXdgSurface *xdgSurface);
	void onXdgPopupCreated(QWaylandXdgPopup *popup,
			       QWaylandXdgSurface *xdgSurface);
	void onLayerShellSurfaceCreated(LayerSurfaceV1 *layerSurface);
	void viewSurfaceDestroyed();
	void initInputMethod();

    private:
	CwlView *findView(const QWaylandSurface *s) const;
	void setupEnvironmentVariables();
	void setupSignalConnections();
	void setupWorkspaceConnections();

	GlWindow *m_glwindow = nullptr;
	QScopedPointer<QWaylandXdgShell> m_xdgShell;
	QPointer<CwlView> m_mouseView;
	QScopedPointer<LayerShellV1> m_layerShell;
	std::unique_ptr<QPointF> m_appPointStart;
	CwlView *m_appView = nullptr;
	std::unique_ptr<CutieShell> m_cutieshell;
	std::unique_ptr<OutputManagerV1> m_outputManager;
	std::unique_ptr<OutputPowerManagerV1> m_outputPowerManager;
	std::unique_ptr<ScreencopyManagerV1> m_screencopyManager;
	std::unique_ptr<ForeignToplevelManagerV1> m_foreignTlManagerV1;
	QScopedPointer<QWaylandOutput> m_output;
	QScopedPointer<QWaylandXdgDecorationManagerV1> m_xdgdecoration;
	std::unique_ptr<InputMethodManagerV2> m_inputMngr;
	CwlView *m_homeView = nullptr;
	CwlView *m_panelView = nullptr;

	int m_scaleFactor = 1;
	double m_blur = 0.0;
	double m_launcherPosition = 1.0;
	bool m_homeOpen = true;
	QString launcher = "cutie-launcher";

	// Process management
	std::unique_ptr<CwlProcessManager> m_processManager;

	// Gesture management
	std::unique_ptr<CwlGestureManager> m_gestureManager;

	// Animation management
	std::unique_ptr<CwlAnimationController> m_animationController;
};

QT_END_NAMESPACE

#endif // CWL_COMPOSITOR_H
