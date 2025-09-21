#include "gesture-action.h"
#include "cutie-wlc.h"
#include "glwindow.h"
#include "gesture.h"
#include "extensions/input-method-v2.h"

// LeftEdgeGestureAction implementation
LeftEdgeGestureAction::LeftEdgeGestureAction(QObject *parent)
	: EdgeGestureAction(parent)
{
}

bool LeftEdgeGestureAction::canExecute(CwlCompositor *compositor) const
{
	return (compositor->launcherPosition() == 0.0) &&
	       (compositor->blur() != 0.0);
}

QString LeftEdgeGestureAction::actionName() const
{
	return "LeftEdgeGesture";
}

bool LeftEdgeGestureAction::handleBegin(QPointerEvent *ev,
					CwlCompositor *compositor)
{
	return canExecute(compositor);
}

bool LeftEdgeGestureAction::handleUpdate(QPointerEvent *ev,
					 CwlCompositor *compositor)
{
	if ((ev->points().first().globalPosition() -
	     compositor->glWindow()->gesture()->startingPoint())
		    .x() > GESTURE_MINIMUM_THRESHOLD) {
		compositor->glWindow()->gesture()->confirmGesture();
	}
	compositor->setBlur(1.0 -
			    1.0 * ev->points().first().globalPosition().x() /
				    compositor->glWindow()->width());
	return true;
}

bool LeftEdgeGestureAction::handleEnd(QPointerEvent *ev,
				      CwlCompositor *compositor)
{
	if (ev->points().first().globalPosition().x() >
	    GESTURE_ACCEPT_THRESHOLD) {
		compositor->raise(compositor->getHomeView());
		return true;
	}
	compositor->startBlurAnimation();
	compositor->setHomeOpen(false);
	return true;
}

// RightEdgeGestureAction implementation
RightEdgeGestureAction::RightEdgeGestureAction(QObject *parent)
	: EdgeGestureAction(parent)
{
}

bool RightEdgeGestureAction::canExecute(CwlCompositor *compositor) const
{
	return (compositor->launcherPosition() == 0.0) &&
	       (compositor->blur() != 0.0);
}

QString RightEdgeGestureAction::actionName() const
{
	return "RightEdgeGesture";
}

bool RightEdgeGestureAction::handleBegin(QPointerEvent *ev,
					 CwlCompositor *compositor)
{
	return canExecute(compositor);
}

bool RightEdgeGestureAction::handleUpdate(QPointerEvent *ev,
					  CwlCompositor *compositor)
{
	if ((-ev->points().first().globalPosition() +
	     compositor->glWindow()->gesture()->startingPoint())
		    .x() > GESTURE_MINIMUM_THRESHOLD) {
		compositor->glWindow()->gesture()->confirmGesture();
	}
	compositor->setBlur(1.0 * ev->points().first().globalPosition().x() /
			    compositor->glWindow()->width());
	return true;
}

bool RightEdgeGestureAction::handleEnd(QPointerEvent *ev,
				       CwlCompositor *compositor)
{
	if (ev->points().first().globalPosition().x() <
	    compositor->glWindow()->width() - GESTURE_ACCEPT_THRESHOLD) {
		compositor->raise(compositor->getHomeView());
		return true;
	}
	compositor->startBlurAnimation();
	compositor->setHomeOpen(false);
	return true;
}

// TopEdgeGestureAction implementation
TopEdgeGestureAction::TopEdgeGestureAction(QObject *parent)
	: EdgeGestureAction(parent)
{
}

bool TopEdgeGestureAction::canExecute(CwlCompositor *compositor) const
{
	return compositor->launcherPosition() > 0.0;
}

QString TopEdgeGestureAction::actionName() const
{
	return "TopEdgeGesture";
}

bool TopEdgeGestureAction::handleBegin(QPointerEvent *ev,
				       CwlCompositor *compositor)
{
	return canExecute(compositor);
}

bool TopEdgeGestureAction::handleUpdate(QPointerEvent *ev,
					CwlCompositor *compositor)
{
	if ((ev->points().first().globalPosition() -
	     compositor->glWindow()->gesture()->startingPoint())
		    .y() > GESTURE_MINIMUM_THRESHOLD) {
		compositor->glWindow()->gesture()->confirmGesture();
	}
	compositor->setLauncherPosition(qMin(
		1.0, 1.0 - (ev->points().first().globalPosition().y() /
				    compositor->scaleFactor() -
			    compositor->m_workspace->outputGeometry().y()) /
				     compositor->m_workspace->outputGeometry()
					     .height()));
	return true;
}

bool TopEdgeGestureAction::handleEnd(QPointerEvent *ev,
				     CwlCompositor *compositor)
{
	if (ev->points().first().globalPosition().y() <
	    compositor->glWindow()->height() * 0.2) {
		compositor->startLauncherOpenAnimation();
	} else {
		compositor->startLauncherCloseAnimation();
	}
	return true;
}

// BottomEdgeGestureAction implementation
BottomEdgeGestureAction::BottomEdgeGestureAction(QObject *parent)
	: EdgeGestureAction(parent)
{
}

bool BottomEdgeGestureAction::canExecute(CwlCompositor *compositor) const
{
	// Check panel state
	if (compositor->getPanelView() != nullptr &&
	    compositor->getPanelView()->panelState > 1) {
		return false;
	}

	// Check input method state
	if (compositor->getInputMethodManager()->getInputMethod() != nullptr &&
	    !compositor->getInputMethodManager()
		     ->getInputMethod()
		     ->isPanelHidden()) {
		return false;
	}

	return true;
}

QString BottomEdgeGestureAction::actionName() const
{
	return "BottomEdgeGesture";
}

bool BottomEdgeGestureAction::handleBegin(QPointerEvent *ev,
					  CwlCompositor *compositor)
{
	return canExecute(compositor);
}

bool BottomEdgeGestureAction::handleUpdate(QPointerEvent *ev,
					   CwlCompositor *compositor)
{
	if (!canExecute(compositor)) {
		return false;
	}

	if ((-ev->points().first().globalPosition() +
	     compositor->glWindow()->gesture()->startingPoint())
		    .y() > GESTURE_MINIMUM_THRESHOLD) {
		compositor->glWindow()->gesture()->confirmGesture();
		compositor->setLauncherPosition(qMin(
			1.0,
			1.0 - (ev->points().first().globalPosition().y() /
				       compositor->scaleFactor() -
			       compositor->m_workspace->outputGeometry().y()) /
					compositor->m_workspace
						->outputGeometry()
						.height()));
	}
	return true;
}

bool BottomEdgeGestureAction::handleEnd(QPointerEvent *ev,
					CwlCompositor *compositor)
{
	if (compositor->getPanelView() != nullptr &&
	    compositor->getPanelView()->panelState > 1) {
		return false;
	}

	if (ev->points().first().globalPosition().y() <
	    compositor->glWindow()->height() * 0.8) {
		compositor->startLauncherOpenAnimation();
	} else {
		compositor->startLauncherCloseAnimation();
	}
	return true;
}

// BottomCornerGestureAction implementation
BottomCornerGestureAction::BottomCornerGestureAction(int corner,
						     QObject *parent)
	: CornerGestureAction(corner, parent)
{
}

bool BottomCornerGestureAction::canExecute(CwlCompositor *compositor) const
{
	// Only handle bottom corners
	if (m_corner != CORNER_BR && m_corner != CORNER_BL) {
		return false;
	}

	// Check launcher position
	if (compositor->launcherPosition() > 0.0) {
		return false;
	}

	// Check panel state
	if (compositor->getPanelView() != nullptr &&
	    compositor->getPanelView()->panelState > 1) {
		return false;
	}

	return true;
}

QString BottomCornerGestureAction::actionName() const
{
	return QString("BottomCornerGesture_%1")
		.arg(m_corner == CORNER_BR ? "BR" : "BL");
}

bool BottomCornerGestureAction::handleBeginUpdate(QPointerEvent *ev,
						  CwlCompositor *compositor)
{
	if (!canExecute(compositor)) {
		return false;
	}

	// Check input method state during begin/update
	if (compositor->getInputMethodManager()->getInputMethod() != nullptr &&
	    !compositor->getInputMethodManager()
		     ->getInputMethod()
		     ->isPanelHidden()) {
		return false;
	}

	if ((-ev->points().first().globalPosition() +
	     compositor->glWindow()->gesture()->startingPoint())
		    .y() > GESTURE_MINIMUM_THRESHOLD) {
		compositor->glWindow()->gesture()->confirmGesture();
	}
	return true;
}

bool BottomCornerGestureAction::handleEnd(QPointerEvent *ev,
					  CwlCompositor *compositor)
{
	if (compositor->getPanelView() != nullptr &&
	    compositor->getPanelView()->panelState > 1) {
		return false;
	}

	if (ev->points().first().globalPosition().y() <
	    compositor->glWindow()->height() * 0.8) {
		compositor->getInputMethodManager()
			->getInputMethod()
			->showPanel();
		return true;
	}

	return false;
}