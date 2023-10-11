#ifndef GLWINDOW_H
#define GLWINDOW_H

#include <QOpenGLWindow>
#include <QOpenGLTextureBlitter>

#include <QEventPoint>
#include <cutie-wlc.h>
#include <gesture.h>

QT_BEGIN_NAMESPACE

class CwlCompositor;

class GlWindow : public QOpenGLWindow
{
    Q_OBJECT
public:
    GlWindow();
    void setCompositor(CwlCompositor *cwlcompositor);
    void setAppswitcher(CwlAppswitcher *appswitcher);

signals:
    void glReady();

protected:
    void initializeGL() override;
    void paintGL() override;

    void touchEvent(QTouchEvent *ev) override;

private:
    QOpenGLTextureBlitter m_textureBlitter;
    CwlCompositor *m_cwlcompositor = nullptr;
    CwlAppswitcher *m_appswitcher = nullptr;
    QList<QEventPoint*> m_evPoint;

    CwlGesture *m_gesture = nullptr;
};

QT_END_NAMESPACE

#endif //GLWINDOW_H