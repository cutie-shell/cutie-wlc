#include <text-input-v3.h>

TextInputManagerV3::TextInputManagerV3()
{
}

TextInputManagerV3::TextInputManagerV3(QWaylandCompositor *compositor)
    :QWaylandCompositorExtensionTemplate(compositor)
{
}

void TextInputManagerV3::initialize()
{
	QWaylandCompositorExtensionTemplate::initialize();
    QWaylandCompositor *compositor = static_cast<QWaylandCompositor *>(extensionContainer());
    init(compositor->display(), 1);
}

void TextInputManagerV3::zwp_text_input_manager_v3_get_text_input(Resource *resource, uint32_t id, struct ::wl_resource *seat)
{

}

void TextInputManagerV3::zwp_text_input_manager_v3_destroy(Resource *resource)
{
}

TextInputV3::TextInputV3(wl_client *client, uint32_t id, int version)
    :QtWaylandServer::zwp_text_input_v3(client, id, version)
{
}

void TextInputV3::zwp_text_input_v3_enable(Resource *resource)
{

}

void TextInputV3::zwp_text_input_v3_disable(Resource *resource)
{

}

void TextInputV3::zwp_text_input_v3_set_surrounding_text(Resource *resource, const QString &text, int32_t cursor, int32_t anchor)
{

}

void TextInputV3::zwp_text_input_v3_set_text_change_cause(Resource *resource, uint32_t cause)
{

}

void TextInputV3::zwp_text_input_v3_set_content_type(Resource *resource, uint32_t hint, uint32_t purpose)
{

}

void TextInputV3::zwp_text_input_v3_set_cursor_rectangle(Resource *resource, int32_t x, int32_t y, int32_t width, int32_t height)
{

}

void TextInputV3::zwp_text_input_v3_commit(Resource *resource)
{

}

void TextInputV3::zwp_text_input_v3_bind_resource(Resource *resource)
{
}

void TextInputV3::zwp_text_input_v3_destroy_resource(Resource *resource)
{

}