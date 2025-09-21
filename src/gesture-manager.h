#ifndef GESTURE_MANAGER_H
#define GESTURE_MANAGER_H

#include <QObject>
#include <QPointerEvent>
#include <QHash>
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

    private:
	void initializeActions();
	void initializeGestureMapping();
	GestureType convertEdgeToGestureType(int edge) const;
	GestureKey createGestureKey(GestureType type,
				    int cornerPosition = -1) const;
	IGestureAction *findActionForGesture(QPointerEvent *ev, int edge,
					     int corner) const;

	CwlCompositor *m_compositor;

	// Legacy action objects (for backward compatibility during transition)
	std::unique_ptr<IGestureAction> m_leftEdgeAction;
	std::unique_ptr<IGestureAction> m_rightEdgeAction;
	std::unique_ptr<IGestureAction> m_topEdgeAction;
	std::unique_ptr<IGestureAction> m_bottomEdgeAction;
	QHash<int, IGestureAction *> m_cornerActions;

	// New gesture mapping registry
	QHash<GestureKey, IGestureAction *> m_gestureRegistry;
};

#endif // GESTURE_MANAGER_H