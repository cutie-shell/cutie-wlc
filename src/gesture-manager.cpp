#include "gesture-manager.h"
#include "gesture-action.h"
#include "cutie-wlc.h"
#include "gesture.h"
#include "glwindow.h"
#include "extensions/input-method-v2.h"

CwlGestureManager::CwlGestureManager(CwlCompositor *compositor, QObject *parent)
	: QObject(parent)
	, m_compositor(compositor)
{
	initializeActions();
	initializeGestureMapping();
}

CwlGestureManager::~CwlGestureManager()
{
	// Clean up corner actions (they are raw pointers in the hash)
	for (auto it = m_cornerActions.begin(); it != m_cornerActions.end();
	     ++it) {
		delete it.value();
	}

	// Clean up gesture registry actions
	for (auto it = m_gestureRegistry.begin(); it != m_gestureRegistry.end();
	     ++it) {
		delete it.value();
	}
}

void CwlGestureManager::initializeActions()
{
	// Initialize edge gesture actions
	m_leftEdgeAction = std::make_unique<LeftEdgeGestureAction>(this);
	m_rightEdgeAction = std::make_unique<RightEdgeGestureAction>(this);
	m_topEdgeAction = std::make_unique<TopEdgeGestureAction>(this);
	m_bottomEdgeAction = std::make_unique<BottomEdgeGestureAction>(this);

	// Initialize corner gesture actions (using raw pointers in hash)
	m_cornerActions[CORNER_BR] =
		new BottomCornerGestureAction(CORNER_BR, this);
	m_cornerActions[CORNER_BL] =
		new BottomCornerGestureAction(CORNER_BL, this);
}

bool CwlGestureManager::handleGesture(QPointerEvent *ev, int edge, int corner)
{
	// Use the new registry-based approach
	IGestureAction *action = findActionForGesture(ev, edge, corner);
	if (action && action->canExecute(m_compositor)) {
		return action->execute(ev, m_compositor);
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

	// Legacy fallback
	auto it = m_cornerActions.find(corner);
	if (it == m_cornerActions.end()) {
		return false;
	}

	IGestureAction *legacyAction = it.value();
	if (!legacyAction->canExecute(m_compositor)) {
		return false;
	}

	return legacyAction->execute(ev, m_compositor);
}

void CwlGestureManager::initializeGestureMapping()
{
	// Initialize the gesture registry with default mappings
	// Edge gesture mappings
	m_gestureRegistry[createGestureKey(GestureType::LEFT_EDGE)] =
		new LeftEdgeGestureAction(this);
	m_gestureRegistry[createGestureKey(GestureType::RIGHT_EDGE)] =
		new RightEdgeGestureAction(this);
	m_gestureRegistry[createGestureKey(GestureType::TOP_EDGE)] =
		new TopEdgeGestureAction(this);
	m_gestureRegistry[createGestureKey(GestureType::BOTTOM_EDGE)] =
		new BottomEdgeGestureAction(this);

	// Corner gesture mappings
	m_gestureRegistry[createGestureKey(GestureType::CORNER_BR, CORNER_BR)] =
		new BottomCornerGestureAction(CORNER_BR, this);
	m_gestureRegistry[createGestureKey(GestureType::CORNER_BL, CORNER_BL)] =
		new BottomCornerGestureAction(CORNER_BL, this);
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
	// Delete existing action if present
	auto it = m_gestureRegistry.find(key);
	if (it != m_gestureRegistry.end()) {
		delete it.value();
	}
	m_gestureRegistry[key] = action;
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
	// Delete existing action if present
	auto it = m_gestureRegistry.find(key);
	if (it != m_gestureRegistry.end()) {
		delete it.value();
	}
	m_gestureRegistry[key] = action;
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
		// Move the action from fromType to toType
		IGestureAction *action = fromIt.value();
		m_gestureRegistry.remove(fromKey);

		// Delete existing action at target if present
		auto toIt = m_gestureRegistry.find(toKey);
		if (toIt != m_gestureRegistry.end()) {
			delete toIt.value();
		}

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
	auto it = m_gestureRegistry.find(key);
	if (it != m_gestureRegistry.end()) {
		delete it.value();
		m_gestureRegistry.remove(key);
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
		delete it.value();
		m_gestureRegistry.remove(key);
	}
}