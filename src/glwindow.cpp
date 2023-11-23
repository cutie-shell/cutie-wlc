#include <glwindow.h>

#include <QPainter>
#include <QMatrix4x4>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QMouseEvent>

#include <QGuiApplication>
#include <qpa/qplatformnativeinterface.h>
#include <QtWaylandCompositor/QWaylandSeat>

GlWindow::GlWindow()
: m_atmosphere(new AtmosphereModel()) {
    QGuiApplication::platformNativeInterface()->nativeResourceForIntegration("displayon");
    m_displayOff = false;
    connect(m_atmosphere, &AtmosphereModel::pathChanged, this, &GlWindow::onAtmospherePathChanged);
}

void GlWindow::setCompositor(CwlCompositor *cwlcompositor)
{
    m_cwlcompositor = cwlcompositor;
    m_gesture = new CwlGesture(cwlcompositor, QSize(width(), height()));
}

void GlWindow::initializeGL()
{
    m_textureBlitter.create();
    onAtmospherePathChanged();
    emit glReady();
}

void GlWindow::onAtmospherePathChanged() {
    if (m_wallpaper) delete m_wallpaper;
    m_wallpaper = new QOpenGLTexture(QImage(m_atmosphere->path() + "/wallpaper.jpg"));
}

void GlWindow::paintGL()
{
    m_cwlcompositor->startRender();

    QOpenGLFunctions *functions = context()->functions();
    GLenum currentTarget = GL_TEXTURE_2D;
    m_textureBlitter.bind(currentTarget);
    functions->glEnable(GL_BLEND);
    functions->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    double wallAspect = 1.0 * m_wallpaper->width() / m_wallpaper->height();
    double screenAspect = 1.0 * width() / height();
    QMatrix4x4 targetTransform;
    QMatrix3x3 sourceTransform;
    qDebug() << wallAspect << " : " << screenAspect;
    if (wallAspect > screenAspect) {
	int tgtWidth = m_wallpaper->height() * width() / height();
        targetTransform = QOpenGLTextureBlitter::targetTransform(
            QRect(QPoint(), QSize(tgtWidth, m_wallpaper->height())),
            QRect(QPoint(), size() * m_wallpaper->height() / height()));
        sourceTransform = QOpenGLTextureBlitter::sourceTransform(
            QRect(QPoint((m_wallpaper->width() - tgtWidth) / 2, 0), QSize(tgtWidth, m_wallpaper->height())),
            QSize(m_wallpaper->width(), m_wallpaper->height()), QOpenGLTextureBlitter::OriginTopLeft);
    } else {
	int tgtHeight = m_wallpaper->width() * height() / width();
        targetTransform = QOpenGLTextureBlitter::targetTransform(
            QRect(QPoint(), QSize(m_wallpaper->width(), tgtHeight)),
            QRect(QPoint(), size() * m_wallpaper->width() / width()));
        sourceTransform = QOpenGLTextureBlitter::sourceTransform(
            QRect(QPoint(0, (m_wallpaper->height() - tgtHeight) / 2), QSize(m_wallpaper->width(), tgtHeight)),
            QSize(m_wallpaper->width(), m_wallpaper->height()), QOpenGLTextureBlitter::OriginTopLeft);
    }
    m_textureBlitter.setOpacity(1.0);
    m_textureBlitter.blit(m_wallpaper->textureId(), targetTransform, sourceTransform);

    int counter = 0;

    QList<CwlView*> renderViews;
    
    if(m_cwlcompositor->m_launcherView != nullptr)
        renderViews = m_cwlcompositor->getViews()<<m_cwlcompositor->m_launcherView;
    else
        renderViews = m_cwlcompositor->getViews();

    for (CwlView *view : renderViews) {
        QString appId;
        if(view != nullptr && view->isToplevel())
            appId = view->getAppId();

        if (appId == "cutie-launcher")
            if (!m_cwlcompositor->launcherClosed && !m_cwlcompositor->launcherOpened)
                m_textureBlitter.setOpacity(1.0 - (m_cwlcompositor->m_launcherView->getPosition().y()/height()));
            else if (m_cwlcompositor->launcherOpened)
                m_textureBlitter.setOpacity(1.0);
            else m_textureBlitter.setOpacity(0.0);
        else if (view->isToplevel())
            if (!m_cwlcompositor->launcherClosed && !m_cwlcompositor->launcherOpened)
                m_textureBlitter.setOpacity((1.0 - m_cwlcompositor->homeOpen) * m_cwlcompositor->m_launcherView->getPosition().y()/height());
            else if (m_cwlcompositor->getTopPanel() != nullptr)
                if(m_cwlcompositor->getTopPanel()->panelState > 1)
                    m_textureBlitter.setOpacity(0.0);
                else m_textureBlitter.setOpacity(1.0 - m_cwlcompositor->homeOpen);
            else {
                m_textureBlitter.setOpacity(1.0 - m_cwlcompositor->homeOpen);
            } 
        else if (view == m_cwlcompositor->getHomeView())
            m_textureBlitter.setOpacity(m_cwlcompositor->homeOpen);
        else m_textureBlitter.setOpacity(1.0);

        if(m_cwlcompositor->launcherOpened && view->layer == 2)
            continue;

        QOpenGLTexture *texture = view->getTexture();
        if (!texture)
            continue;
        if (texture->target() != currentTarget) {
            currentTarget = texture->target();
            m_textureBlitter.bind(currentTarget);
        }

        QWaylandSurface *surface = view->surface();
        if (surface && surface->hasContent()) {
            QSize viewSize = view->size();
            QPointF viewPosition = view->getPosition();
            auto surfaceOrigin = view->textureOrigin();
            QRectF targetRect(
                viewPosition * m_cwlcompositor->scaleFactor(),
                viewSize * m_cwlcompositor->scaleFactor());

            QMatrix4x4 targetTransform = QOpenGLTextureBlitter::targetTransform(targetRect, QRect(QPoint(), size()));
            m_textureBlitter.blit(texture->textureId(), targetTransform, surfaceOrigin);
        }

        for (CwlView *childView : view->getChildViews()) {
            QOpenGLTexture *texture = childView->getTexture();
            if (!texture)
                continue;
            if (texture->target() != currentTarget) {
                currentTarget = texture->target();
                m_textureBlitter.bind(currentTarget);
            }

            QWaylandSurface *surface = childView->surface();
            if (surface && surface->hasContent()) {
                QSize viewSize = childView->size();
                QPointF viewPosition = childView->getPosition();
                auto surfaceOrigin = childView->textureOrigin();
                QRectF targetRect(
                        viewPosition * m_cwlcompositor->scaleFactor(),
                        viewSize * m_cwlcompositor->scaleFactor());
            
                QMatrix4x4 targetTransform = QOpenGLTextureBlitter::targetTransform(targetRect, QRect(QPoint(), size()));
                m_textureBlitter.blit(texture->textureId(), targetTransform, surfaceOrigin);
            }
        }
    }

    m_textureBlitter.release();
    m_cwlcompositor->endRender();
}

void GlWindow::touchEvent(QTouchEvent *ev)
{
    m_gesture->handlePointerEvent(ev, [this](QPointerEvent* ev){
        m_cwlcompositor->handleTouchEvent(static_cast<QTouchEvent*>(ev));
    });
}

void GlWindow::mouseMoveEvent(QMouseEvent *ev)
{
    m_gesture->handlePointerEvent(ev, [this](QPointerEvent* ev){
        m_cwlcompositor->handleMouseMoveEvent(static_cast<QMouseEvent*>(ev));
    });
}

void GlWindow::mousePressEvent(QMouseEvent *ev)
{
    m_gesture->handlePointerEvent(ev, [this](QPointerEvent* ev){
        m_cwlcompositor->handleMousePressEvent(static_cast<QMouseEvent*>(ev));
    });
}

void GlWindow::mouseReleaseEvent(QMouseEvent *ev)
{
    m_gesture->handlePointerEvent(ev, [this](QPointerEvent* ev){
        m_cwlcompositor->handleMouseReleaseEvent(static_cast<QMouseEvent*>(ev));
    });
}

void GlWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_PowerOff)
        qDebug()<<"POWER KEY PRESS";
}

void GlWindow::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_PowerOff){
        if(m_cwlcompositor->getHomeView() != nullptr){
            QWaylandSurface *focusSurface = m_cwlcompositor->defaultSeat()->keyboardFocus();

            m_cwlcompositor->defaultSeat()->setKeyboardFocus(m_cwlcompositor->getHomeView()->surface());
            m_cwlcompositor->defaultSeat()->sendFullKeyEvent(event);

            if(focusSurface != nullptr){
                m_cwlcompositor->defaultSeat()->setKeyboardFocus(focusSurface);
                m_cwlcompositor->defaultSeat()->sendFullKeyEvent(event);
            }
        }

        //Can be removed later when the shell handles the power key;
        if(m_displayOff){
            QGuiApplication::platformNativeInterface()->nativeResourceForIntegration("displayon");
            m_displayOff = false;
            this->requestUpdate();
        } else {
            QGuiApplication::platformNativeInterface()->nativeResourceForIntegration("displayoff");
            m_displayOff = true;
        }
    }
}
