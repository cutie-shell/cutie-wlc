#ifndef GESTURE_MANAGER_H
#define GESTURE_MANAGER_H

#include <QObject>
#include <QPointerEvent>
#include <QHash>
#include <memory>

class CwlCompositor;
class IGestureAction;

/**
 * @brief The CwlGestureManager class manages gesture handling for the compositor
 * 
 * This class extracts gesture action logic from CwlCompositor to improve
 * code organization, maintainability, and extensibility. It uses action objects
 * for loose coupling and easier testing.
 */
class CwlGestureManager : public QObject {
	Q_OBJECT

    public:
	explicit CwlGestureManager(CwlCompositor *compositor,
				   QObject *parent = nullptr);
	~CwlGestureManager();

	// Main gesture handling interface
	bool handleGesture(QPointerEvent *ev, int edge, int corner);

	// Edge gesture handlers
	bool handleLeftEdgeGesture(QPointerEvent *ev);
	bool handleRightEdgeGesture(QPointerEvent *ev);
	bool handleTopEdgeGesture(QPointerEvent *ev);
	bool handleBottomEdgeGesture(QPointerEvent *ev);

	// Corner gesture handler
	bool handleCornerGesture(QPointerEvent *ev, int corner);

    private:
	void initializeActions();

	CwlCompositor *m_compositor;

	// Action objects for each gesture type
	std::unique_ptr<IGestureAction> m_leftEdgeAction;
	std::unique_ptr<IGestureAction> m_rightEdgeAction;
	std::unique_ptr<IGestureAction> m_topEdgeAction;
	std::unique_ptr<IGestureAction> m_bottomEdgeAction;
	QHash<int, IGestureAction *> m_cornerActions;
};

#endif // GESTURE_MANAGER_H