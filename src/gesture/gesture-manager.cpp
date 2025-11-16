#include "gesture-manager.h"
#include "gesture-action.h"
#include <cutie-wlc.h>
#include "gesture.h"
#include <glwindow.h>
#include "extensions/input-method-v2.h"
#include <QDateTime>

CwlGestureManager::CwlGestureManager(CwlCompositor *compositor, QObject *parent)
	: QObject(parent)
	, m_compositor(compositor)
	, m_gestureTimeout(new QTimer(this))
{
	initializeActions();
	initializeGestureMapping();

	// Initialize gesture timeout timer
	m_gestureTimeout->setSingleShot(false);
	m_gestureTimeout->setInterval(1000); // Check every second
	connect(m_gestureTimeout, &QTimer::timeout, this,
		&CwlGestureManager::onGestureTimeout);
	m_gestureTimeout->start();
}

CwlGestureManager::~CwlGestureManager()
{
	// Cancel all active gestures before cleanup
	cancelAllGestures();

	// m_cornerActions and edge action unique_ptr members own the actions and
	// will be cleaned up automatically. The m_gestureRegistry holds
	// non-owning pointers and therefore should not delete the targets here.
}

void CwlGestureManager::initializeActions()
{
	// Initialize edge gesture actions
	m_leftEdgeAction = std::make_unique<LeftEdgeGestureAction>(this);
	m_rightEdgeAction = std::make_unique<RightEdgeGestureAction>(this);
	m_topEdgeAction = std::make_unique<TopEdgeGestureAction>(this);
	m_bottomEdgeAction = std::make_unique<BottomEdgeGestureAction>(this);

	// Initialize corner gesture actions (owned by unique_ptr in the hash)
	m_cornerActions[CORNER_BR] =
		std::make_unique<BottomCornerGestureAction>(CORNER_BR, this);
	m_cornerActions[CORNER_BL] =
		std::make_unique<BottomCornerGestureAction>(CORNER_BL, this);
}

bool CwlGestureManager::handleGesture(QPointerEvent *ev, int edge, int corner)
{
	// Clean up any expired gestures first
	cleanupExpiredGestures();

	// Create gesture key for the detected gesture
	GestureKey gestureKey;
	if (edge != EDGE_UNDEFINED) {
		gestureKey = createGestureKey(convertEdgeToGestureType(edge));
	} else if (corner == CORNER_BR || corner == CORNER_BL) {
		GestureType cornerType = (corner == CORNER_BR) ?
						 GestureType::CORNER_BR :
						 GestureType::CORNER_BL;
		gestureKey = createGestureKey(cornerType, corner);
	} else {
		return false; // No valid gesture detected
	}

	// Check if we can start this gesture (considering conflicts and priority)
	if (!canStartGesture(gestureKey, 0)) {
		return false;
	}

	// Use the new registry-based approach
	IGestureAction *action = findActionForGesture(ev, edge, corner);
	if (action && action->canExecute(m_compositor)) {
		// Check if gesture is already active - if so, just execute it
		if (isGestureActive(gestureKey)) {
			bool result = action->execute(ev, m_compositor);
			if (!result && ev->isEndEvent()) {
				// Gesture execution failed on end event
				updateGestureState(gestureKey,
						   GestureState::FAILED);
				cancelGesture(gestureKey);
			} else if (ev->isEndEvent()) {
				// Gesture completed successfully
				completeGesture(gestureKey);
			}
			return result;
		}
		
		// Start gesture state tracking for new gesture
		if (ev->isBeginEvent() && startGesture(gestureKey, ev, action, 0)) {
			bool result = action->execute(ev, m_compositor);
			if (result) {
				updateGestureState(gestureKey,
						   GestureState::ACTIVE);
				return true;
			} else {
				// Gesture execution failed
				updateGestureState(gestureKey,
						   GestureState::FAILED);
				cancelGesture(gestureKey);
				return false;
			}
		}
	}

	// Fallback to legacy approach for backward compatibility
	switch (edge) {
	case EDGE_LEFT:
		return handleLeftEdgeGesture(ev);
	case EDGE_RIGHT:
		return handleRightEdgeGesture(ev);
	case EDGE_BOTTOM:
		return handleBottomEdgeGesture(ev);
	case EDGE_TOP:
		return handleTopEdgeGesture(ev);
	default:
		break;
	}

	// Handle corner gestures if no edge gesture was processed
	if (corner == CORNER_BR || corner == CORNER_BL) {
		return handleCornerGesture(ev, corner);
	}

	return false;
}

bool CwlGestureManager::handleLeftEdgeGesture(QPointerEvent *ev)
{
	// Try registry first, fallback to legacy
	IGestureAction *action = getGestureAction(GestureType::LEFT_EDGE);
	if (action) {
		if (action->canExecute(m_compositor)) {
			return action->execute(ev, m_compositor);
		}
		return false;
	}

	// Legacy fallback
	if (!m_leftEdgeAction->canExecute(m_compositor)) {
		return false;
	}
	return m_leftEdgeAction->execute(ev, m_compositor);
}

bool CwlGestureManager::handleRightEdgeGesture(QPointerEvent *ev)
{
	// Try registry first, fallback to legacy
	IGestureAction *action = getGestureAction(GestureType::RIGHT_EDGE);
	if (action) {
		if (action->canExecute(m_compositor)) {
			return action->execute(ev, m_compositor);
		}
		return false;
	}

	// Legacy fallback
	if (!m_rightEdgeAction->canExecute(m_compositor)) {
		return false;
	}
	return m_rightEdgeAction->execute(ev, m_compositor);
}

bool CwlGestureManager::handleTopEdgeGesture(QPointerEvent *ev)
{
	// Try registry first, fallback to legacy
	IGestureAction *action = getGestureAction(GestureType::TOP_EDGE);
	if (action) {
		if (action->canExecute(m_compositor)) {
			return action->execute(ev, m_compositor);
		}
		return false;
	}

	// Legacy fallback
	if (!m_topEdgeAction->canExecute(m_compositor)) {
		return false;
	}
	return m_topEdgeAction->execute(ev, m_compositor);
}

bool CwlGestureManager::handleBottomEdgeGesture(QPointerEvent *ev)
{
	// Try registry first, fallback to legacy
	IGestureAction *action = getGestureAction(GestureType::BOTTOM_EDGE);
	if (action) {
		if (action->canExecute(m_compositor)) {
			return action->execute(ev, m_compositor);
		}
		return false;
	}

	// Legacy fallback
	if (!m_bottomEdgeAction->canExecute(m_compositor)) {
		return false;
	}
	return m_bottomEdgeAction->execute(ev, m_compositor);
}

bool CwlGestureManager::handleCornerGesture(QPointerEvent *ev, int corner)
{
	// Try registry first, fallback to legacy
	IGestureAction *action = getCornerGestureAction(corner);
	if (action) {
		if (action->canExecute(m_compositor)) {
			return action->execute(ev, m_compositor);
		}
		return false;
	}

	// Legacy fallback: corner actions are owned as unique_ptrs in m_cornerActions
	auto it = m_cornerActions.find(corner);
	if (it == m_cornerActions.end()) {
		return false;
	}

	IGestureAction *legacyAction = it.value().get();
	if (!legacyAction->canExecute(m_compositor)) {
		return false;
	}

	return legacyAction->execute(ev, m_compositor);
}

void CwlGestureManager::initializeGestureMapping()
{
	// Initialize the gesture registry with default mappings
	// Edge gesture mappings
	// Point the registry at the owned action objects to avoid duplicated
	// instances. The registry stores non-owning pointers.
	m_gestureRegistry[createGestureKey(GestureType::LEFT_EDGE)] =
		m_leftEdgeAction.get();
	m_gestureRegistry[createGestureKey(GestureType::RIGHT_EDGE)] =
		m_rightEdgeAction.get();
	m_gestureRegistry[createGestureKey(GestureType::TOP_EDGE)] =
		m_topEdgeAction.get();
	m_gestureRegistry[createGestureKey(GestureType::BOTTOM_EDGE)] =
		m_bottomEdgeAction.get();

	// Corner gesture mappings - use the unique_ptr-owned corner actions
	m_gestureRegistry[createGestureKey(GestureType::CORNER_BR, CORNER_BR)] =
		m_cornerActions[CORNER_BR].get();
	m_gestureRegistry[createGestureKey(GestureType::CORNER_BL, CORNER_BL)] =
		m_cornerActions[CORNER_BL].get();
}

GestureType CwlGestureManager::convertEdgeToGestureType(int edge) const
{
	switch (edge) {
	case EDGE_LEFT:
		return GestureType::LEFT_EDGE;
	case EDGE_RIGHT:
		return GestureType::RIGHT_EDGE;
	case EDGE_TOP:
		return GestureType::TOP_EDGE;
	case EDGE_BOTTOM:
		return GestureType::BOTTOM_EDGE;
	default:
		return GestureType::UNDEFINED;
	}
}

GestureKey CwlGestureManager::createGestureKey(GestureType type,
					       int cornerPosition) const
{
	GestureKey key;
	key.type = type;
	key.cornerPosition = cornerPosition;
	return key;
}

IGestureAction *CwlGestureManager::findActionForGesture(QPointerEvent *ev,
							int edge,
							int corner) const
{
	// First try to find edge gesture action
	if (edge != EDGE_UNDEFINED) {
		GestureType gestureType = convertEdgeToGestureType(edge);
		if (gestureType != GestureType::UNDEFINED) {
			GestureKey key = createGestureKey(gestureType);
			auto it = m_gestureRegistry.find(key);
			if (it != m_gestureRegistry.end()) {
				return it.value();
			}
		}
	}

	// Then try corner gesture action
	if (corner == CORNER_BR || corner == CORNER_BL) {
		GestureType cornerType = (corner == CORNER_BR) ?
						 GestureType::CORNER_BR :
						 GestureType::CORNER_BL;
		GestureKey key = createGestureKey(cornerType, corner);
		auto it = m_gestureRegistry.find(key);
		if (it != m_gestureRegistry.end()) {
			return it.value();
		}
	}

	return nullptr;
}

void CwlGestureManager::setGestureAction(GestureType type,
					 IGestureAction *action)
{
	GestureKey key = createGestureKey(type);
	// For edge types we take ownership into the corresponding unique_ptr
	// so ownership is clear. For other types we store a non-owning pointer
	// in the registry.
	switch (type) {
	case GestureType::LEFT_EDGE:
		m_leftEdgeAction.reset(action);
		m_gestureRegistry[key] = m_leftEdgeAction.get();
		break;
	case GestureType::RIGHT_EDGE:
		m_rightEdgeAction.reset(action);
		m_gestureRegistry[key] = m_rightEdgeAction.get();
		break;
	case GestureType::TOP_EDGE:
		m_topEdgeAction.reset(action);
		m_gestureRegistry[key] = m_topEdgeAction.get();
		break;
	case GestureType::BOTTOM_EDGE:
		m_bottomEdgeAction.reset(action);
		m_gestureRegistry[key] = m_bottomEdgeAction.get();
		break;
	default:
		m_gestureRegistry[key] = action;
		break;
	}
}

void CwlGestureManager::setCornerGestureAction(int cornerPosition,
					       IGestureAction *action)
{
	GestureType cornerType;
	switch (cornerPosition) {
	case CORNER_TL:
		cornerType = GestureType::CORNER_TL;
		break;
	case CORNER_TR:
		cornerType = GestureType::CORNER_TR;
		break;
	case CORNER_BL:
		cornerType = GestureType::CORNER_BL;
		break;
	case CORNER_BR:
		cornerType = GestureType::CORNER_BR;
		break;
	default:
		return; // Invalid corner position
	}

	GestureKey key = createGestureKey(cornerType, cornerPosition);
	// Take ownership of the provided action into the corner actions map
	m_cornerActions[cornerPosition].reset(action);
	// Update registry to point to the owned action
	m_gestureRegistry[key] = m_cornerActions[cornerPosition].get();
}

IGestureAction *CwlGestureManager::getGestureAction(GestureType type) const
{
	GestureKey key = createGestureKey(type);
	auto it = m_gestureRegistry.find(key);
	return (it != m_gestureRegistry.end()) ? it.value() : nullptr;
}

IGestureAction *
CwlGestureManager::getCornerGestureAction(int cornerPosition) const
{
	GestureType cornerType;
	switch (cornerPosition) {
	case CORNER_TL:
		cornerType = GestureType::CORNER_TL;
		break;
	case CORNER_TR:
		cornerType = GestureType::CORNER_TR;
		break;
	case CORNER_BL:
		cornerType = GestureType::CORNER_BL;
		break;
	case CORNER_BR:
		cornerType = GestureType::CORNER_BR;
		break;
	default:
		return nullptr; // Invalid corner position
	}

	GestureKey key = createGestureKey(cornerType, cornerPosition);
	auto it = m_gestureRegistry.find(key);
	return (it != m_gestureRegistry.end()) ? it.value() : nullptr;
}

void CwlGestureManager::remapGesture(GestureType fromType, GestureType toType)
{
	GestureKey fromKey = createGestureKey(fromType);
	GestureKey toKey = createGestureKey(toType);

	auto fromIt = m_gestureRegistry.find(fromKey);
	if (fromIt != m_gestureRegistry.end()) {
		// Move the mapping (non-owning pointer) from one key to another.
		IGestureAction *action = fromIt.value();
		m_gestureRegistry.remove(fromKey);
		m_gestureRegistry[toKey] = action;
	}
}

void CwlGestureManager::remapCornerGesture(int fromCorner, int toCorner)
{
	IGestureAction *action = getCornerGestureAction(fromCorner);
	if (action) {
		// Create a new action for the target corner
		// Note: This is a simplified approach; in a real implementation,
		// you might want to clone the action or implement action copying
		clearCornerGestureMapping(fromCorner);
		// The actual remapping would need action cloning capability
	}
}

void CwlGestureManager::clearGestureMapping(GestureType type)
{
	GestureKey key = createGestureKey(type);
	// For edges, also clear owned action. For other gestures, simply remove
	// the non-owning registry entry.
	auto it = m_gestureRegistry.find(key);
	if (it != m_gestureRegistry.end()) {
		m_gestureRegistry.remove(key);
	}
	switch (type) {
	case GestureType::LEFT_EDGE:
		m_leftEdgeAction.reset();
		break;
	case GestureType::RIGHT_EDGE:
		m_rightEdgeAction.reset();
		break;
	case GestureType::TOP_EDGE:
		m_topEdgeAction.reset();
		break;
	case GestureType::BOTTOM_EDGE:
		m_bottomEdgeAction.reset();
		break;
	default:
		break;
	}
}

void CwlGestureManager::clearCornerGestureMapping(int cornerPosition)
{
	GestureType cornerType;
	switch (cornerPosition) {
	case CORNER_TL:
		cornerType = GestureType::CORNER_TL;
		break;
	case CORNER_TR:
		cornerType = GestureType::CORNER_TR;
		break;
	case CORNER_BL:
		cornerType = GestureType::CORNER_BL;
		break;
	case CORNER_BR:
		cornerType = GestureType::CORNER_BR;
		break;
	default:
		return; // Invalid corner position
	}

	GestureKey key = createGestureKey(cornerType, cornerPosition);
	auto it = m_gestureRegistry.find(key);
	if (it != m_gestureRegistry.end()) {
		m_gestureRegistry.remove(key);
	}

	// Reset the owned corner action (if any)
	auto cornerIt = m_cornerActions.find(cornerPosition);
	if (cornerIt != m_cornerActions.end()) {
		cornerIt.value().reset();
	}
}

// Gesture State Management Implementation

bool CwlGestureManager::startGesture(const GestureKey &key, QPointerEvent *ev,
				     IGestureAction *action, int priority)
{
	if (!canStartGesture(key, priority)) {
		return false;
	}

	// Resolve any conflicts with existing gestures
	resolveGestureConflicts(key, priority);

	// Create and store the active gesture
	ActiveGesture activeGesture(key, action, priority);
	activeGesture.startTime = QDateTime::currentMSecsSinceEpoch();
	// Store a safe copy of the event points and the event phase so the
	// active gesture doesn't refer to a potentially destroyed QPointerEvent.
	activeGesture.lastEventPoints = ev->points();
	if (ev->isBeginEvent())
		activeGesture.lastEventPhase = 1;
	else if (ev->isUpdateEvent())
		activeGesture.lastEventPhase = 2;
	else if (ev->isEndEvent())
		activeGesture.lastEventPhase = 3;
	activeGesture.state = GestureState::DETECTING;

	m_activeGestures[key] = activeGesture;
	return true;
}

void CwlGestureManager::updateGestureState(const GestureKey &key,
					   GestureState newState)
{
	auto it = m_activeGestures.find(key);
	if (it != m_activeGestures.end()) {
		it.value().state = newState;

		// If gesture is completed or failed, schedule cleanup
		if (newState == GestureState::COMPLETED ||
		    newState == GestureState::FAILED) {
			// Use a single-shot timer to delay cleanup
			QTimer::singleShot(100, [this, key]() {
				m_activeGestures.remove(key);
			});
		}
	}
}

void CwlGestureManager::completeGesture(const GestureKey &key)
{
	updateGestureState(key, GestureState::COMPLETED);
}

void CwlGestureManager::cancelGesture(const GestureKey &key)
{
	auto it = m_activeGestures.find(key);
	if (it != m_activeGestures.end()) {
		ActiveGesture &gesture = it.value();

		// Set state to cancelling
		gesture.state = GestureState::CANCELLING;

		// Call the action's cancel method to cleanly revert UI state
		if (gesture.action) {
			gesture.action->cancel(m_compositor);
		}

		// Remove the gesture
		m_activeGestures.remove(key);
	}
}

void CwlGestureManager::cancelAllGestures()
{
	QList<GestureKey> gestureKeys = m_activeGestures.keys();
	for (const GestureKey &key : gestureKeys) {
		cancelGesture(key);
	}
}

bool CwlGestureManager::isGestureActive(const GestureKey &key) const
{
	auto it = m_activeGestures.find(key);
	if (it != m_activeGestures.end()) {
		GestureState state = it.value().state;
		return state == GestureState::DETECTING ||
		       state == GestureState::ACTIVE;
	}
	return false;
}

GestureState CwlGestureManager::getGestureState(const GestureKey &key) const
{
	auto it = m_activeGestures.find(key);
	if (it != m_activeGestures.end()) {
		return it.value().state;
	}
	return GestureState::IDLE;
}

QList<ActiveGesture> CwlGestureManager::getActiveGestures() const
{
	return m_activeGestures.values();
}

// Gesture Conflict Resolution Implementation

bool CwlGestureManager::hasConflictingGestures(const GestureKey &key) const
{
	for (auto it = m_activeGestures.begin(); it != m_activeGestures.end();
	     ++it) {
		if (isConflictingGesture(it.key(), key)) {
			return true;
		}
	}
	return false;
}

void CwlGestureManager::resolveGestureConflicts(const GestureKey &newGesture,
						int priority)
{
	QList<GestureKey> conflictingGestures;

	// Find all conflicting gestures
	for (auto it = m_activeGestures.begin(); it != m_activeGestures.end();
	     ++it) {
		if (isConflictingGesture(it.key(), newGesture)) {
			const ActiveGesture &existing = it.value();
			// Cancel lower priority gestures
			if (existing.priority <= priority) {
				conflictingGestures.append(it.key());
			}
		}
	}

	// Cancel conflicting gestures
	for (const GestureKey &key : conflictingGestures) {
		cancelGesture(key);
	}
}

bool CwlGestureManager::canStartGesture(const GestureKey &key,
					int priority) const
{
	// Check if there are conflicting gestures with higher priority
	for (auto it = m_activeGestures.begin(); it != m_activeGestures.end();
	     ++it) {
		if (isConflictingGesture(it.key(), key)) {
			const ActiveGesture &existing = it.value();
			if (existing.priority > priority) {
				return false; // Higher priority gesture is active
			}
		}
	}
	return true;
}

// Helper Methods Implementation

void CwlGestureManager::cleanupExpiredGestures()
{
	qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
	QList<GestureKey> expiredGestures;

	for (auto it = m_activeGestures.begin(); it != m_activeGestures.end();
	     ++it) {
		const ActiveGesture &gesture = it.value();
		if (currentTime - gesture.startTime > GESTURE_TIMEOUT_MS) {
			expiredGestures.append(it.key());
		}
	}

	// Cancel expired gestures
	for (const GestureKey &key : expiredGestures) {
		cancelGesture(key);
	}
}

void CwlGestureManager::onGestureTimeout()
{
	cleanupExpiredGestures();
}

bool CwlGestureManager::isConflictingGesture(const GestureKey &existing,
					     const GestureKey &newGesture) const
{
	// Same gesture type is always conflicting
	if (existing == newGesture) {
		return true;
	}

	// Edge gestures conflict with each other (only one edge gesture at a time)
	if ((existing.type == GestureType::LEFT_EDGE ||
	     existing.type == GestureType::RIGHT_EDGE ||
	     existing.type == GestureType::TOP_EDGE ||
	     existing.type == GestureType::BOTTOM_EDGE) &&
	    (newGesture.type == GestureType::LEFT_EDGE ||
	     newGesture.type == GestureType::RIGHT_EDGE ||
	     newGesture.type == GestureType::TOP_EDGE ||
	     newGesture.type == GestureType::BOTTOM_EDGE)) {
		return true;
	}

	// Corner gestures on the same side might conflict
	if (existing.cornerPosition != -1 && newGesture.cornerPosition != -1) {
		// Bottom corners conflict with each other
		if ((existing.cornerPosition == CORNER_BL ||
		     existing.cornerPosition == CORNER_BR) &&
		    (newGesture.cornerPosition == CORNER_BL ||
		     newGesture.cornerPosition == CORNER_BR)) {
			return true;
		}
		// Top corners conflict with each other
		if ((existing.cornerPosition == CORNER_TL ||
		     existing.cornerPosition == CORNER_TR) &&
		    (newGesture.cornerPosition == CORNER_TL ||
		     newGesture.cornerPosition == CORNER_TR)) {
			return true;
		}
	}

	return false;
}