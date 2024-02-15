#pragma once

#include "wayland-util.h"

#include <QtWaylandCompositor/QWaylandCompositorExtensionTemplate>
#include <QtWaylandCompositor/QWaylandCompositor>
#include <QtWaylandCompositor/QWaylandSurface>

#include "qwayland-server-wlr-output-power-management-unstable-v1.h"
#include <cutie-wlc.h>
#include <glwindow.h>

class OutputPowerManagerV1
	: public QWaylandCompositorExtensionTemplate<OutputPowerManagerV1>,
	  public QtWaylandServer::zwlr_output_power_manager_v1 {
	Q_OBJECT
    public:
	OutputPowerManagerV1(CwlCompositor *compositor);
	void initialize() override;

    protected:
	void zwlr_output_power_manager_v1_get_output_power(
		Resource *resource, uint32_t id,
		struct ::wl_resource *output) override;

    private:
	CwlCompositor *m_compositor;
};

class OutputPowerV1 : public QWaylandCompositorExtensionTemplate<OutputPowerV1>,
		      public QtWaylandServer::zwlr_output_power_v1 {
	Q_OBJECT
    public:
	OutputPowerV1(struct ::wl_client *client, uint32_t id, int version);
	void setGlWindow(GlWindow *glWindow);

    protected:
	void zwlr_output_power_v1_set_mode(Resource *resource,
					   uint32_t mode) override;

    private Q_SLOTS:
	void onDisplayOffChanged(bool displayOff);

    private:
	GlWindow *m_glWindow = nullptr;
};
