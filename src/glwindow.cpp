#include <glwindow.h>

#include <QPainter>
#include <QMatrix4x4>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QMouseEvent>

GlWindow::GlWindow()
{
}

void GlWindow::setCompositor(CwlCompositor *cwlcompositor)
{
    m_cwlcompositor = cwlcompositor;
    m_gesture = new CwlGesture(cwlcompositor, QSize(width(), height()));
}

void GlWindow::setAppswitcher(CwlAppswitcher *appswitcher)
{
    m_appswitcher = appswitcher;
}

void GlWindow::initializeGL()
{
    m_textureBlitter.create();
    emit glReady();
}

void GlWindow::paintGL()
{
    m_cwlcompositor->startRender();

    QOpenGLFunctions *functions = context()->functions();
    functions->glClearColor(1.f, .6f, .0f, 0.5f);
    functions->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLenum currentTarget = GL_TEXTURE_2D;
    m_textureBlitter.bind(currentTarget);
    functions->glEnable(GL_BLEND);
    functions->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    int counter = 0;

    QList<CwlView*> renderViews;

    if(!m_cwlcompositor->launcherOpened && m_cwlcompositor->launcherClosed){
        renderViews = m_cwlcompositor->getViews();
    } else if(!m_cwlcompositor->launcherClosed && m_cwlcompositor->launcherOpened){
        renderViews = renderViews<<m_cwlcompositor->m_launcherView;
    } else if(!m_cwlcompositor->launcherClosed && !m_cwlcompositor->launcherOpened){
        renderViews = m_cwlcompositor->getViews()<<m_cwlcompositor->m_launcherView;
    }

    for (CwlView *view : renderViews) {
        QString appId;
        if(view != nullptr && view->isToplevel())
            appId = view->getTopLevel()->appId();

        if(!m_cwlcompositor->launcherClosed && !m_cwlcompositor->launcherOpened){
            if(appId == "cutie-launcher"){
                m_textureBlitter.setOpacity(1.0 - (m_cwlcompositor->m_launcherView->getPosition().y()/height()));
            } else {
                m_textureBlitter.setOpacity(m_cwlcompositor->m_launcherView->getPosition().y()/height());
            }
        } else {
            m_textureBlitter.setOpacity(1.0);
        }
        
        QOpenGLTexture *texture = view->getTexture();
        if (!texture)
            continue;
        if (texture->target() != currentTarget) {
            currentTarget = texture->target();
            m_textureBlitter.bind(currentTarget);
        }

        QWaylandSurface *surface = view->surface();
        if (surface && surface->hasContent()) {
            QSize s = view->size();
            QPointF pos = view->getPosition();
            QRectF surfaceGeometry(pos, s);
            auto surfaceOrigin = view->textureOrigin();
            QRectF targetRect;

            if(m_appswitcher != nullptr && m_appswitcher->isActive() && m_appswitcher->getRecentViews().contains(view)){
                QRectF geom = m_appswitcher->getRecentViews().value(view);
                targetRect = QRectF(geom.topLeft(), geom.size());
            } else {
                targetRect = QRectF(surfaceGeometry.topLeft(), surfaceGeometry.size());
            }

            QMatrix4x4 targetTransform = QOpenGLTextureBlitter::targetTransform(targetRect, QRect(QPoint(), size()));
            m_textureBlitter.blit(texture->textureId(), targetTransform, surfaceOrigin);
        }
    }

    m_textureBlitter.release();
    m_cwlcompositor->endRender();
}

void GlWindow::touchEvent(QTouchEvent *ev)
{
    m_gesture->handleTouchEvent(ev);
}