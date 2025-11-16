#pragma once

#include <QObject>
#include <QPointerEvent>

class CwlCompositor;

/**
 * @brief Abstract interface for gesture actions
 * 
 * This interface provides loose coupling between gesture detection and action execution,
 * making the system more modular and testable.
 */
class IGestureAction : public QObject {
	Q_OBJECT

    public:
	explicit IGestureAction(QObject *parent = nullptr)
		: QObject(parent)
	{
	}
	virtual ~IGestureAction() = default;

	/**
     * @brief Execute the gesture action
     * @param ev The pointer event triggering the gesture
     * @param compositor The compositor instance for state access
     * @return true if the gesture was handled, false otherwise
     */
	virtual bool execute(QPointerEvent *ev, CwlCompositor *compositor) = 0;

	/**
     * @brief Cancel the gesture action and revert any UI state changes
     * @param compositor The compositor instance for state access
     * 
     * This method is called when a gesture is preempted by another gesture
     * or cancelled by the system. Implementations should cleanly revert any
     * UI state changes made during execute() calls. The default implementation
     * does nothing, which is appropriate for gestures with no intermediate state.
     */
	virtual void cancel(CwlCompositor *compositor)
	{
		// Default implementation: no-op
		// Subclasses override if they need cleanup
	}

	/**
     * @brief Check if this action can handle the current gesture state
     * @param compositor The compositor instance for state checks
     * @return true if the action can be executed, false otherwise
     */
	virtual bool canExecute(CwlCompositor *compositor) const = 0;

	/**
     * @brief Get a human-readable name for this action (for debugging/logging)
     * @return The action name
     */
	virtual QString actionName() const = 0;
};

/**
 * @brief Base class for edge-based gesture actions
 * 
 * Provides common functionality for gestures that originate from screen edges.
 */
class EdgeGestureAction : public IGestureAction {
	Q_OBJECT

    public:
	explicit EdgeGestureAction(QObject *parent = nullptr)
		: IGestureAction(parent)
	{
	}

    protected:
	/**
     * @brief Handle the begin phase of an edge gesture
     * @param ev The pointer event
     * @param compositor The compositor instance
     * @return true if the gesture should continue, false otherwise
     */
	virtual bool handleBegin(QPointerEvent *ev,
				 CwlCompositor *compositor) = 0;

	/**
     * @brief Handle the update phase of an edge gesture
     * @param ev The pointer event
     * @param compositor The compositor instance
     * @return true if the gesture was handled, false otherwise
     */
	virtual bool handleUpdate(QPointerEvent *ev,
				  CwlCompositor *compositor) = 0;

	/**
     * @brief Handle the end phase of an edge gesture
     * @param ev The pointer event
     * @param compositor The compositor instance
     * @return true if the gesture was handled, false otherwise
     */
	virtual bool handleEnd(QPointerEvent *ev,
			       CwlCompositor *compositor) = 0;

    public:
	bool execute(QPointerEvent *ev,
		     CwlCompositor *compositor) override final
	{
		if (ev->isBeginEvent()) {
			return handleBegin(ev, compositor);
		} else if (ev->isUpdateEvent()) {
			return handleUpdate(ev, compositor);
		} else if (ev->isEndEvent()) {
			return handleEnd(ev, compositor);
		}
		return false;
	}
};

/**
 * @brief Base class for corner-based gesture actions
 * 
 * Provides common functionality for gestures that originate from screen corners.
 */
class CornerGestureAction : public IGestureAction {
	Q_OBJECT

    public:
	explicit CornerGestureAction(int corner, QObject *parent = nullptr)
		: IGestureAction(parent)
		, m_corner(corner)
	{
	}

    protected:
	int m_corner;

	/**
     * @brief Handle the begin/update phase of a corner gesture
     * @param ev The pointer event
     * @param compositor The compositor instance
     * @return true if the gesture was handled, false otherwise
     */
	virtual bool handleBeginUpdate(QPointerEvent *ev,
				       CwlCompositor *compositor) = 0;

	/**
     * @brief Handle the end phase of a corner gesture
     * @param ev The pointer event
     * @param compositor The compositor instance
     * @return true if the gesture was handled, false otherwise
     */
	virtual bool handleEnd(QPointerEvent *ev,
			       CwlCompositor *compositor) = 0;

    public:
	bool execute(QPointerEvent *ev,
		     CwlCompositor *compositor) override final
	{
		if (ev->isBeginEvent() || ev->isUpdateEvent()) {
			return handleBeginUpdate(ev, compositor);
		} else if (ev->isEndEvent()) {
			return handleEnd(ev, compositor);
		}
		return false;
	}
};

// Forward declarations for concrete action classes
class LeftEdgeGestureAction;
class RightEdgeGestureAction;
class TopEdgeGestureAction;
class BottomEdgeGestureAction;
class BottomCornerGestureAction;

/**
 * @brief Action for left edge gesture - typically opens launcher/home
 */
class LeftEdgeGestureAction : public EdgeGestureAction {
	Q_OBJECT

    public:
	explicit LeftEdgeGestureAction(QObject *parent = nullptr);
	bool canExecute(CwlCompositor *compositor) const override;
	QString actionName() const override;
	void cancel(CwlCompositor *compositor) override;

    protected:
	bool handleBegin(QPointerEvent *ev, CwlCompositor *compositor) override;
	bool handleUpdate(QPointerEvent *ev,
			  CwlCompositor *compositor) override;
	bool handleEnd(QPointerEvent *ev, CwlCompositor *compositor) override;
};

/**
 * @brief Action for right edge gesture - typically opens launcher/home from right
 */
class RightEdgeGestureAction : public EdgeGestureAction {
	Q_OBJECT

    public:
	explicit RightEdgeGestureAction(QObject *parent = nullptr);
	bool canExecute(CwlCompositor *compositor) const override;
	QString actionName() const override;
	void cancel(CwlCompositor *compositor) override;

    protected:
	bool handleBegin(QPointerEvent *ev, CwlCompositor *compositor) override;
	bool handleUpdate(QPointerEvent *ev,
			  CwlCompositor *compositor) override;
	bool handleEnd(QPointerEvent *ev, CwlCompositor *compositor) override;
};

/**
 * @brief Action for top edge gesture - typically handles launcher positioning
 */
class TopEdgeGestureAction : public EdgeGestureAction {
	Q_OBJECT

    public:
	explicit TopEdgeGestureAction(QObject *parent = nullptr);
	bool canExecute(CwlCompositor *compositor) const override;
	QString actionName() const override;
	void cancel(CwlCompositor *compositor) override;

    protected:
	bool handleBegin(QPointerEvent *ev, CwlCompositor *compositor) override;
	bool handleUpdate(QPointerEvent *ev,
			  CwlCompositor *compositor) override;
	bool handleEnd(QPointerEvent *ev, CwlCompositor *compositor) override;
};

/**
 * @brief Action for bottom edge gesture - typically handles launcher from bottom
 */
class BottomEdgeGestureAction : public EdgeGestureAction {
	Q_OBJECT

    public:
	explicit BottomEdgeGestureAction(QObject *parent = nullptr);
	bool canExecute(CwlCompositor *compositor) const override;
	QString actionName() const override;
	void cancel(CwlCompositor *compositor) override;

    protected:
	bool handleBegin(QPointerEvent *ev, CwlCompositor *compositor) override;
	bool handleUpdate(QPointerEvent *ev,
			  CwlCompositor *compositor) override;
	bool handleEnd(QPointerEvent *ev, CwlCompositor *compositor) override;
};

/**
 * @brief Action for bottom corner gestures - typically shows input method panel
 */
class BottomCornerGestureAction : public CornerGestureAction {
	Q_OBJECT

    public:
	explicit BottomCornerGestureAction(int corner,
					   QObject *parent = nullptr);
	bool canExecute(CwlCompositor *compositor) const override;
	QString actionName() const override;
	void cancel(CwlCompositor *compositor) override;

    protected:
	bool handleBeginUpdate(QPointerEvent *ev,
			       CwlCompositor *compositor) override;
	bool handleEnd(QPointerEvent *ev, CwlCompositor *compositor) override;
};