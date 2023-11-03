#include <appswitcher.h>
#include <workspace.h>
#include <QDebug>

CwlAppswitcher::CwlAppswitcher(CwlWorkspace *workspace)
{
	m_workspace = workspace;
}

CwlAppswitcher::~CwlAppswitcher()
{
}

void CwlAppswitcher::updateViewGeometry(CwlView* view, QRectF geometry)
{
	m_toplevelViews.insert(view, geometry);
}

QMap<CwlView*, QRectF> CwlAppswitcher::getRecentViews()
{
	return m_toplevelViews;
}

void CwlAppswitcher::activate()
{
	if(m_workspace->getToplevelViews().size() < 1)
		return;

	m_workspace->singleView(false);
	m_workspace->showDesktop(false);

	m_active = true;

	animationOpen();
}

void CwlAppswitcher::update()
{
	if(!m_active)
		return;

	m_workspace->singleView(false);
	m_workspace->showDesktop(false);

	animationUpdate();
}

void CwlAppswitcher::deactivate()
{
	m_workspace->singleView(true);
	m_active = false;
	m_toplevelViews.clear();
}

bool CwlAppswitcher::isActive()
{
	return m_active;
}

CwlView* CwlAppswitcher::findViewAt(QPointF point)
{
	QMapIterator<CwlView*, QRectF> i(m_toplevelViews);
	CwlView* view = nullptr;
	while (i.hasNext()){
		i.next();

		if (i.value().contains(point))
			return i.key();
	}

	return view;
}

void CwlAppswitcher::animationOpen()
{
	QList<CwlView*> tlViews = m_workspace->getToplevelViews();

	if(tlViews.empty()){
		deactivate();
		m_animationFactor = 0.0;
		animationRunning = false;
		return;
	}

	if(!animationRunning)
		animationRunning = true;

	m_animationFactor += 0.1;

	if(m_animationFactor > 1.0)
			m_animationFactor = 1.0;

	int count = 0;
	int row = 0;

	QPointF centerScreen = QPointF(m_workspace->availableGeometry().size().width() / 2,
									m_workspace->availableGeometry().size().height() / 2);

	m_toplevelViews.clear();

	for (CwlView *view : tlViews) {

		count += 1;
		row = qCeil((float)count/2);

		QSize finalSize = QSize((m_workspace->availableGeometry().size().width() - m_gridSpacing * 3) / 2,
							m_workspace->availableGeometry().size().height() / m_workspace->availableGeometry().size().width() * 
							m_workspace->availableGeometry().size().width() / 2 - 3 * m_gridSpacing);

		finalSize = finalSize * m_workspace->getScaleFactor();

		int xPos = m_gridSpacing + finalSize.width() / 2;
		int yPos = m_gridSpacing + m_workspace->availableGeometry().top() + finalSize.height() / 2;

		if((count % 2) == 0)
			xPos += finalSize.width() + m_gridSpacing;

		if(row > 1)
			yPos += finalSize.height() * qCeil((float)row/2) + qCeil((float)row/2) * m_gridSpacing;

		QPointF finalPosition = QPointF(xPos, yPos);

		QRectF currentGeometry(view->getPosition(), view->size());
		QRectF finalGeometry(finalPosition, finalSize);
		
		if(m_animationFactor == 1.0){
			finalGeometry.moveCenter(finalPosition);
			m_toplevelViews.insert(view, finalGeometry);
			if(view->getChildViews().size()>0){
                for (CwlView *childView : view->getChildViews()) {
                    if(!childView->isToplevel())
                        continue;
					QRectF childGeom = childView->getTopLevel()->xdgSurface()->windowGeometry();
                    childGeom.setSize(childGeom.size() * m_animationFactor);
                    if(childGeom.width() > finalGeometry.width()){
                    	double aspect = childGeom.width() / childGeom.height();
                    	childGeom.setWidth(finalGeometry.size().width());
                    	childGeom.setHeight(childGeom.width() / aspect);
                    }
                    childGeom.moveCenter(finalGeometry.center());
                    m_toplevelViews.insert(childView, childGeom);
                }
            }
		} else {
			qreal newX;
			qreal newY;

			if(centerScreen.x() > finalPosition.x())
				newX = centerScreen.x() - (centerScreen.x() - finalPosition.x()) * m_animationFactor;
			else
				newX = centerScreen.x() + (finalPosition.x() - centerScreen.x()) * m_animationFactor;

			if(centerScreen.y() > finalPosition.y())
				newY = centerScreen.y() - (centerScreen.y() - finalPosition.y()) * m_animationFactor;
			else
				newY = centerScreen.y() + (finalPosition.y() - centerScreen.y()) * m_animationFactor;

			finalGeometry.setSize(finalSize*m_animationFactor);
			finalGeometry.moveCenter(QPointF(newX, newY));
			m_toplevelViews.insert(view, finalGeometry);
			if(view->getChildViews().size()>0){
                for (CwlView *childView : view->getChildViews()) {
                    if(!childView->isToplevel())
                        continue;
                    QRectF childGeom = childView->getTopLevel()->xdgSurface()->windowGeometry();
                    childGeom.setSize(childGeom.size() * m_animationFactor);
                    if(childGeom.width() > finalGeometry.width()){
                    	double aspect = childGeom.width() / childGeom.height();
                    	childGeom.setWidth(finalGeometry.size().width());
                    	childGeom.setHeight(childGeom.width() / aspect);
                    }
                    childGeom.moveCenter(finalGeometry.center());
                    m_toplevelViews.insert(childView, childGeom);
                }
            }
		}
		emit redraw();
	}

	if(m_animationFactor != 1.0){
		QTimer::singleShot(20, this, &CwlAppswitcher::animationOpen);
	} else {
		m_animationFactor = 0.0;
		animationRunning = false;
		emit redraw();
	}
}

void CwlAppswitcher::animationUpdate()
{
	QList<CwlView*> tlViews = m_workspace->getToplevelViews();

	if(tlViews.empty()){
		deactivate();
		m_animationFactor = 0.0;
		animationRunning = false;
		return;
	}

	QMapIterator<CwlView*, QRectF> i(m_toplevelViews);
	while (i.hasNext()){
		i.next();
		if(!tlViews.contains(i.key())){
			m_toplevelViews.remove(i.key());
		}
	}

	if(!animationRunning)
		animationRunning = true;

	m_animationFactor += 0.05;

	if(m_animationFactor > 1.0)
			m_animationFactor = 1.0;

	int count = 0;
	int row = 0;

	QPointF currentPosition;

	for (CwlView *view : tlViews) {
		if(view == nullptr)
			continue;

		currentPosition = m_toplevelViews.value(view).center();

		count += 1;
		row = qCeil((float)count/2);

		QSize finalSize = QSize((m_workspace->availableGeometry().size().width() - m_gridSpacing * 3) / 2,
							m_workspace->availableGeometry().size().height() / m_workspace->availableGeometry().size().width() * 
							m_workspace->availableGeometry().size().width() / 2 - 3 * m_gridSpacing);

		finalSize = finalSize * m_workspace->getScaleFactor();
		
		int xPos = m_gridSpacing + finalSize.width() / 2;
		int yPos = m_gridSpacing + m_workspace->availableGeometry().top() + finalSize.height() / 2;

		if((count % 2) == 0)
			xPos += finalSize.width() + m_gridSpacing;

		if(row > 1)
			yPos += finalSize.height() * qCeil((float)row/2) + qCeil((float)row/2) * m_gridSpacing;

		QPointF finalPosition = QPointF(xPos, yPos);

		QRectF currentGeometry(view->getPosition(), view->size());
		QRectF finalGeometry(finalPosition, finalSize);
		
		if(m_animationFactor == 1.0){
			finalGeometry.moveCenter(finalPosition);
			m_toplevelViews.insert(view, finalGeometry);
			if(view->getChildViews().size()>0){
                for (CwlView *childView : view->getChildViews()) {
                    if(!childView->isToplevel())
                        continue;
                    QRectF childGeom = childView->getTopLevel()->xdgSurface()->windowGeometry();
                    childGeom.setSize(childGeom.size() * m_animationFactor);
                    if(childGeom.width() > finalGeometry.width()){
                    	double aspect = childGeom.width() / childGeom.height();
                    	childGeom.setWidth(finalGeometry.size().width());
                    	childGeom.setHeight(childGeom.width() / aspect);
                    }
                    childGeom.moveCenter(finalGeometry.center());
                    m_toplevelViews.insert(childView, childGeom);
                }
            }
		} else {
			qreal newX;
			qreal newY;

			if(currentPosition.x() > finalPosition.x())
				newX = currentPosition.x() - (currentPosition.x() - finalPosition.x()) * m_animationFactor;
			else if(currentPosition.x() < finalPosition.x())
				newX = currentPosition.x() + (finalPosition.x() - currentPosition.x()) * m_animationFactor;
			else
				newX = currentPosition.x();

			if(currentPosition.y() > finalPosition.y())
				newY = currentPosition.y() - (currentPosition.y() - finalPosition.y()) * m_animationFactor;
			else if(currentPosition.y() < finalPosition.y())
				newY = currentPosition.y() + (finalPosition.y() - currentPosition.y()) * m_animationFactor;
			else
				newY = currentPosition.y();

			finalGeometry.setSize(finalSize);
			finalGeometry.moveCenter(QPointF(newX, newY));
			m_toplevelViews.insert(view, finalGeometry);
			if(view->getChildViews().size()>0){
                for (CwlView *childView : view->getChildViews()) {
                    if(!childView->isToplevel())
                        continue;
                    QRectF childGeom = childView->getTopLevel()->xdgSurface()->windowGeometry();
                    childGeom.setSize(childGeom.size() * m_animationFactor);
                    if(childGeom.width() > finalGeometry.width()){
                    	double aspect = childGeom.width() / childGeom.height();
                    	childGeom.setWidth(finalGeometry.size().width());
                    	childGeom.setHeight(childGeom.width() / aspect);
                    }
                    childGeom.moveCenter(finalGeometry.center());
                    m_toplevelViews.insert(childView, childGeom);
                }
            }
		}
		emit redraw();
	}

	if(m_animationFactor != 1.0){
		QTimer::singleShot(40, this, &CwlAppswitcher::animationUpdate);
	} else {
		m_animationFactor = 0.0;
		animationRunning = false;
		emit redraw();
	}
}