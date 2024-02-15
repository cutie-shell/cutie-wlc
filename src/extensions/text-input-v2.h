#ifndef TEXT_INPUT_V2
#define TEXT_INPUT_V2

#include <wayland-util.h>

#include <QtWaylandCompositor/QWaylandCompositorExtensionTemplate>
#include <QtWaylandCompositor/QWaylandCompositor>

#include <cutie-wlc.h>

#include <qwayland-server-text-input-unstable-v2.h>

class TextInputV2;

class TextInputManagerV2
	: public QWaylandCompositorExtensionTemplate<TextInputManagerV2>,
	  public QtWaylandServer::zwp_text_input_manager_v2

{
	Q_OBJECT
    public:
	TextInputManagerV2();
	TextInputManagerV2(CwlCompositor *compositor);
	void initialize() override;

    signals:
	void showInputPanel();
	void hideInputPanel();
	void contentTypeChanged(uint32_t purpose);

    protected:
	void zwp_text_input_manager_v2_get_text_input(
		Resource *resource, uint32_t id,
		struct ::wl_resource *seat) override;

    private:
	CwlCompositor *m_compositor;
};

class TextInputV2 : public QWaylandCompositorExtensionTemplate<TextInputV2>,
		    public QtWaylandServer::zwp_text_input_v2 {
	Q_OBJECT
    public:
	TextInputV2(struct ::wl_client *client, uint32_t id, int version,
		    CwlCompositor *compositor);

    signals:
	void showInputPanel();
	void hideInputPanel();
	void contentTypeChanged(uint32_t purpose);

    protected:
	void zwp_text_input_v2_destroy(Resource *resource) override;
	void zwp_text_input_v2_destroy_resource(Resource *resource) override;
	void zwp_text_input_v2_enable(Resource *resource,
				      struct ::wl_resource *surface) override;
	void zwp_text_input_v2_disable(Resource *resource,
				       struct ::wl_resource *surface) override;
	void zwp_text_input_v2_show_input_panel(Resource *resource) override;
	void zwp_text_input_v2_hide_input_panel(Resource *resource) override;
	void zwp_text_input_v2_set_surrounding_text(Resource *resource,
						    const QString &text,
						    int32_t cursor,
						    int32_t anchor) override;
	void zwp_text_input_v2_set_content_type(Resource *resource,
						uint32_t hint,
						uint32_t purpose) override;
	void zwp_text_input_v2_set_cursor_rectangle(Resource *resource,
						    int32_t x, int32_t y,
						    int32_t width,
						    int32_t height) override;
	void zwp_text_input_v2_set_preferred_language(
		Resource *resource, const QString &language) override;
	void zwp_text_input_v2_update_state(Resource *resource, uint32_t serial,
					    uint32_t reason) override;

    private:
	CwlCompositor *m_compositor;
};

#endif //TEXTINPUTV2