#include "gesture-manager.h"
#include "cutie-wlc.h"
#include "gesture.h"
#include "glwindow.h"
#include "extensions/input-method-v2.h"

CwlGestureManager::CwlGestureManager(CwlCompositor *compositor, QObject *parent)
	: QObject(parent)
	, m_compositor(compositor)
{
}

CwlGestureManager::~CwlGestureManager()
{
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
	if (ev->isBeginEvent()) {
		return (m_compositor->launcherPosition() == 0.0) &&
		       (m_compositor->blur() != 0.0);
	}

	if (ev->isUpdateEvent()) {
		if ((ev->points().first().globalPosition() -
		     m_compositor->glWindow()->gesture()->startingPoint())
			    .x() > GESTURE_MINIMUM_THRESHOLD) {
			m_compositor->glWindow()->gesture()->confirmGesture();
		}
		m_compositor->setBlur(
			1.0 - 1.0 * ev->points().first().globalPosition().x() /
				      m_compositor->glWindow()->width());
		return true;
	}

	if (ev->isEndEvent()) {
		if (ev->points().first().globalPosition().x() >
		    GESTURE_ACCEPT_THRESHOLD) {
			m_compositor->raise(m_compositor->getHomeView());
			return true;
		}
		m_compositor->startBlurAnimation();
		m_compositor->setHomeOpen(false);
		return true;
	}

	return false;
}

bool CwlGestureManager::handleRightEdgeGesture(QPointerEvent *ev)
{
	if (ev->isBeginEvent()) {
		return (m_compositor->launcherPosition() == 0.0) &&
		       (m_compositor->blur() != 0.0);
	}

	if (ev->isUpdateEvent()) {
		if ((-ev->points().first().globalPosition() +
		     m_compositor->glWindow()->gesture()->startingPoint())
			    .x() > GESTURE_MINIMUM_THRESHOLD) {
			m_compositor->glWindow()->gesture()->confirmGesture();
		}
		m_compositor->setBlur(
			1.0 * ev->points().first().globalPosition().x() /
			m_compositor->glWindow()->width());
		return true;
	}

	if (ev->isEndEvent()) {
		if (ev->points().first().globalPosition().x() <
		    m_compositor->glWindow()->width() -
			    GESTURE_ACCEPT_THRESHOLD) {
			m_compositor->raise(m_compositor->getHomeView());
			return true;
		}
		m_compositor->startBlurAnimation();
		m_compositor->setHomeOpen(false);
		return true;
	}

	return false;
}

bool CwlGestureManager::handleTopEdgeGesture(QPointerEvent *ev)
{
	if (m_compositor->launcherPosition() <= 0.0) {
		return false;
	}

	if (ev->isBeginEvent() || ev->isUpdateEvent()) {
		if ((ev->points().first().globalPosition() -
		     m_compositor->glWindow()->gesture()->startingPoint())
			    .y() > GESTURE_MINIMUM_THRESHOLD) {
			m_compositor->glWindow()->gesture()->confirmGesture();
		}
		m_compositor->setLauncherPosition(qMin(
			1.0,
			1.0 - (ev->points().first().globalPosition().y() /
				       m_compositor->scaleFactor() -
			       m_compositor->m_workspace->outputGeometry().y()) /
					m_compositor->m_workspace
						->outputGeometry()
						.height()));
		return true;
	}

	if (ev->isEndEvent()) {
		if (ev->points().first().globalPosition().y() <
		    m_compositor->glWindow()->height() * 0.2)
			m_compositor->startLauncherOpenAnimation();
		else
			m_compositor->startLauncherCloseAnimation();
		return true;
	}

	return false;
}

bool CwlGestureManager::handleBottomEdgeGesture(QPointerEvent *ev)
{
	if (ev->isBeginEvent() || ev->isUpdateEvent()) {
		if (m_compositor->getPanelView() != nullptr)
			if (m_compositor->getPanelView()->panelState > 1)
				return false;
		if (m_compositor->getInputMethodManager()->getInputMethod() !=
		    nullptr)
			if (!m_compositor->getInputMethodManager()
				     ->getInputMethod()
				     ->isPanelHidden())
				return false;
		if ((-ev->points().first().globalPosition() +
		     m_compositor->glWindow()->gesture()->startingPoint())
			    .y() > GESTURE_MINIMUM_THRESHOLD) {
			m_compositor->glWindow()->gesture()->confirmGesture();
			m_compositor->setLauncherPosition(qMin(
				1.0,
				1.0 - (ev->points().first().globalPosition().y() /
					       m_compositor->scaleFactor() -
				       m_compositor->m_workspace
					       ->outputGeometry()
					       .y()) /
						m_compositor->m_workspace
							->outputGeometry()
							.height()));
		}
		return true;
	}

	if (ev->isEndEvent()) {
		if (m_compositor->getPanelView() != nullptr) {
			if (m_compositor->getPanelView()->panelState > 1) {
				return false;
			}
		}
		if (ev->points().first().globalPosition().y() <
		    m_compositor->glWindow()->height() * 0.8)
			m_compositor->startLauncherOpenAnimation();
		else
			m_compositor->startLauncherCloseAnimation();
		return true;
	}

	return false;
}

bool CwlGestureManager::handleCornerGesture(QPointerEvent *ev, int corner)
{
	if (corner != CORNER_BR && corner != CORNER_BL) {
		return false;
	}

	if (m_compositor->launcherPosition() > 0.0) {
		return false;
	}

	if (m_compositor->getPanelView() != nullptr)
		if (m_compositor->getPanelView()->panelState > 1)
			return false;

	if (ev->isBeginEvent() || ev->isUpdateEvent()) {
		if (m_compositor->getInputMethodManager()->getInputMethod() !=
		    nullptr)
			if (!m_compositor->getInputMethodManager()
				     ->getInputMethod()
				     ->isPanelHidden()) {
				return false;
			}
		if ((-ev->points().first().globalPosition() +
		     m_compositor->glWindow()->gesture()->startingPoint())
			    .y() > GESTURE_MINIMUM_THRESHOLD) {
			m_compositor->glWindow()->gesture()->confirmGesture();
		}
		return true;
	}

	if (ev->isEndEvent()) {
		if (m_compositor->getPanelView() != nullptr) {
			if (m_compositor->getPanelView()->panelState > 1) {
				return false;
			}
		}
		if (ev->points().first().globalPosition().y() <
		    m_compositor->glWindow()->height() * 0.8) {
			m_compositor->getInputMethodManager()
				->getInputMethod()
				->showPanel();
			return true;
		}
	}

	return false;
}