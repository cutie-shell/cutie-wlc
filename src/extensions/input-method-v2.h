#ifndef INPUTMETHODV2
#define INPUTMETHODV2

#include "wayland-util.h"

#include <QtWaylandCompositor/QWaylandCompositorExtensionTemplate>
#include <QtWaylandCompositor/QWaylandCompositor>
#include <QtWaylandCompositor/QWaylandSurface>

#include <cutie-wlc.h>
#include <text-input-v1.h>
#include <text-input-v2.h>
#include <text-input-v3.h>

#include "qwayland-server-input-method-unstable-v2.h"

class InputMethodV2;

class InputMethodManagerV2 : public QWaylandCompositorExtensionTemplate<InputMethodManagerV2>
	, public QtWaylandServer::zwp_input_method_manager_v2

{
	Q_OBJECT
public:
	InputMethodManagerV2(CwlCompositor *compositor);
	void initialize() override;

signals:
	void imDestroyed();
	
protected:
	void zwp_input_method_manager_v2_bind_resource(Resource *resource) override;
    void zwp_input_method_manager_v2_destroy_resource(Resource *resource) override;

    void zwp_input_method_manager_v2_get_input_method(Resource *resource, struct ::wl_resource *seat, uint32_t input_method) override;
    void zwp_input_method_manager_v2_destroy(Resource *resource) override;

private:
	CwlCompositor *m_compositor = nullptr;
	InputMethodV2 *m_inputmethod = nullptr;
	TextInputManagerV1 *m_textinputV1 = nullptr;
	TextInputManagerV2 *m_textinputV2 = nullptr;
	TextInputManagerV3 *m_textinputV3 = nullptr;

};

class InputMethodV2 : public QWaylandCompositorExtensionTemplate<InputMethodV2>
	, public QtWaylandServer::zwp_input_method_v2

{
	Q_OBJECT
public:
	InputMethodV2(struct ::wl_client *client, uint32_t id, int version, CwlCompositor *compositor);

protected:
	void zwp_input_method_v2_commit_string(Resource *resource, const QString &text) override;
	void zwp_input_method_v2_set_preedit_string(Resource *resource, const QString &text, int32_t cursor_begin, int32_t cursor_end) override;
	void zwp_input_method_v2_delete_surrounding_text(Resource *resource, uint32_t before_length, uint32_t after_length) override;
	void zwp_input_method_v2_commit(Resource *resource, uint32_t serial) override;
	void zwp_input_method_v2_get_input_popup_surface(Resource *resource, uint32_t id, struct ::wl_resource *surface) override;
	void zwp_input_method_v2_grab_keyboard(Resource *resource, uint32_t keyboard) override;
	void zwp_input_method_v2_destroy(Resource *resource) override;

public slots:
	void onShowInputPanel();
	void onHideInputPanel();

private:
	CwlCompositor *m_compositor;
	uint32_t m_serial = 0;
	QString m_lastString = "";

};

#endif //INPUTMETHODV2