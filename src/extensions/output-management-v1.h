#pragma once

#include "wayland-util.h"

#include <QtWaylandCompositor/QWaylandCompositorExtensionTemplate>
#include <QtWaylandCompositor/QWaylandCompositor>
#include <QtWaylandCompositor/QWaylandSurface>

#include "qwayland-server-wlr-output-management-unstable-v1.h"
#include <cutie-wlc.h>
#include <glwindow.h>

class OutputHeadV1;
class OutputModeV1;
class OutputConfigurationHeadV1;

class OutputManagerV1
	: public QWaylandCompositorExtensionTemplate<OutputManagerV1>,
	  public QtWaylandServer::zwlr_output_manager_v1 {
	Q_OBJECT
    public:
	OutputManagerV1(CwlCompositor *compositor);
	void initialize() override;

    public Q_SLOTS:
	void onDisplayOffChanged(bool displayOff);
	void onScaleFactorChanged(int scaleFactor);

    protected:
	void zwlr_output_manager_v1_bind_resource(Resource *resource) override;
	void
	zwlr_output_manager_v1_destroy_resource(Resource *resource) override;
	void zwlr_output_manager_v1_create_configuration(
		Resource *resource, uint32_t id, uint32_t serial) override;

    private:
	uint32_t next_serial = 0;
	CwlCompositor *m_compositor;

	QMap<struct ::wl_client *, OutputHeadV1 *> m_head;
	QMap<struct ::wl_client *, OutputModeV1 *> m_mode;

	QMap<QString, QString> displayDescriptions = {
		{ "hwcomposer", "Android hwcomposer output via libhybris" },
		{ "eglfs", "DRM/KMS output via Qt EGLFS QPA" },
		{ "xcb", "Virtual X11 output" },
		{ "wayland", "Virtual nested Wayland output" }
	};
};

class OutputHeadV1 : public QWaylandCompositorExtensionTemplate<OutputHeadV1>,
		     public QtWaylandServer::zwlr_output_head_v1 {
	Q_OBJECT
    public:
	OutputHeadV1(struct ::wl_client *client, uint32_t id, int version);
};

class OutputModeV1 : public QWaylandCompositorExtensionTemplate<OutputModeV1>,
		     public QtWaylandServer::zwlr_output_mode_v1 {
	Q_OBJECT
    public:
	OutputModeV1(struct ::wl_client *client, uint32_t id, int version);
};

class OutputConfigurationV1
	: public QWaylandCompositorExtensionTemplate<OutputConfigurationV1>,
	  public QtWaylandServer::zwlr_output_configuration_v1 {
	Q_OBJECT
    public:
	OutputConfigurationV1(struct ::wl_client *client, uint32_t id,
			      int version);
	void setCompositor(CwlCompositor *compositor);

    protected:
	void zwlr_output_configuration_v1_enable_head(
		Resource *resource, uint32_t id,
		struct ::wl_resource *head) override;
	void zwlr_output_configuration_v1_disable_head(
		Resource *resource, struct ::wl_resource *head) override;
	void zwlr_output_configuration_v1_apply(Resource *resource) override;
	void zwlr_output_configuration_v1_test(Resource *resource) override;

    private:
	CwlCompositor *m_compositor = nullptr;
	OutputConfigurationHeadV1 *m_headConfiguration = nullptr;
	bool m_headEnabled = false;
	bool m_headConfigured = false;
};

class OutputConfigurationHeadV1
	: public QWaylandCompositorExtensionTemplate<OutputConfigurationHeadV1>,
	  public QtWaylandServer::zwlr_output_configuration_head_v1 {
	Q_OBJECT
    public:
	OutputConfigurationHeadV1(struct ::wl_client *client, uint32_t id,
				  int version);
	int m_scale;

    protected:
	void zwlr_output_configuration_head_v1_set_mode(
		Resource *resource, struct ::wl_resource *mode) override;
	void zwlr_output_configuration_head_v1_set_custom_mode(
		Resource *resource, int32_t width, int32_t height,
		int32_t refresh) override;
	void zwlr_output_configuration_head_v1_set_position(Resource *resource,
							    int32_t x,
							    int32_t y) override;
	void zwlr_output_configuration_head_v1_set_transform(
		Resource *resource, int32_t transform) override;
	void
	zwlr_output_configuration_head_v1_set_scale(Resource *resource,
						    wl_fixed_t scale) override;
	void zwlr_output_configuration_head_v1_set_adaptive_sync(
		Resource *resource, uint32_t state) override;
};
