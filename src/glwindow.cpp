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

    for (CwlView *view : m_cwlcompositor->getViews()) {
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
    if(ev->isBeginEvent() && ev->points().size() == 1 && ev->points().first().globalPosition().x() > width() - 150){
        m_evPoint.clear();
        QEventPoint *evP = new QEventPoint(ev->points().first());
        m_evPoint << evP;
    } else if (ev->isUpdateEvent() && ev->points().size() == 1 && !m_evPoint.empty()){
        QEventPoint *evP = new QEventPoint(ev->points().first());
        m_evPoint << evP;
    } else if (ev->isEndEvent() && ev->points().size() == 1 && !m_evPoint.empty() && ev->points().first().globalPosition().x() < width()*0.49) {
        m_evPoint.clear();
        m_appswitcher->activate();
        requestUpdate();
    } else if (ev->isEndEvent() && ev->points().size() == 1 && !m_evPoint.empty()) {
        QEventPoint *evP = new QEventPoint(ev->points().first());
        m_evPoint << evP;
        for (QEventPoint *evP : m_evPoint) {
            m_cwlcompositor->handleTouchPointEvent(evP);
        }
        m_evPoint.clear();
    } else {
        if(!m_evPoint.empty())
            m_evPoint.clear();
        
        m_cwlcompositor->handleTouchEvent(ev);
    }
}