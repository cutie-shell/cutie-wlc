#include <workspace.h>
#include <QDebug>

CwlWorkspace::CwlWorkspace(CwlCompositor *compositor)
{
	m_outputGeometry = compositor->defaultOutput()->geometry();
	m_outputGeometry = QRect(
		m_outputGeometry.topLeft() / compositor->scaleFactor(),
		m_outputGeometry.size() / compositor->scaleFactor()
	);
	m_availableGeometry = m_outputGeometry;

	m_viewList.clear();
	m_viewMap.clear();
}

CwlWorkspace::~CwlWorkspace()
{
}

QRect CwlWorkspace::availableGeometry()
{
	return m_availableGeometry;
}

void CwlWorkspace::removeView(CwlView *view)
{
	m_viewMap.remove(view->layer, view);
	updateViewList();

	if(view->getLayerSurface() != nullptr && view->getLayerSurface()->ls_zone > 0)
		updateAvailableGeometry();

	if(view->isToplevel()){
		emit toplevelDestroyed(view);
	}
}

void CwlWorkspace::addView(CwlView *view)
{
	bool isNew = true;

	if(m_viewMap.contains(view->layer, view)){
		m_viewMap.remove(view->layer, view);
		isNew = false;
	}

	if(m_viewMap.contains(CwlViewLayer::UNDEFINED, view)){
		m_viewMap.remove(CwlViewLayer::UNDEFINED, view);
	}

	m_viewMap.insert(m_viewMap.constEnd(), view->layer, view);

	if(view->isToplevel() && !view->isHidden())
		showDesktop(false);

	updateViewList();

	if(isNew && view->isToplevel())
		emit toplevelCreated(view);
}

QList<CwlView*> CwlWorkspace::getViews()
{
	return m_viewList;
}

QList<CwlView*> CwlWorkspace::getToplevelViews()
{
	QList tlViews = m_viewMap.values(CwlViewLayer::TOP);

	for (CwlView *view : tlViews) {
		if(!view->isToplevel())
			tlViews.removeAll(view);
	}

	return tlViews;
}

void CwlWorkspace::hideAllTopLevel()
{
	QList<CwlView*> tlViews = getToplevelViews();

	if(tlViews.empty()){
		return;
	}

	for (CwlView *view : tlViews){
		view->setHidden(true);
	}

	showDesktop(true);
}

void CwlWorkspace::showDesktop(bool show)
{
	m_showDesktop = show;
	updateViewList();
}

void CwlWorkspace::singleView(bool single)
{
	m_singleView = single;
	updateViewList();
}

void CwlWorkspace::updateViewList()
{
	QList tlViews = m_viewMap.values(CwlViewLayer::TOP);

	QList ret = m_viewMap.values(CwlViewLayer::UNDEFINED);
	ret << m_viewMap.values(CwlViewLayer::BACKGROUND);
	ret << m_viewMap.values(CwlViewLayer::BOTTOM);
	if(!m_showDesktop && !tlViews.isEmpty() && !m_singleView)
		ret << tlViews;
	else if((!m_showDesktop && !tlViews.isEmpty() && m_singleView))
		ret << tlViews.last();
	ret << m_viewMap.values(CwlViewLayer::OVERLAY);
	m_viewList = ret;
}

void CwlWorkspace::updateAvailableGeometry()
{
	QRect geometry = m_outputGeometry;

	QList<LayerSurfaceV1*> lsList = getLayerSurfaces();
	if(lsList.empty())
		return;

	for (LayerSurfaceV1* surface : lsList) {
		if(surface != nullptr && surface->initialized){
			if(surface->ls_anchor == 1){
	        	geometry.setTop(geometry.top()+surface->ls_zone);
	        } else if(surface->ls_anchor == 2){
	        	geometry.setHeight(geometry.height()-surface->ls_zone);
	        }
	        else if(surface->ls_anchor == 4){
	        	geometry.setLeft(geometry.left()+surface->ls_zone);
	        }
	        else if(surface->ls_anchor == 8){
	        	geometry.setWidth(geometry.width()-surface->ls_zone);
	        }
		}
    }

    if(geometry != m_availableGeometry){
		m_availableGeometry = geometry;
		emit availableGeometryChanged(m_availableGeometry);
    }
}

void CwlWorkspace::onLayerSurfaceDataChanged(LayerSurfaceV1 *surface)
{
	if(surface->ls_zone > 0 && surface->initialized &&
        (surface->ls_anchor == 1 ||
            surface->ls_anchor == 2 ||
            surface->ls_anchor == 4 ||
            surface->ls_anchor == 8)){
		surface->send_configure(surface->ls_serial, surface->size.width(), surface->size.height());
        updateAvailableGeometry();
    } else if (surface->ls_zone <= 0){
    	surface->send_configure(surface->ls_serial, surface->size.width(), surface->size.height());
        updateAvailableGeometry();
    }
}

QList<LayerSurfaceV1*> CwlWorkspace::getLayerSurfaces()
{
	QList<LayerSurfaceV1*> ret;
	ret.clear();
	for(CwlView* view : m_viewMap.values())
	{
		if(view->layer != CwlViewLayer::UNDEFINED && view->getLayerSurface() != nullptr && view->getLayerSurface()->ls_zone > 0 &&
	        (view->getLayerSurface()->ls_anchor == 1 ||
	            view->getLayerSurface()->ls_anchor == 2 ||
	            view->getLayerSurface()->ls_anchor == 4 ||
	            view->getLayerSurface()->ls_anchor == 8))
		{
	        ret << view->getLayerSurface();
	    }
	}
	return ret;
}