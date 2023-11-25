#pragma once

#include <QOpenGLWindow>
#include <QOpenGLTextureBlitter>
#include <QEventPoint>

#include <cutie-wlc.h>
#include <gesture.h>

QT_BEGIN_NAMESPACE

class GlWindow : public QOpenGLWindow {
    Q_OBJECT
public:
    GlWindow();
    void setCompositor(CwlCompositor *cwlcompositor);
    bool displayOff();
    void setDisplayOff(bool displayOff);

signals:
    void glReady();
    void displayOffChanged(bool displayOff);

protected:
    void initializeGL() override;
    void paintGL() override;

    void touchEvent(QTouchEvent *ev) override;
    void mouseMoveEvent(QMouseEvent *ev) override;
    void mousePressEvent(QMouseEvent *ev) override;
    void mouseReleaseEvent(QMouseEvent *ev) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    void renderView(CwlView *view);

    QOpenGLTextureBlitter m_textureBlitter;
    GLenum m_currentTarget;
    QOpenGLTexture *m_wallpaper = nullptr;

    QList<QEventPoint*> m_evPoint;
    bool m_displayOff = false;

    CwlCompositor *m_cwlcompositor = nullptr;
    CwlGesture *m_gesture = nullptr;
};

QT_END_NAMESPACE
