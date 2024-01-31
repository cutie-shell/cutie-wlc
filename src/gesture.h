#pragma once

#include <cutie-wlc.h>
#include <QTouchEvent>
#include <QStack>

// Minimum offset to consider the movement a gesture (either accepted or canceled)
#define GESTURE_MINIMUM_THRESHOLD 10
// Minimum offset to consider a gesture accepted.
#define GESTURE_ACCEPT_THRESHOLD 200
#define GESTURE_OFFSET 50

enum EdgeSwipe: uint32_t {
    EDGE_RIGHT      = 0,
    EDGE_LEFT       = 1,
    EDGE_TOP        = 2,
    EDGE_BOTTOM     = 3,
    EDGE_UNDEFINED  = 4
};

enum CornerSwipe: uint32_t {
    CORNER_TL        = 0,
    CORNER_TR        = 1,
    CORNER_BL        = 2,
    CORNER_BR        = 3,
    CORNER_UNDEFINED = 4
};

class CwlGesture : public QObject {
    Q_OBJECT
public:
    CwlGesture(CwlCompositor *compositor, QSize screenSize);
    ~CwlGesture();

    void handlePointerEvent(QPointerEvent *ev, std::function<void(QList<QEventPoint>)> next);
    void confirmGesture();
    QPointF startingPoint();

private:
    void updateGestureRect();
    inline int scaledGestureOffset() {
        return GESTURE_OFFSET * m_cwlcompositor->scaleFactor();
    }

    CwlCompositor *m_cwlcompositor = nullptr;
    EdgeSwipe edge = EDGE_UNDEFINED;
    CornerSwipe corner = CORNER_UNDEFINED;
    QSize m_screenSize;
    QPointF m_startingPoint;
    bool m_gestureConfirmed = false;
    QStack<QPair<QList<QEventPoint>, std::function<void(QList<QEventPoint>)>>> m_eventQueue;

    QRectF m_edges[EdgeSwipe::EDGE_UNDEFINED];
    QRectF m_corners[CornerSwipe::CORNER_UNDEFINED];
};
