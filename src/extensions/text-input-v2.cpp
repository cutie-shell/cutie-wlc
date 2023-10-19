#include <text-input-v2.h>

TextInputManagerV2::TextInputManagerV2()
{
}

TextInputManagerV2::TextInputManagerV2(QWaylandCompositor *compositor)
    :QWaylandCompositorExtensionTemplate(compositor)
{
}

void TextInputManagerV2::initialize()
{
	QWaylandCompositorExtensionTemplate::initialize();
    QWaylandCompositor *compositor = static_cast<QWaylandCompositor *>(extensionContainer());
    init(compositor->display(), 1);
}

void TextInputManagerV2::zwp_text_input_manager_v2_get_text_input(Resource *resource, uint32_t id, struct ::wl_resource *seat)
{

}

TextInputV2::TextInputV2(wl_client *client, uint32_t id, int version)
    :QtWaylandServer::zwp_text_input_v2(client, id, version)
{
}

void TextInputV2::zwp_text_input_v2_show_input_panel(Resource *resource)
{

}

void TextInputV2::zwp_text_input_v2_hide_input_panel(Resource *resource)
{

}

void TextInputV2::zwp_text_input_v2_destroy_resource(Resource *resource)
{

}