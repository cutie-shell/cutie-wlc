#ifndef TEXT_INPUT_V3
#define TEXT_INPUT_V3

#include <wayland-util.h>

#include <QtWaylandCompositor/QWaylandCompositorExtensionTemplate>
#include <QtWaylandCompositor/QWaylandCompositor>

#include <cutie-wlc.h>

#include <qwayland-server-text-input-unstable-v3.h>

class TextInputV3;

class TextInputManagerV3
	: public QWaylandCompositorExtensionTemplate<TextInputManagerV3>,
	  public QtWaylandServer::zwp_text_input_manager_v3

{
	Q_OBJECT
    public:
	TextInputManagerV3();
	TextInputManagerV3(CwlCompositor *compositor);
	void initialize() override;

    signals:
	void showInputPanel();
	void hideInputPanel();
	void contentTypeChanged(uint32_t purpose);

    protected:
	void zwp_text_input_manager_v3_get_text_input(
		Resource *resource, uint32_t id,
		struct ::wl_resource *seat) override;
	void zwp_text_input_manager_v3_destroy(Resource *resource) override;

    private:
	CwlCompositor *m_compositor;
};

class TextInputV3 : public QWaylandCompositorExtensionTemplate<TextInputV3>,
		    public QtWaylandServer::zwp_text_input_v3 {
	Q_OBJECT
    public:
	TextInputV3(struct ::wl_client *client, uint32_t id, int version,
		    CwlCompositor *compositor);

    signals:
	void showInputPanel();
	void hideInputPanel();
	void contentTypeChanged(uint32_t purpose);

    protected:
	void zwp_text_input_v3_enable(Resource *resource) override;
	void zwp_text_input_v3_disable(Resource *resource) override;
	void zwp_text_input_v3_set_surrounding_text(Resource *resource,
						    const QString &text,
						    int32_t cursor,
						    int32_t anchor) override;
	void zwp_text_input_v3_set_text_change_cause(Resource *resource,
						     uint32_t cause) override;
	void zwp_text_input_v3_set_content_type(Resource *resource,
						uint32_t hint,
						uint32_t purpose) override;
	void zwp_text_input_v3_set_cursor_rectangle(Resource *resource,
						    int32_t x, int32_t y,
						    int32_t width,
						    int32_t height) override;
	void zwp_text_input_v3_commit(Resource *resource) override;
	void zwp_text_input_v3_bind_resource(Resource *resource) override;
	void zwp_text_input_v3_destroy_resource(Resource *resource) override;

    private:
	CwlCompositor *m_compositor;
};

#endif //TEXT_INPUT_V3