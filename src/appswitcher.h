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
    void deactivate();

    void updateViewMap();

    CwlView* findViewAt(QPointF point);

public slots:

signals:
    void redraw();
    
private:
    CwlWorkspace *m_workspace;
    QMap<CwlView*, QRectF> m_toplevelViews;
    bool m_active = false;
};

#endif //CWL_APPSWITCHER_H