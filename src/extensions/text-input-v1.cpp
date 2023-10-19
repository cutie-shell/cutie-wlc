#include <text-input-v1.h>

TextInputManagerV1::TextInputManagerV1()
{
}

TextInputManagerV1::TextInputManagerV1(QWaylandCompositor *compositor)
    :QWaylandCompositorExtensionTemplate(compositor)
{

}

void TextInputManagerV1::initialize()
{
	QWaylandCompositorExtensionTemplate::initialize();
    QWaylandCompositor *compositor = static_cast<QWaylandCompositor *>(extensionContainer());
    init(compositor->display(), 1);
}

void TextInputManagerV1::zwp_text_input_manager_v1_create_text_input(Resource *resource, uint32_t id)
{

}

TextInputV1::TextInputV1(wl_client *client, uint32_t id, int version)
    :QtWaylandServer::zwp_text_input_v1(client, id, version)
{
}

void TextInputV1::zwp_text_input_v1_bind_resource(Resource *resource)
{
}

void TextInputV1::zwp_text_input_v1_destroy_resource(Resource *resource)
{

}

void TextInputV1::zwp_text_input_v1_activate(Resource *resource, struct ::wl_resource *seat, struct ::wl_resource *surface)
{
}

void TextInputV1::zwp_text_input_v1_deactivate(Resource *resource, struct ::wl_resource *seat)
{
}

void TextInputV1::zwp_text_input_v1_show_input_panel(Resource *resource)
{

}

void TextInputV1::zwp_text_input_v1_hide_input_panel(Resource *resource)
{

}

void TextInputV1::zwp_text_input_v1_reset(Resource *resource)
{
}

void TextInputV1::zwp_text_input_v1_set_surrounding_text(Resource *resource, const QString &text, uint32_t cursor, uint32_t anchor)
{
}

void TextInputV1::zwp_text_input_v1_set_content_type(Resource *resource, uint32_t hint, uint32_t purpose)
{
}

void TextInputV1::zwp_text_input_v1_set_cursor_rectangle(Resource *resource, int32_t x, int32_t y, int32_t width, int32_t height)
{
}

void TextInputV1::zwp_text_input_v1_set_preferred_language(Resource *resource, const QString &language)
{
}

void TextInputV1::zwp_text_input_v1_commit_state(Resource *resource, uint32_t serial)
{
}

void TextInputV1::zwp_text_input_v1_invoke_action(Resource *resource, uint32_t button, uint32_t index)
{
}