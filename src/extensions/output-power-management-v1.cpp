#include <output-power-management-v1.h>

OutputPowerManagerV1::OutputPowerManagerV1(CwlCompositor *compositor)
	: QWaylandCompositorExtensionTemplate(compositor)
	, m_compositor(compositor)
{
}

void OutputPowerManagerV1::initialize()
{
	QWaylandCompositorExtensionTemplate::initialize();
	if (m_compositor) {
		init(m_compositor->display(), 1);
	}
}

void OutputPowerManagerV1::zwlr_output_power_manager_v1_get_output_power(
	Resource *resource, uint32_t id, struct ::wl_resource *output)
{
	OutputPowerV1 *power =
		new OutputPowerV1(resource->client(), id, resource->version());
	if (m_compositor) {
		power->setGlWindow(m_compositor->glWindow());
	}
}

OutputPowerV1::OutputPowerV1(wl_client *client, uint32_t id, int version)
	: QtWaylandServer::zwlr_output_power_v1(client, id, version)
{
}

void OutputPowerV1::setGlWindow(GlWindow *glWindow)
{
	m_glWindow = glWindow;
	if (m_glWindow) {
		connect(m_glWindow, &GlWindow::displayOffChanged, this,
			&OutputPowerV1::onDisplayOffChanged);
		onDisplayOffChanged(m_glWindow->displayOff());
	}
}

void OutputPowerV1::onDisplayOffChanged(bool displayOff)
{
	if (m_glWindow) {
		if (displayOff) {
			send_mode(mode_off);
		} else {
			send_mode(mode_on);
		}
	}
}

void OutputPowerV1::zwlr_output_power_v1_set_mode(Resource *resource,
						  uint32_t mode)
{
	if (!m_glWindow) {
		return;
	}
	m_glWindow->setDisplayOff(!mode);
}
