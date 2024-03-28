#include <layer-shell.h>

LayerShellV1::LayerShellV1(QWaylandCompositor *compositor)
	: QWaylandCompositorExtensionTemplate(compositor)
	, m_compositor(compositor)
{
}

void LayerShellV1::initialize()
{
	QWaylandCompositorExtensionTemplate::initialize();
	init(m_compositor->display(), 1);
}

void LayerShellV1::zwlr_layer_shell_v1_get_layer_surface(
	Resource *resource, uint32_t id, struct ::wl_resource *surface,
	struct ::wl_resource *output, uint32_t layer, const QString &scope)
{
	LayerSurfaceV1 *obj =
		new LayerSurfaceV1(resource->client(), id, resource->version());
	obj->surface = QWaylandSurface::fromResource(surface);
	obj->ls_layer = layer;
	obj->new_ls_layer = layer;
	obj->ls_scope = scope;

	connect(obj->surface, &QWaylandSurface::redraw, obj,
		&LayerSurfaceV1::onCommit);

	if (layer == 2)
		emit layerShellSurfaceTopCreated(surface);
	else
		emit layerShellSurfaceCreated(obj);
}

LayerSurfaceV1::LayerSurfaceV1(wl_client *client, uint32_t id, int version)
	: QtWaylandServer::zwlr_layer_surface_v1(client, id, version)
{
}

void LayerSurfaceV1::zwlr_layer_surface_v1_set_size(Resource *resource,
						    uint32_t width,
						    uint32_t height)
{
	newSize = QSize(width, height);
}

void LayerSurfaceV1::zwlr_layer_surface_v1_set_anchor(Resource *resource,
						      uint32_t anchor)
{
	new_ls_anchor = anchor;
}

void LayerSurfaceV1::zwlr_layer_surface_v1_set_exclusive_zone(
	Resource *resource, int32_t zone)
{
	new_ls_zone = zone;
}

void LayerSurfaceV1::zwlr_layer_surface_v1_set_margin(Resource *resource,
						      int32_t top,
						      int32_t right,
						      int32_t bottom,
						      int32_t left)
{
	newMargins = QMargins(left, top, right, bottom);
}

void LayerSurfaceV1::zwlr_layer_surface_v1_set_keyboard_interactivity(
	Resource *resource, uint32_t keyboard_interactivity)
{
	new_ls_keyboard_interactivity = keyboard_interactivity;
}

void LayerSurfaceV1::zwlr_layer_surface_v1_get_popup(
	Resource *resource, struct ::wl_resource *popup)
{
	qDebug() << "get_popup";
}

void LayerSurfaceV1::zwlr_layer_surface_v1_ack_configure(Resource *resource,
							 uint32_t serial)
{
	if (!initialized) {
		initialized = true;
		ls_serial = serial;
		emit layerSurfaceDataChanged(this);
	} else {
		ls_serial = serial;
	}
}

void LayerSurfaceV1::zwlr_layer_surface_v1_destroy(Resource *resource)
{
	emit layerSurfaceDestroyed(surface);
}

void LayerSurfaceV1::zwlr_layer_surface_v1_set_layer(Resource *resource,
						     uint32_t layer)
{
	new_ls_layer = layer;
}

void LayerSurfaceV1::onCommit()
{
	bool dataChanged = false;

	if (newSize != size) {
		size = newSize;
		qDebug() << size;
		dataChanged = true;
	}

	if (newMargins != margins) {
		margins = newMargins;
		dataChanged = true;
	}

	if (new_ls_anchor != ls_anchor) {
		ls_anchor = new_ls_anchor;
		dataChanged = true;
	}

	if (new_ls_zone != ls_zone) {
		ls_zone = new_ls_zone;
		dataChanged = true;
	}

	if (new_ls_keyboard_interactivity != ls_keyboard_interactivity) {
		ls_keyboard_interactivity = new_ls_keyboard_interactivity;
		dataChanged = true;
	}

	if (new_ls_layer != ls_layer) {
		ls_layer = new_ls_layer;
		dataChanged = true;
	}

	if (dataChanged)
		emit layerSurfaceDataChanged(this);
}
