#pragma once

#include <cutie-wlc.h>
#include <QTouchEvent>
#include <QStack>

/**
 * @file gesture.h
 * @brief Low-level gesture detection for the Cutie compositor
 * 
 * This file contains the gesture detection logic that identifies edge and corner
 * gestures from pointer events. It works in coordination with CwlGestureManager
 * to provide a complete gesture handling system.
 */

// Minimum offset to consider the movement a gesture (either accepted or canceled)
#define GESTURE_MINIMUM_THRESHOLD 10
// Minimum offset to consider a gesture accepted.
#define GESTURE_ACCEPT_THRESHOLD 200
#define GESTURE_EDGE_OFFSET 20
#define GESTURE_CORNER_OFFSET 50

/**
 * @brief Scaled gesture edge offset based on compositor scale factor
 * 
 * This macro provides dynamic scaling of edge detection areas based on
 * the current display scale factor for consistent gesture feel across
 * different screen densities.
 */
#define SCALED_GESTURE_EDGE_OFFSET \
	(GESTURE_EDGE_OFFSET * m_cwlcompositor->scaleFactor())

/**
 * @brief Scaled gesture corner offset based on compositor scale factor
 * 
 * This macro provides dynamic scaling of corner detection areas based on
 * the current display scale factor for consistent gesture feel across
 * different screen densities.
 */
#define SCALED_GESTURE_CORNER_OFFSET \
	(GESTURE_CORNER_OFFSET * m_cwlcompositor->scaleFactor())

/**
 * @brief Edge swipe direction enumeration
 * 
 * Defines the possible edge directions for swipe gestures.
 * Used for identifying which screen edge initiated the gesture.
 */
enum EdgeSwipe : uint32_t {
	EDGE_RIGHT = 0, ///< Swipe from right edge
	EDGE_LEFT = 1, ///< Swipe from left edge
	EDGE_TOP = 2, ///< Swipe from top edge
	EDGE_BOTTOM = 3, ///< Swipe from bottom edge
	EDGE_UNDEFINED = 4 ///< No edge detected or invalid edge
};

/**
 * @brief Corner swipe direction enumeration
 * 
 * Defines the possible corner positions for swipe gestures.
 * Used for identifying which screen corner initiated the gesture.
 */
enum CornerSwipe : uint32_t {
	CORNER_TL = 0, ///< Top-left corner
	CORNER_TR = 1, ///< Top-right corner
	CORNER_BL = 2, ///< Bottom-left corner
	CORNER_BR = 3, ///< Bottom-right corner
	CORNER_UNDEFINED = 4 ///< No corner detected or invalid corner
};

/**
 * @brief Low-level gesture detection class
 * 
 * CwlGesture provides low-level gesture detection for edge and corner swipes.
 * It analyzes pointer events to determine if they represent valid gestures
 * and coordinates with CwlGestureManager for action execution.
 * 
 * The class maintains detection areas for edges and corners, tracks gesture
 * state, and queues events during gesture detection phase.
 */
class CwlGesture : public QObject {
	Q_OBJECT
    public:
	/**
	 * @brief Construct a new gesture detector
	 * @param compositor The compositor instance for scale factor and gesture handling
	 * @param screenSize The screen dimensions for calculating detection areas
	 */
	CwlGesture(CwlCompositor *compositor, QSize screenSize);

	/**
	 * @brief Destructor
	 */
	~CwlGesture();

	/**
	 * @brief Process a pointer event for gesture detection
	 * @param ev The pointer event to analyze
	 * @param next Callback function to process queued events when gesture is confirmed/cancelled
	 * 
	 * This method analyzes pointer events to detect edge and corner gestures.
	 * Events are queued during detection and processed via the callback when
	 * the gesture is confirmed or cancelled.
	 */
	void handlePointerEvent(QPointerEvent *ev,
				std::function<void(QList<QEventPoint>)> next);

	/**
	 * @brief Confirm the current gesture and process queued events
	 * 
	 * This method is called when a gesture has been definitively detected
	 * and should be processed. It triggers processing of any queued events.
	 */
	void confirmGesture();

	/**
	 * @brief Get the starting point of the current gesture
	 * @return The initial touch/pointer position when gesture detection began
	 */
	QPointF startingPoint();

    private:
	/**
	 * @brief Update gesture detection rectangles based on current screen size
	 * 
	 * Recalculates the edge and corner detection areas when screen size
	 * or scale factor changes.
	 */
	void updateGestureRect();

	CwlCompositor *m_cwlcompositor =
		nullptr; ///< Compositor instance for callbacks and scale factor
	EdgeSwipe edge = EDGE_UNDEFINED; ///< Currently detected edge (if any)
	CornerSwipe corner =
		CORNER_UNDEFINED; ///< Currently detected corner (if any)
	QSize m_screenSize; ///< Current screen dimensions
	QPointF m_startingPoint; ///< Initial touch/pointer position
	bool m_gestureConfirmed =
		false; ///< Whether current gesture has been confirmed

	/**
	 * @brief Event queue for storing events during gesture detection
	 * 
	 * Events are queued while gesture detection is in progress and processed
	 * when the gesture is confirmed or cancelled.
	 */
	QStack<QPair<QList<QEventPoint>,
		     std::function<void(QList<QEventPoint>)> > >
		m_eventQueue;

	QRectF m_edges
		[EdgeSwipe::EDGE_UNDEFINED]; ///< Detection rectangles for each edge
	QRectF m_corners
		[CornerSwipe::CORNER_UNDEFINED]; ///< Detection rectangles for each corner
};
