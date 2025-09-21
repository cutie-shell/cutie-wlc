#ifndef GESTURE_MANAGER_H
#define GESTURE_MANAGER_H

#include <QObject>
#include <QPointerEvent>

class CwlCompositor;

/**
 * @brief The CwlGestureManager class manages gesture handling for the compositor
 * 
 * This class extracts gesture action logic from CwlCompositor to improve
 * code organization, maintainability, and extensibility.
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
	CwlCompositor *m_compositor;
};

#endif // GESTURE_MANAGER_H