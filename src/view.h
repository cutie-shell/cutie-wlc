#ifndef CWL_VIEW_H
#define CWL_VIEW_H

#include <QtWaylandCompositor/QWaylandCompositor>
#include <QtWaylandCompositor/QWaylandSurface>
#include <QtWaylandCompositor/QWaylandView>
#include <QtWaylandCompositor/QWaylandXdgSurface>
#include <QTimer>
#include <QOpenGLTextureBlitter>

#include <layer-shell.h>

QT_BEGIN_NAMESPACE

class QOpenGLTexture;
class QWaylandXdgShell;
class CwlCompositor;

enum CwlViewLayer: uint32_t
{
    BACKGROUND      = 0,
    BOTTOM          = 1,
    TOP             = 2,
    OVERLAY         = 3,
    UNDEFINED       = 4
};

class CwlView : public QWaylandView
{
    Q_OBJECT
public:
    CwlView(CwlCompositor *cwlcompositor);
    ~CwlView();

    QOpenGLTexture *getTexture();
    QOpenGLTextureBlitter::Origin textureOrigin();
    QPointF getPosition();
    void setPosition(const QPointF &pos);
    QSize size();

    CwlViewLayer layer = UNDEFINED;

    bool isToplevel();
    LayerSurfaceV1 *getLayerSurface();
    bool isHidden();
    void setHidden(bool hide);

    QWaylandXdgToplevel *getTopLevel();

    QList<CwlView*> getChildViews();
    void removeChildView(CwlView *view);
    void addChildView(CwlView *view);
    CwlView* parentView();
    void setParentView(CwlView* view);

    QString getAppId();
    QString getTitle();

signals:

protected:

private:
    friend class CwlCompositor;
    CwlCompositor *m_cwlcompositor = nullptr;
    GLenum m_textureTarget = GL_TEXTURE_2D;
    QOpenGLTexture *m_texture = nullptr;
    QOpenGLTextureBlitter::Origin m_origin;
    QPointF m_position;
    QSize m_size;
    QWaylandXdgSurface *m_xdgSurface = nullptr;
    QWaylandXdgPopup *m_xdgPopup = nullptr;
    QWaylandXdgToplevel *m_toplevel = nullptr;
    LayerSurfaceV1 *m_layerSurface = nullptr;
    CwlView *m_parentView = nullptr;
    bool m_hidden = false;
    QList<CwlView*> m_childViewList;

public slots:
    void onAvailableGeometryChanged(QRect geometry);
};

QT_END_NAMESPACE

#endif //VIEW_H