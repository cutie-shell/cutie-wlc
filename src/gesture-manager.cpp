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
}

CwlGestureManager::~CwlGestureManager()
{
	// Clean up corner actions (they are raw pointers in the hash)
	for (auto it = m_cornerActions.begin(); it != m_cornerActions.end();
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
	if (!m_leftEdgeAction->canExecute(m_compositor)) {
		return false;
	}
	return m_leftEdgeAction->execute(ev, m_compositor);
}

bool CwlGestureManager::handleRightEdgeGesture(QPointerEvent *ev)
{
	if (!m_rightEdgeAction->canExecute(m_compositor)) {
		return false;
	}
	return m_rightEdgeAction->execute(ev, m_compositor);
}

bool CwlGestureManager::handleTopEdgeGesture(QPointerEvent *ev)
{
	if (!m_topEdgeAction->canExecute(m_compositor)) {
		return false;
	}
	return m_topEdgeAction->execute(ev, m_compositor);
}

bool CwlGestureManager::handleBottomEdgeGesture(QPointerEvent *ev)
{
	if (!m_bottomEdgeAction->canExecute(m_compositor)) {
		return false;
	}
	return m_bottomEdgeAction->execute(ev, m_compositor);
}

bool CwlGestureManager::handleCornerGesture(QPointerEvent *ev, int corner)
{
	auto it = m_cornerActions.find(corner);
	if (it == m_cornerActions.end()) {
		return false;
	}

	IGestureAction *action = it.value();
	if (!action->canExecute(m_compositor)) {
		return false;
	}

	return action->execute(ev, m_compositor);
}