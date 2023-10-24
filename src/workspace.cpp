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
	m_bgLayerList.clear();
    m_bottomLayerList.clear();
    m_topLayerList.clear();
    m_overlayLayerList.clear();
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
	if(view->layer == CwlViewLayer::UNDEFINED){
		m_undefinedLayerList.removeAll(view);
	} else if(view->layer == CwlViewLayer::BACKGROUND){
		m_bgLayerList.removeAll(view);
	} else if(view->layer == CwlViewLayer::BOTTOM){
		m_bottomLayerList.removeAll(view);
	} else if(view->layer == CwlViewLayer::TOP){
		m_topLayerList.removeAll(view);
	} else if(view->layer == CwlViewLayer::OVERLAY){
		m_overlayLayerList.removeAll(view);
	}
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

	if(m_undefinedLayerList.contains(view))
		m_undefinedLayerList.removeAll(view);

	if(view->layer == CwlViewLayer::UNDEFINED){
		m_undefinedLayerList<<view;
	}

	if(view->layer == CwlViewLayer::BACKGROUND){
		if(m_bgLayerList.contains(view))
			m_bgLayerList.removeAll(view);
		m_bgLayerList<<view;
	} else if(view->layer == CwlViewLayer::BOTTOM){
		if(m_bottomLayerList.contains(view))
			m_bottomLayerList.removeAll(view);
		m_bottomLayerList<<view;
	} else if(view->layer == CwlViewLayer::TOP){
		if(m_topLayerList.contains(view))
			m_topLayerList.removeAll(view);
		m_topLayerList<<view;
	} else if(view->layer == CwlViewLayer::OVERLAY){
		if(m_overlayLayerList.contains(view))
			m_overlayLayerList.removeAll(view);
		m_overlayLayerList<<view;
	}

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
	return m_topLayerList;
}

void CwlWorkspace::hideAllTopLevel()
{
	if(m_topLayerList.empty()){
		return;
	}

	for (CwlView *view : m_topLayerList){
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
	QList ret = m_bgLayerList;
	ret << m_undefinedLayerList;
	ret << m_bottomLayerList;

	if(!m_showDesktop && !m_topLayerList.isEmpty() && !m_singleView)
		ret << m_topLayerList;
	else if((!m_showDesktop && !m_topLayerList.isEmpty() && m_singleView))
		ret << m_topLayerList.last();

	ret << m_overlayLayerList;
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
	if(surface->ls_zone > 0 && surface->initialized){
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
	for(CwlView* view : m_viewList)
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