#ifndef INPUTMETHODV2
#define INPUTMETHODV2

#include "wayland-util.h"

#include <QtWaylandCompositor/QWaylandCompositorExtensionTemplate>
#include <QtWaylandCompositor/QWaylandCompositor>
#include <QtWaylandCompositor/QWaylandSurface>

#include <cutie-wlc.h>
#include <view.h>

#include "qwayland-server-input-method-unstable-v2.h"

class InputMethodV2;

class InputMethodManagerV2 : public QWaylandCompositorExtensionTemplate<InputMethodManagerV2>
	, public QtWaylandServer::zwp_input_method_manager_v2

{
	Q_OBJECT
public:
	InputMethodManagerV2(CwlCompositor *compositor);
	void initialize() override;

protected:
	void zwp_input_method_manager_v2_bind_resource(Resource *resource) override;
    void zwp_input_method_manager_v2_destroy_resource(Resource *resource) override;

    void zwp_input_method_manager_v2_get_input_method(Resource *resource, struct ::wl_resource *seat, uint32_t input_method) override;
    void zwp_input_method_manager_v2_destroy(Resource *resource) override;
	

private:
	CwlCompositor *m_compositor;

};

class InputMethodV2 : public QWaylandCompositorExtensionTemplate<InputMethodV2>
	, public QtWaylandServer::zwp_input_method_v2

{
	Q_OBJECT
public:
	InputMethodV2(struct ::wl_client *client, uint32_t id, int version);

protected:
	

private:
	CwlCompositor *m_compositor;

};

#endif //INPUTMETHODV2