#ifndef GLWINDOW_H
#define GLWINDOW_H

#include <QOpenGLWindow>
#include <QOpenGLTextureBlitter>

#include <QEventPoint>
#include <atmosphere.h>
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

signals:
    void glReady();

protected:
    void initializeGL() override;
    void paintGL() override;

    void touchEvent(QTouchEvent *ev) override;
    void mouseMoveEvent(QMouseEvent *ev) override;
    void mousePressEvent(QMouseEvent *ev) override;
    void mouseReleaseEvent(QMouseEvent *ev) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

public Q_SLOTS:
    void onAtmospherePathChanged();

private:
    QOpenGLTextureBlitter m_textureBlitter;
    CwlCompositor *m_cwlcompositor = nullptr;
    QList<QEventPoint*> m_evPoint;
    QOpenGLTexture *m_wallpaper = nullptr;
    AtmosphereModel *m_atmosphere = nullptr;

    CwlGesture *m_gesture = nullptr;
    bool m_displayOff = false;
};

QT_END_NAMESPACE

#endif //GLWINDOW_H
