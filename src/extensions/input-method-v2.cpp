#include <input-method-v2.h>

InputMethodManagerV2::InputMethodManagerV2(CwlCompositor *compositor)
    :QWaylandCompositorExtensionTemplate(compositor)
{
    m_compositor = compositor;
}

void InputMethodManagerV2::initialize()
{
	QWaylandCompositorExtensionTemplate::initialize();
    CwlCompositor *compositor = static_cast<CwlCompositor *>(extensionContainer());
    init(compositor->display(), 1);
}

void InputMethodManagerV2::zwp_input_method_manager_v2_bind_resource(Resource *resource)
{
	
}
void InputMethodManagerV2::zwp_input_method_manager_v2_destroy_resource(Resource *resource)
{
	
}

void InputMethodManagerV2::zwp_input_method_manager_v2_get_input_method(Resource *resource, struct ::wl_resource *seat, uint32_t input_method)
{
	
}

void InputMethodManagerV2::zwp_input_method_manager_v2_destroy(Resource *resource)
{

}

InputMethodV2::InputMethodV2(struct ::wl_client *client, uint32_t id, int version)
    :QtWaylandServer::zwp_input_method_v2(client, id, version)
{

}