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
	if(!m_active || m_toplevelViews.empty())
		updateViewMap();

	return m_toplevelViews;
}

void CwlAppswitcher::activate()
{
	m_workspace->singleView(false);
	m_workspace->showDesktop(false);

	updateViewMap();

	if(m_toplevelViews.empty())
		return;

	m_active = true;
}

void CwlAppswitcher::deactivate()
{
	m_workspace->singleView(true);
	m_active = false;
}

bool CwlAppswitcher::isActive()
{
	return m_active;
}

void CwlAppswitcher::updateViewMap()
{
	int itemCount = 0;
	int x;
	int y;
	int views;
	QPointF newPos;
	QSizeF newSize;

	m_toplevelViews.clear();

	QList<CwlView*> tlViews = m_workspace->getToplevelViews();

	if(tlViews.empty()){
		m_active = false;
		return;
	}

	for (CwlView *view : tlViews) {
		views = m_workspace->getToplevelViews().size();

		if(views == 1){
			newSize = m_workspace->availableGeometry().size() * 0.8;
			newPos = QPointF(m_workspace->availableGeometry().size().width()*0.1, m_workspace->availableGeometry().size().height()*0.1);
		} else if(views == 2){
			itemCount += 1;
			newSize = m_workspace->availableGeometry().size() * 0.46;

			if(itemCount == 1){
				x = (m_workspace->availableGeometry().size().width() - 2*newSize.width())/3;
				y = m_workspace->availableGeometry().size().height()/2 - newSize.height()/2;
			}
			else{
				x = (m_workspace->availableGeometry().size().width() - 2*newSize.width())/3*2 + newSize.width();
				y = m_workspace->availableGeometry().size().height()/2 - newSize.height()/2;
			}

			newPos = QPointF(x, y);
		} else if(views == 3) {
			itemCount += 1;
			newSize = m_workspace->availableGeometry().size() * 0.46;

			if(itemCount == 1){
				x = (m_workspace->availableGeometry().size().width() - 2*newSize.width())/3;
				y = (m_workspace->availableGeometry().size().height() - 2*newSize.height())/3;
			}
			else if(itemCount == 2){
				x = (m_workspace->availableGeometry().size().width() - 2*newSize.width())/3*2 + newSize.width();
				y = (m_workspace->availableGeometry().size().height() - 2*newSize.height())/3;
			}
			else if(itemCount == 3){
				x = m_workspace->availableGeometry().size().width()/2 - newSize.width()/2;
				y = (m_workspace->availableGeometry().size().height() - 2*newSize.height())/3*2 + newSize.height();
			}

			newPos = QPointF(x, y);
		} else if(views == 4) {
			itemCount += 1;
			newSize = m_workspace->availableGeometry().size() * 0.46;

			if(itemCount == 1){
				x = (m_workspace->availableGeometry().size().width() - 2*newSize.width())/3;
				y = (m_workspace->availableGeometry().size().height() - 2*newSize.height())/3;
			}
			else if(itemCount == 2){
				x = (m_workspace->availableGeometry().size().width() - 2*newSize.width())/3*2 + newSize.width();
				y = (m_workspace->availableGeometry().size().height() - 2*newSize.height())/3;
			}
			else if(itemCount == 3){
				x = (m_workspace->availableGeometry().size().width() - 2*newSize.width())/3;
				y = (m_workspace->availableGeometry().size().height() - 2*newSize.height())/3*2 + newSize.height();
			} else if(itemCount == 4){
				x = (m_workspace->availableGeometry().size().width() - 2*newSize.width())/3*2 + newSize.width();
				y = (m_workspace->availableGeometry().size().height() - 2*newSize.height())/3*2 + newSize.height();
			}

			newPos = QPointF(x, y);
		}

		m_toplevelViews.insert(view, QRectF(newPos, newSize));
	}
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