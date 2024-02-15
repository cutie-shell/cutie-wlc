#include <output-management-v1.h>

#include <QGuiApplication>

OutputManagerV1::OutputManagerV1(CwlCompositor *compositor)
	: QWaylandCompositorExtensionTemplate(compositor)
	, m_compositor(compositor)
{
	connect(m_compositor->glWindow(), &GlWindow::displayOffChanged, this,
		&OutputManagerV1::onDisplayOffChanged);
	connect(m_compositor, &CwlCompositor::scaleFactorChanged, this,
		&OutputManagerV1::onScaleFactorChanged);
}

void OutputManagerV1::initialize()
{
	QWaylandCompositorExtensionTemplate::initialize();
	CwlCompositor *compositor =
		static_cast<CwlCompositor *>(extensionContainer());
	init(compositor->display(), 3);
}

void OutputManagerV1::onDisplayOffChanged(bool displayOff)
{
	QMultiMapIterator<struct ::wl_client *, Resource *> i(resourceMap());
	while (i.hasNext()) {
		i.next();
		m_head[i.key()]->send_enabled((uint32_t)!displayOff);
		send_done(i.value()->handle, next_serial++);
	}
}

void OutputManagerV1::onScaleFactorChanged(int scaleFactor)
{
	QMultiMapIterator<struct ::wl_client *, Resource *> i(resourceMap());
	while (i.hasNext()) {
		i.next();
		m_head[i.key()]->send_scale(wl_fixed_from_double(scaleFactor));
		send_done(i.value()->handle, next_serial++);
	}
}

void OutputManagerV1::zwlr_output_manager_v1_bind_resource(Resource *resource)
{
	OutputHeadV1 *head =
		new OutputHeadV1(resource->client(), 0, resource->version());
	m_head.insert(resource->client(), head);
	send_head(resource->handle, head->resource()->handle);

	head->send_name(qGuiApp->platformName().toUpper() + "-1");
	if (displayDescriptions.contains(qGuiApp->platformName()))
		head->send_description(
			displayDescriptions[qGuiApp->platformName()]);
	else
		head->send_description("An output of unknown type");
	head->send_enabled(!m_compositor->glWindow()->displayOff());
	head->send_position(0, 0);
	head->send_transform(0);
	head->send_scale(wl_fixed_from_double(m_compositor->scaleFactor()));

	OutputModeV1 *mode =
		new OutputModeV1(resource->client(), 0, resource->version());
	m_mode.insert(resource->client(), mode);
	head->send_mode(mode->resource()->handle);
	head->send_current_mode(mode->resource()->handle);

	mode->send_size(m_compositor->glWindow()->width(),
			m_compositor->glWindow()->height());
	mode->send_preferred();

	send_done(resource->handle, next_serial++);
}

void OutputManagerV1::zwlr_output_manager_v1_destroy_resource(Resource *resource)
{
	delete m_head[resource->client()];
	delete m_mode[resource->client()];

	m_head.remove(resource->client());
	m_mode.remove(resource->client());
}

void OutputManagerV1::zwlr_output_manager_v1_create_configuration(
	Resource *resource, uint32_t id, uint32_t serial)
{
	OutputConfigurationV1 *configuration = new OutputConfigurationV1(
		resource->client(), id, resource->version());
	configuration->setCompositor(m_compositor);
}

OutputHeadV1::OutputHeadV1(wl_client *client, uint32_t id, int version)
	: QtWaylandServer::zwlr_output_head_v1(client, id, version)
{
}

OutputModeV1::OutputModeV1(wl_client *client, uint32_t id, int version)
	: QtWaylandServer::zwlr_output_mode_v1(client, id, version)
{
}

OutputConfigurationV1::OutputConfigurationV1(wl_client *client, uint32_t id,
					     int version)
	: QtWaylandServer::zwlr_output_configuration_v1(client, id, version)
{
}

void OutputConfigurationV1::setCompositor(CwlCompositor *compositor)
{
	m_compositor = compositor;
}

void OutputConfigurationV1::zwlr_output_configuration_v1_enable_head(
	Resource *resource, uint32_t id, struct ::wl_resource *head)
{
	if (m_headConfigured) {
		send_failed();
		return;
	}
	m_headConfiguration = new OutputConfigurationHeadV1(
		resource->client(), id, resource->version());
	m_headConfiguration->m_scale =
		m_compositor ? m_compositor->scaleFactor() : 1;
	m_headEnabled = true;
	m_headConfigured = true;
}

void OutputConfigurationV1::zwlr_output_configuration_v1_disable_head(
	Resource *resource, struct ::wl_resource *head)
{
	if (m_headConfigured) {
		send_failed();
		return;
	}
	m_headEnabled = false;
	m_headConfigured = true;
}

void OutputConfigurationV1::zwlr_output_configuration_v1_apply(
	Resource *resource)
{
	if (!m_headConfigured) {
		send_failed();
		return;
	}

	if (m_headEnabled) {
		m_compositor->setScaleFactor(m_headConfiguration->m_scale);
		m_compositor->glWindow()->setDisplayOff(false);
	} else {
		m_compositor->glWindow()->setDisplayOff(true);
	}

	send_succeeded();
}

void OutputConfigurationV1::zwlr_output_configuration_v1_test(Resource *resource)
{
	if (!m_headConfigured)
		send_failed();
	else
		send_succeeded();
}

OutputConfigurationHeadV1::OutputConfigurationHeadV1(wl_client *client,
						     uint32_t id, int version)
	: QtWaylandServer::zwlr_output_configuration_head_v1(client, id,
							     version)
{
}

void OutputConfigurationHeadV1::zwlr_output_configuration_head_v1_set_mode(
	Resource *resource, struct ::wl_resource *mode)
{
}

void OutputConfigurationHeadV1::zwlr_output_configuration_head_v1_set_custom_mode(
	Resource *resource, int32_t width, int32_t height, int32_t refresh)
{
}

void OutputConfigurationHeadV1::zwlr_output_configuration_head_v1_set_position(
	Resource *resource, int32_t x, int32_t y)
{
	// TODO: handle this
}

void OutputConfigurationHeadV1::zwlr_output_configuration_head_v1_set_transform(
	Resource *resource, int32_t transform)
{
}

void OutputConfigurationHeadV1::zwlr_output_configuration_head_v1_set_scale(
	Resource *resource, wl_fixed_t scale)
{
	if (wl_fixed_to_double(scale) > 0)
		m_scale = wl_fixed_to_double(scale);
}

void OutputConfigurationHeadV1::
	zwlr_output_configuration_head_v1_set_adaptive_sync(Resource *resource,
							    uint32_t state)
{
}