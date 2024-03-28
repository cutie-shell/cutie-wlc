#ifndef FOREIGNTOPLEVEL
#define FOREIGNTOPLEVEL

#include "wayland-util.h"

#include <QtWaylandCompositor/QWaylandCompositorExtensionTemplate>
#include <QtWaylandCompositor/QWaylandCompositor>
#include <QtWaylandCompositor/QWaylandSurface>

#include <cutie-wlc.h>
#include <view.h>

#include "qwayland-server-wlr-foreign-toplevel-management-unstable-v1.h"

class ForeignToplevelHandleV1;
class ForeignToplevelManagerV1
	: public QWaylandCompositorExtensionTemplate<ForeignToplevelManagerV1>,
	  public QtWaylandServer::zwlr_foreign_toplevel_manager_v1

{
	Q_OBJECT
    public:
	ForeignToplevelManagerV1(CwlCompositor *compositor);
	void initialize() override;
	void removedToplevel(CwlView *view);
	ForeignToplevelHandleV1 *handleForView(struct ::wl_client *client,
					       CwlView *view);

    public slots:
	void onToplevelCreated(CwlView *view);
	void onToplevelDestroyed(CwlView *view);

    protected:
	void zwlr_foreign_toplevel_manager_v1_bind_resource(
		Resource *resource) override;
	void zwlr_foreign_toplevel_manager_v1_destroy_resource(
		Resource *resource) override;

	void zwlr_foreign_toplevel_manager_v1_stop(Resource *resource) override;

    private:
	CwlCompositor *m_compositor;
	QMap<struct ::wl_client *, QList<ForeignToplevelHandleV1 *> >
		m_toplevelMap;
};

class ForeignToplevelHandleV1
	: public QWaylandCompositorExtensionTemplate<ForeignToplevelHandleV1>,
	  public QtWaylandServer::zwlr_foreign_toplevel_handle_v1 {
	Q_OBJECT
    public:
	ForeignToplevelHandleV1(struct ::wl_client *client, uint32_t id,
				int version, CwlView *view,
				CwlCompositor *compositor);
	CwlView *view();

    private slots:
	void onToplevelTitleChanged();
	void onToplevelAppIdChanged();

    protected:
	void zwlr_foreign_toplevel_handle_v1_bind_resource(
		Resource *resource) override;
	void zwlr_foreign_toplevel_handle_v1_destroy_resource(
		Resource *resource) override;

	void zwlr_foreign_toplevel_handle_v1_set_maximized(
		Resource *resource) override;
	void zwlr_foreign_toplevel_handle_v1_unset_maximized(
		Resource *resource) override;
	void zwlr_foreign_toplevel_handle_v1_set_minimized(
		Resource *resource) override;
	void zwlr_foreign_toplevel_handle_v1_unset_minimized(
		Resource *resource) override;
	void zwlr_foreign_toplevel_handle_v1_activate(
		Resource *resource, struct ::wl_resource *seat) override;
	void zwlr_foreign_toplevel_handle_v1_close(Resource *resource) override;
	void zwlr_foreign_toplevel_handle_v1_set_rectangle(
		Resource *resource, struct ::wl_resource *surface, int32_t x,
		int32_t y, int32_t width, int32_t height) override;
	void
	zwlr_foreign_toplevel_handle_v1_destroy(Resource *resource) override;
	void zwlr_foreign_toplevel_handle_v1_set_fullscreen(
		Resource *resource, struct ::wl_resource *output) override;
	void zwlr_foreign_toplevel_handle_v1_unset_fullscreen(
		Resource *resource) override;

    private:
	CwlView *m_view = nullptr;
	CwlCompositor *m_compositor = nullptr;
};

#endif //FOREIGNTOPLEVEL