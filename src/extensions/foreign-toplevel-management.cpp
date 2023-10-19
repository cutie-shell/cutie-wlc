#include <foreign-toplevel-management.h>

ForeignToplevelManagerV1::ForeignToplevelManagerV1(CwlCompositor *compositor)
    :QWaylandCompositorExtensionTemplate(compositor)
{
    m_compositor = compositor;
}

void ForeignToplevelManagerV1::initialize()
{
	QWaylandCompositorExtensionTemplate::initialize();
    CwlCompositor *compositor = static_cast<CwlCompositor *>(extensionContainer());
    init(compositor->display(), 3);
}

void ForeignToplevelManagerV1::zwlr_foreign_toplevel_manager_v1_bind_resource(Resource *resource)
{
	if(m_compositor->getToplevelViews().isEmpty())
		return;

	for (CwlView* view : m_compositor->getToplevelViews()) {
        ForeignToplevelHandleV1 *ftl;
		ftl = new ForeignToplevelHandleV1(resource->client(), 0, resource->version(), view, m_compositor);

		m_toplevelMap.insert(ftl, view);

		this->send_toplevel(resource->handle, ftl->resource()->handle);
		ftl->send_app_id(view->getAppId());
	    ftl->send_title(view->getTitle());
	    ftl->send_done();
    }
}

void ForeignToplevelManagerV1::zwlr_foreign_toplevel_manager_v1_destroy_resource(Resource *resource)
{
	if(m_toplevelMap.isEmpty())
		return;

	QMapIterator<ForeignToplevelHandleV1 *, CwlView *> i(m_toplevelMap);
	while (i.hasNext()) {
	    i.next();
	    if(i.key()->resource()->client() == resource->client()){
	    	m_toplevelMap.remove(i.key());
	    }
	}
}

void ForeignToplevelManagerV1::zwlr_foreign_toplevel_manager_v1_stop(Resource *resource)
{

}

void ForeignToplevelManagerV1::onToplevelCreated(CwlView *view)
{
	if(resourceMap().isEmpty())
		return;

	ForeignToplevelHandleV1 *ftl;
	ftl = new ForeignToplevelHandleV1(this->resourceMap().first()->client(), 0, this->resourceMap().first()->version(), view, m_compositor);

	m_toplevelMap.insert(ftl, view);

	QMultiMapIterator<struct ::wl_client*, Resource*> i(resourceMap());
	while (i.hasNext()) {
	    i.next();
	    this->send_toplevel(i.value()->handle, ftl->resource()->handle);
	    ftl->send_app_id(view->getAppId());
	    ftl->send_title(view->getTitle());
	    ftl->send_done();
	}
}

void ForeignToplevelManagerV1::onToplevelDestroyed(CwlView *view)
{
	if(resourceMap().isEmpty() || m_toplevelMap.isEmpty())
		return;

	QMapIterator<ForeignToplevelHandleV1 *, CwlView *> i(m_toplevelMap);
	while (i.hasNext()) {
	    i.next();
	    if(i.value() == view){
	    	i.key()->send_closed();
	    	i.key()->send_done();
	    	m_toplevelMap.remove(i.key());
	    }
	}
}

ForeignToplevelHandleV1::ForeignToplevelHandleV1(wl_client *client, uint32_t id, int version, CwlView *view, CwlCompositor *compositor)
    :QtWaylandServer::zwlr_foreign_toplevel_handle_v1(client, id, version)
{
	m_view = view;
	m_compositor = compositor;

	connect(m_view->getTopLevel(), &QWaylandXdgToplevel::titleChanged, this, &ForeignToplevelHandleV1::onToplevelTitleChanged);
}

void ForeignToplevelHandleV1::onToplevelTitleChanged()
{
	this->send_title(m_view->getTitle());
	this->send_done();
}

void ForeignToplevelHandleV1::zwlr_foreign_toplevel_handle_v1_bind_resource(Resource *resource)
{

}

void ForeignToplevelHandleV1::zwlr_foreign_toplevel_handle_v1_destroy_resource(Resource *resource)
{

}

void ForeignToplevelHandleV1::zwlr_foreign_toplevel_handle_v1_set_maximized(Resource *resource)
{

}

void ForeignToplevelHandleV1::zwlr_foreign_toplevel_handle_v1_unset_maximized(Resource *resource)
{

}

void ForeignToplevelHandleV1::zwlr_foreign_toplevel_handle_v1_set_minimized(Resource *resource)
{

}

void ForeignToplevelHandleV1::zwlr_foreign_toplevel_handle_v1_unset_minimized(Resource *resource)
{

}

void ForeignToplevelHandleV1::zwlr_foreign_toplevel_handle_v1_activate(Resource *resource, struct ::wl_resource *seat)
{
	m_compositor->raise(m_view);
}

void ForeignToplevelHandleV1::zwlr_foreign_toplevel_handle_v1_close(Resource *resource)
{
	m_view->getTopLevel()->sendClose();
}

void ForeignToplevelHandleV1::zwlr_foreign_toplevel_handle_v1_set_rectangle(Resource *resource, struct ::wl_resource *surface, int32_t x, int32_t y, int32_t width, int32_t height)
{

}

void ForeignToplevelHandleV1::zwlr_foreign_toplevel_handle_v1_destroy(Resource *resource)
{

}

void ForeignToplevelHandleV1::zwlr_foreign_toplevel_handle_v1_set_fullscreen(Resource *resource, struct ::wl_resource *output)
{

}

void ForeignToplevelHandleV1::zwlr_foreign_toplevel_handle_v1_unset_fullscreen(Resource *resource)
{

}