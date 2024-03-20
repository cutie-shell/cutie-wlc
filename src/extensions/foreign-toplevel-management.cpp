#include <foreign-toplevel-management.h>
#include <QtWaylandCompositor/QWaylandSeat>

ForeignToplevelManagerV1::ForeignToplevelManagerV1(CwlCompositor *compositor)
	: QWaylandCompositorExtensionTemplate(compositor)
{
	m_compositor = compositor;
}

void ForeignToplevelManagerV1::initialize()
{
	QWaylandCompositorExtensionTemplate::initialize();
	CwlCompositor *compositor =
		static_cast<CwlCompositor *>(extensionContainer());
	init(compositor->display(), 3);
}

void ForeignToplevelManagerV1::zwlr_foreign_toplevel_manager_v1_bind_resource(
	Resource *resource)
{
	if (m_compositor->getToplevelViews().isEmpty())
		return;

	QList<ForeignToplevelHandleV1 *> ftls;
	for (CwlView *view : m_compositor->getToplevelViews()) {
		ForeignToplevelHandleV1 *ftl = new ForeignToplevelHandleV1(
			resource->client(), 0, resource->version(), view,
			m_compositor);
		ftls.append(ftl);

		this->send_toplevel(resource->handle, ftl->resource()->handle);
		ftl->send_app_id(view->getAppId());
		ftl->send_title(view->getTitle());
		ftl->send_done();
	}

	m_toplevelMap.insert(resource->client(), ftls);
}

void ForeignToplevelManagerV1::zwlr_foreign_toplevel_manager_v1_destroy_resource(
	Resource *resource)
{
	m_toplevelMap.remove(resource->client());
}

void ForeignToplevelManagerV1::zwlr_foreign_toplevel_manager_v1_stop(
	Resource *resource)
{
}

void ForeignToplevelManagerV1::onToplevelCreated(CwlView *view)
{
	if (resourceMap().isEmpty())
		return;

	QMultiMapIterator<struct ::wl_client *, Resource *> i(resourceMap());
	while (i.hasNext()) {
		i.next();
		ForeignToplevelHandleV1 *ftl = new ForeignToplevelHandleV1(
			i.key(), 0, i.value()->version(), view, m_compositor);
		QList<ForeignToplevelHandleV1 *> list = m_toplevelMap[i.key()];
		list.append(ftl);
		m_toplevelMap.insert(i.key(), list);
		send_toplevel(i.value()->handle, ftl->resource()->handle);
		ftl->send_app_id(view->getAppId());
		ftl->send_title(view->getTitle());
		ftl->send_done();
	}
}

void ForeignToplevelManagerV1::onToplevelDestroyed(CwlView *view)
{
	if (resourceMap().isEmpty() || m_toplevelMap.isEmpty())
		return;

	QMapIterator<struct ::wl_client *, QList<ForeignToplevelHandleV1 *> > i(
		m_toplevelMap);
	while (i.hasNext()) {
		i.next();
		QList<ForeignToplevelHandleV1 *> list = i.value();
		for (ForeignToplevelHandleV1 *value : list) {
			if (value->view() == view) {
				value->send_closed();
				value->send_done();
				list.removeAll(value);
			}
		}
		m_toplevelMap.insert(i.key(), list);
	}
}

ForeignToplevelHandleV1 *
ForeignToplevelManagerV1::handleForView(struct ::wl_client *client,
					CwlView *view)
{
	QList<ForeignToplevelHandleV1 *> list = m_toplevelMap[client];
	for (ForeignToplevelHandleV1 *value : list)
		if (value->view() == view)
			return value;
	return nullptr;
}

ForeignToplevelHandleV1::ForeignToplevelHandleV1(wl_client *client, uint32_t id,
						 int version, CwlView *view,
						 CwlCompositor *compositor)
	: QtWaylandServer::zwlr_foreign_toplevel_handle_v1(client, id, version)
{
	m_view = view;
	m_compositor = compositor;

	connect(m_view->getTopLevel(), &QWaylandXdgToplevel::titleChanged, this,
		&ForeignToplevelHandleV1::onToplevelTitleChanged);
}

CwlView *ForeignToplevelHandleV1::view()
{
	return m_view;
}

void ForeignToplevelHandleV1::onToplevelTitleChanged()
{
	this->send_title(m_view->getTitle());
	this->send_done();
}

void ForeignToplevelHandleV1::zwlr_foreign_toplevel_handle_v1_bind_resource(
	Resource *resource)
{
}

void ForeignToplevelHandleV1::zwlr_foreign_toplevel_handle_v1_destroy_resource(
	Resource *resource)
{
}

void ForeignToplevelHandleV1::zwlr_foreign_toplevel_handle_v1_set_maximized(
	Resource *resource)
{
}

void ForeignToplevelHandleV1::zwlr_foreign_toplevel_handle_v1_unset_maximized(
	Resource *resource)
{
}

void ForeignToplevelHandleV1::zwlr_foreign_toplevel_handle_v1_set_minimized(
	Resource *resource)
{
}

void ForeignToplevelHandleV1::zwlr_foreign_toplevel_handle_v1_unset_minimized(
	Resource *resource)
{
}

void ForeignToplevelHandleV1::zwlr_foreign_toplevel_handle_v1_activate(
	Resource *resource, struct ::wl_resource *seat)
{
	if(m_view->parentView())
		m_compositor->raise(m_view->parentView());
	else
		m_compositor->raise(m_view);

	if(m_view->getChildViews().size() > 0)
		m_compositor->defaultSeat()->setKeyboardFocus(m_view->getChildViews().last()->surface());
	else
		m_compositor->defaultSeat()->setKeyboardFocus(m_view->surface());
}

void ForeignToplevelHandleV1::zwlr_foreign_toplevel_handle_v1_close(
	Resource *resource)
{
	m_view->getTopLevel()->sendClose();
}

void ForeignToplevelHandleV1::zwlr_foreign_toplevel_handle_v1_set_rectangle(
	Resource *resource, struct ::wl_resource *surface, int32_t x, int32_t y,
	int32_t width, int32_t height)
{
}

void ForeignToplevelHandleV1::zwlr_foreign_toplevel_handle_v1_destroy(
	Resource *resource)
{
}

void ForeignToplevelHandleV1::zwlr_foreign_toplevel_handle_v1_set_fullscreen(
	Resource *resource, struct ::wl_resource *output)
{
}

void ForeignToplevelHandleV1::zwlr_foreign_toplevel_handle_v1_unset_fullscreen(
	Resource *resource)
{
}