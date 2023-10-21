#ifndef TEXT_INPUT_V1
#define TEXT_INPUT_V1

#include <wayland-util.h>

#include <QtWaylandCompositor/QWaylandCompositorExtensionTemplate>
#include <QtWaylandCompositor/QWaylandCompositor>

#include <cutie-wlc.h>

#include <qwayland-server-text-input-unstable-v1.h>

class TextInputV1;

class TextInputManagerV1 : public QWaylandCompositorExtensionTemplate<TextInputManagerV1>
	, public QtWaylandServer::zwp_text_input_manager_v1

{
	Q_OBJECT
public:
	TextInputManagerV1();
	TextInputManagerV1(CwlCompositor *compositor);
	void initialize() override;

signals:
	void showInputPanel();
	void hideInputPanel();

protected:
	void zwp_text_input_manager_v1_create_text_input(Resource *resource, uint32_t id) override;

private:
	CwlCompositor *m_compositor;

};

class  TextInputV1 : public QWaylandCompositorExtensionTemplate< TextInputV1>
	, public QtWaylandServer::zwp_text_input_v1
{
	Q_OBJECT
public:
	TextInputV1(struct ::wl_client *client, uint32_t id, int version);

signals:
	void showInputPanel();
	void hideInputPanel();

protected:
	void zwp_text_input_v1_bind_resource(Resource *resource) override;
	void zwp_text_input_v1_destroy_resource(Resource *resource) override;

	void zwp_text_input_v1_activate(Resource *resource, struct ::wl_resource *seat, struct ::wl_resource *surface) override;
	void zwp_text_input_v1_deactivate(Resource *resource, struct ::wl_resource *seat) override;
	void zwp_text_input_v1_show_input_panel(Resource *resource) override;
	void zwp_text_input_v1_hide_input_panel(Resource *resource) override;
	void zwp_text_input_v1_reset(Resource *resource) override;
	void zwp_text_input_v1_set_surrounding_text(Resource *resource, const QString &text, uint32_t cursor, uint32_t anchor) override;
	void zwp_text_input_v1_set_content_type(Resource *resource, uint32_t hint, uint32_t purpose) override;
	void zwp_text_input_v1_set_cursor_rectangle(Resource *resource, int32_t x, int32_t y, int32_t width, int32_t height) override;
	void zwp_text_input_v1_set_preferred_language(Resource *resource, const QString &language) override;
	void zwp_text_input_v1_commit_state(Resource *resource, uint32_t serial) override;
	void zwp_text_input_v1_invoke_action(Resource *resource, uint32_t button, uint32_t index) override;
};

#endif //TEXT_INPUT_V1