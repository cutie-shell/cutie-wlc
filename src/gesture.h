#pragma once

#include <cutie-wlc.h>
#include <QTouchEvent>

#define GESTURE_ACCEPT_THRESHOLD 100
#define GESTURE_OFFSET 20

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

    void handlePointerEvent(QPointerEvent *ev, std::function<void(QPointerEvent*)> next);

private:
    void updateGestureRect();
    inline int scaledGestureOffset() {
        return GESTURE_OFFSET * m_cwlcompositor->scaleFactor();
    }

    CwlCompositor *m_cwlcompositor = nullptr;
    EdgeSwipe edge = EDGE_UNDEFINED;
    CornerSwipe corner = CORNER_UNDEFINED;
    QSize m_screenSize;

    QRectF m_edges[EdgeSwipe::EDGE_UNDEFINED];
    QRectF m_corners[CornerSwipe::CORNER_UNDEFINED];
};
