#include "gesture.h"
#include "gesture-manager.h"
#include <QDebug>

CwlGesture::CwlGesture(CwlCompositor *compositor, QSize screenSize)
	: m_cwlcompositor(compositor)
	, m_screenSize(screenSize)
{
	updateGestureRect();

	connect(m_cwlcompositor, &CwlCompositor::scaleFactorChanged, this,
		&CwlGesture::updateGestureRect);
}

CwlGesture::~CwlGesture()
{
}

void CwlGesture::handlePointerEvent(
	QPointerEvent *ev, std::function<void(QList<QEventPoint>)> next)
{
	if (ev->points().size() != 1) {
		corner = CORNER_UNDEFINED;
		edge = EDGE_UNDEFINED;
		next(ev->points());
		return;
	}

	bool handled = false;

	if (ev->isBeginEvent()) {
		m_startingPoint = ev->points().first().globalPosition();
		for (uint32_t e = 0; e < EDGE_UNDEFINED; ++e)
			if (m_edges[e].contains(m_startingPoint))
				edge = (EdgeSwipe)e;
		for (uint32_t c = 0; c < CORNER_UNDEFINED; ++c)
			if (m_corners[c].contains(m_startingPoint))
				corner = (CornerSwipe)c;

		if (edge < EDGE_UNDEFINED || corner < CORNER_UNDEFINED)
			handled = m_cwlcompositor->gestureManager()
					  ->handleGesture(ev, edge, corner);
	}

	if (ev->isUpdateEvent())
		if (edge < EDGE_UNDEFINED || corner < CORNER_UNDEFINED)
			handled = m_cwlcompositor->gestureManager()
					  ->handleGesture(ev, edge, corner);

	if (ev->isEndEvent())
		if (edge < EDGE_UNDEFINED || corner < CORNER_UNDEFINED) {
			handled = m_cwlcompositor->gestureManager()
					  ->handleGesture(ev, edge, corner);
			corner = CORNER_UNDEFINED;
			edge = EDGE_UNDEFINED;

			if (m_gestureConfirmed)
				m_eventQueue.clear();
			else {
				while (!m_eventQueue.isEmpty()) {
					auto oldEvent = m_eventQueue.pop();
					oldEvent.second(oldEvent.first);
				}
				next(ev->points());
			}

			m_gestureConfirmed = false;
			return;
		}

	if (handled) {
		m_eventQueue.push(
			QPair<QList<QEventPoint>,
			      std::function<void(QList<QEventPoint>)> >(
				ev->points(), next));
	} else {
		corner = CORNER_UNDEFINED;
		edge = EDGE_UNDEFINED;
		next(ev->points());
	}
}

void CwlGesture::confirmGesture()
{
	m_gestureConfirmed = true;
}

QPointF CwlGesture::startingPoint()
{
	return m_startingPoint;
}

void CwlGesture::updateGestureRect()
{
	m_edges[EdgeSwipe::EDGE_TOP] =
		QRect(SCALED_GESTURE_EDGE_OFFSET, 0,
		      m_screenSize.width() - (2 * SCALED_GESTURE_EDGE_OFFSET),
		      SCALED_GESTURE_EDGE_OFFSET);
	m_edges[EdgeSwipe::EDGE_BOTTOM] =
		QRect(SCALED_GESTURE_EDGE_OFFSET,
		      m_screenSize.height() - SCALED_GESTURE_EDGE_OFFSET,
		      m_screenSize.width() - (2 * SCALED_GESTURE_EDGE_OFFSET),
		      SCALED_GESTURE_EDGE_OFFSET);
	m_edges[EdgeSwipe::EDGE_LEFT] =
		QRect(0, SCALED_GESTURE_EDGE_OFFSET, SCALED_GESTURE_EDGE_OFFSET,
		      m_screenSize.height() - (2 * SCALED_GESTURE_EDGE_OFFSET));
	m_edges[EdgeSwipe::EDGE_RIGHT] =
		QRect(m_screenSize.width() - SCALED_GESTURE_EDGE_OFFSET,
		      SCALED_GESTURE_EDGE_OFFSET, SCALED_GESTURE_EDGE_OFFSET,
		      m_screenSize.height() - (2 * SCALED_GESTURE_EDGE_OFFSET));

	m_corners[CornerSwipe::CORNER_TL] = QRect(0, 0,
						  SCALED_GESTURE_CORNER_OFFSET,
						  SCALED_GESTURE_CORNER_OFFSET);
	m_corners[CornerSwipe::CORNER_TR] = QRect(
		m_screenSize.width() - SCALED_GESTURE_CORNER_OFFSET, 0,
		SCALED_GESTURE_CORNER_OFFSET, SCALED_GESTURE_CORNER_OFFSET);
	m_corners[CornerSwipe::CORNER_BL] = QRect(
		0, m_screenSize.height() - SCALED_GESTURE_CORNER_OFFSET,
		SCALED_GESTURE_CORNER_OFFSET, SCALED_GESTURE_CORNER_OFFSET);
	m_corners[CornerSwipe::CORNER_BR] = QRect(
		m_screenSize.width() - SCALED_GESTURE_CORNER_OFFSET,
		m_screenSize.height() - SCALED_GESTURE_CORNER_OFFSET,
		SCALED_GESTURE_CORNER_OFFSET, SCALED_GESTURE_CORNER_OFFSET);
}
