#ifndef CWL_GESTURE_H
#define CWL_GESTURE_H

#include <cutie-wlc.h>
#include <QTouchEvent>

#define GESTURE_ACCEPT_THRESHOLD 100

enum EdgeSwipe: uint32_t
{
    EDGE_RIGHT      = 0,
    EDGE_LEFT       = 1,
    EDGE_TOP        = 2,
    EDGE_BOTTOM     = 3,
    EDGE_UNDEFINED  = 4
};

enum CornerSwipe: uint32_t
{
    CORNER_TL        = 0,
    CORNER_TR        = 1,
    CORNER_BL        = 2,
    CORNER_BR        = 3,
    CORNER_UNDEFINED = 4
};

class CwlGesture : public QObject
{
    Q_OBJECT
public:
    CwlGesture(CwlCompositor *compositor, QSize screenSize);
    ~CwlGesture();

    void handlePointerEvent(QPointerEvent *ev, std::function<void(QPointerEvent*)> next);

public slots:
    
signals:

private:
    void updateGestureRect();
    int scaledGestureOffset();

    CwlCompositor *m_cwlcompositor = nullptr;
    EdgeSwipe edge = EDGE_UNDEFINED;
    CornerSwipe corner = CORNER_UNDEFINED;
    QSize m_screenSize;
    int m_gestureOffset = 20;

    QRectF m_topEdge;
    QRectF m_bottomEdge;
    QRectF m_leftEdge;
    QRectF m_rightEdge;

    QRectF m_tlCorner;
    QRectF m_trCorner;
    QRectF m_blCorner;
    QRectF m_brCorner;
    
};

#endif //CWL_GESTURE_H
