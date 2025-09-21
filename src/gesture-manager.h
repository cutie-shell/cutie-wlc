#ifndef GESTURE_MANAGER_H
#define GESTURE_MANAGER_H

#include <QObject>
#include <QPointerEvent>
#include <QHash>
#include <QTimer>
#include <memory>

class CwlCompositor;
class IGestureAction;

/**
 * @brief Standardized gesture type enumeration for mapping system
 * 
 * This enum provides a unified way to identify all gesture types,
 * including edge gestures and corner gestures with their positions.
 */
enum class GestureType : uint32_t {
	LEFT_EDGE = 0,
	RIGHT_EDGE = 1,
	TOP_EDGE = 2,
	BOTTOM_EDGE = 3,
	CORNER_TL = 4, // Top-left corner
	CORNER_TR = 5, // Top-right corner
	CORNER_BL = 6, // Bottom-left corner
	CORNER_BR = 7, // Bottom-right corner
	UNDEFINED = 8
};

/**
 * @brief Gesture state enumeration for tracking gesture lifecycle
 * 
 * This enum tracks the current state of active gestures for better
 * state management and conflict resolution.
 */
enum class GestureState : uint32_t {
	IDLE = 0, // No gesture activity
	DETECTING = 1, // Gesture detection in progress
	ACTIVE = 2, // Gesture confirmed and executing
	CANCELLING = 3, // Gesture being cancelled
	COMPLETED = 4, // Gesture completed successfully
	FAILED = 5 // Gesture failed or was rejected
};

/**
 * @brief Gesture mapping key combining type and optional corner position
 * 
 * For edge gestures, only type is used. For corner gestures, both
 * type and cornerPosition are used for precise identification.
 */
struct GestureKey {
	GestureType type;
	int cornerPosition =
		-1; // Used for corner gestures, -1 for edge gestures

	bool operator==(const GestureKey &other) const
	{
		return type == other.type &&
		       cornerPosition == other.cornerPosition;
	}
};

/**
 * @brief Active gesture tracking structure
 * 
 * Tracks the state and properties of an active gesture for
 * state management and conflict resolution.
 */
struct ActiveGesture {
	GestureKey key;
	GestureState state;
	IGestureAction *action;
	QPointerEvent *lastEvent; // Store copy for cancellation
	qint64 startTime; // Timestamp when gesture started
	int priority; // Gesture priority for conflict resolution

	ActiveGesture()
		: action(nullptr)
		, lastEvent(nullptr)
		, startTime(0)
		, priority(0)
	{
	}
	ActiveGesture(const GestureKey &k, IGestureAction *a, int prio = 0)
		: key(k)
		, state(GestureState::DETECTING)
		, action(a)
		, lastEvent(nullptr)
		, startTime(0)
		, priority(prio)
	{
	}
};

// Hash function for GestureKey to use in QHash
inline uint qHash(const GestureKey &key, uint seed = 0)
{
	return qHash(static_cast<uint32_t>(key.type), seed) ^
	       qHash(key.cornerPosition, seed);
}

/**
 * @brief The CwlGestureManager class manages gesture handling for the compositor
 * 
 * This class extracts gesture action logic from CwlCompositor to improve
 * code organization, maintainability, and extensibility. It uses action objects
 * for loose coupling and easier testing. The new mapping system allows for
 * dynamic configuration of gesture-to-action associations.
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

	// Gesture mapping management
	void setGestureAction(GestureType type, IGestureAction *action);
	void setCornerGestureAction(int cornerPosition, IGestureAction *action);
	IGestureAction *getGestureAction(GestureType type) const;
	IGestureAction *getCornerGestureAction(int cornerPosition) const;

	// Runtime configuration support
	void remapGesture(GestureType fromType, GestureType toType);
	void remapCornerGesture(int fromCorner, int toCorner);
	void clearGestureMapping(GestureType type);
	void clearCornerGestureMapping(int cornerPosition);

	// Gesture state management
	bool startGesture(const GestureKey &key, QPointerEvent *ev,
			  IGestureAction *action, int priority = 0);
	void updateGestureState(const GestureKey &key, GestureState newState);
	void completeGesture(const GestureKey &key);
	void cancelGesture(const GestureKey &key);
	void cancelAllGestures();
	bool isGestureActive(const GestureKey &key) const;
	GestureState getGestureState(const GestureKey &key) const;
	QList<ActiveGesture> getActiveGestures() const;

	// Gesture conflict resolution
	bool hasConflictingGestures(const GestureKey &key) const;
	void resolveGestureConflicts(const GestureKey &newGesture,
				     int priority);
	bool canStartGesture(const GestureKey &key, int priority) const;

    private:
	void initializeActions();
	void initializeGestureMapping();
	GestureType convertEdgeToGestureType(int edge) const;
	GestureKey createGestureKey(GestureType type,
				    int cornerPosition = -1) const;
	IGestureAction *findActionForGesture(QPointerEvent *ev, int edge,
					     int corner) const;

	// State management helper methods
	void cleanupExpiredGestures();
	void onGestureTimeout();
	bool isConflictingGesture(const GestureKey &existing,
				  const GestureKey &newGesture) const;

	CwlCompositor *m_compositor;

	// Legacy action objects (for backward compatibility during transition)
	std::unique_ptr<IGestureAction> m_leftEdgeAction;
	std::unique_ptr<IGestureAction> m_rightEdgeAction;
	std::unique_ptr<IGestureAction> m_topEdgeAction;
	std::unique_ptr<IGestureAction> m_bottomEdgeAction;
	QHash<int, IGestureAction *> m_cornerActions;

	// New gesture mapping registry
	QHash<GestureKey, IGestureAction *> m_gestureRegistry;

	// Gesture state management
	QHash<GestureKey, ActiveGesture> m_activeGestures;
	QTimer *m_gestureTimeout;
	static constexpr int GESTURE_TIMEOUT_MS =
		5000; // 5 second timeout for gestures
};

#endif // GESTURE_MANAGER_H