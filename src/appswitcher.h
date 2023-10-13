#ifndef CWL_APPSWITCHER_H
#define CWL_APPSWITCHER_H

#include <workspace.h>
#include <QList>

class CwlWorkspace;

class CwlAppswitcher : public QObject
{
    Q_OBJECT
public:
    CwlAppswitcher(CwlWorkspace *workspace);
    ~CwlAppswitcher();

    void updateViewGeometry(CwlView* view, QRectF geometry);
    QMap<CwlView*, QRectF> getRecentViews();

    bool isActive();

    void activate();
    void update();
    void deactivate();

    void updateViewMap();

    CwlView* findViewAt(QPointF point);

    bool animationRunning = false;

public slots:

private slots:
    void animationOpen();
    void animationUpdate();

signals:
    void redraw();
    
private:
    CwlWorkspace *m_workspace;
    QMap<CwlView*, QRectF> m_toplevelViews;
    bool m_active = false;
    qreal m_animationFactor = 0.0;
    qreal m_gridSpacing = 40.0;
};

#endif //CWL_APPSWITCHER_H