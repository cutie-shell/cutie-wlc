#ifndef CWL_WORKSPACE_H
#define CWL_WORKSPACE_H

#include <cutie-wlc.h>
#include <QList>

class CwlWorkspace : public QObject
{
    Q_OBJECT
public:
    CwlWorkspace(CwlCompositor *compositor);
    ~CwlWorkspace();

    QRect availableGeometry();

    void removeView(CwlView *view);
    void addView(CwlView *view);
    QList<CwlView*> getViews();
    QList<CwlView*> getToplevelViews();
    void hideAllTopLevel();
    void showDesktop(bool show);
    void singleView(bool single);
    int getScaleFactor();

public slots:
    void onLayerSurfaceDataChanged(LayerSurfaceV1 *surface);

signals:
	void availableGeometryChanged(QRect geometry);
    void toplevelCreated(CwlView *view);
    void toplevelDestroyed(CwlView *view);

private:
    CwlCompositor *m_compositor = nullptr;
	QRect m_availableGeometry;
	QRect m_outputGeometry;

    QList<CwlView*> m_viewList;
    QList<CwlView*> m_undefinedLayerList;
    QList<CwlView*> m_bgLayerList;
    QList<CwlView*> m_bottomLayerList;
    QList<CwlView*> m_topLayerList;
    QList<CwlView*> m_overlayLayerList;


    QList<LayerSurfaceV1*> getLayerSurfaces();

    void updateViewList();
    void updateAvailableGeometry();
    bool m_showDesktop = false;
    bool m_singleView = true;
};

#endif //CWL_WORKSPACE_H