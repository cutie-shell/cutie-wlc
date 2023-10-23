#include <text-input-v2.h>

#include <QWaylandSeat>

TextInputManagerV2::TextInputManagerV2()
{
}

TextInputManagerV2::TextInputManagerV2(CwlCompositor *compositor)
    :QWaylandCompositorExtensionTemplate(compositor)
{
	m_compositor = compositor;
}

void TextInputManagerV2::initialize()
{
	QWaylandCompositorExtensionTemplate::initialize();
    QWaylandCompositor *compositor = static_cast<QWaylandCompositor *>(extensionContainer());
    init(compositor->display(), 1);
}

void TextInputManagerV2::zwp_text_input_manager_v2_get_text_input(Resource *resource, uint32_t id, struct ::wl_resource *seat)
{
	QWaylandSurface *surface;
	TextInputV2 *textInput = new TextInputV2(resource->client(), id, resource->version(), m_compositor);

	connect(textInput, &TextInputV2::showInputPanel, this, &TextInputManagerV2::showInputPanel);
	connect(textInput, &TextInputV2::hideInputPanel, this, &TextInputManagerV2::hideInputPanel);

	if(m_compositor->defaultSeat()->keyboardFocus() != nullptr){
        surface = m_compositor->defaultSeat()->keyboardFocus();
        if(surface->client()->client() == resource->client()){
            textInput->send_enter(0, surface->resource());
            m_compositor->findView(surface)->tiV2 = textInput;
        }
    }
}

TextInputV2::TextInputV2(wl_client *client, uint32_t id, int version, CwlCompositor *compositor)
    :QtWaylandServer::zwp_text_input_v2(client, id, version)
{
	m_compositor = compositor;
}

void TextInputV2::zwp_text_input_v2_show_input_panel(Resource *resource)
{
	QWaylandSurface *surface;
	if(m_compositor->defaultSeat()->keyboardFocus() != nullptr){
        surface = m_compositor->defaultSeat()->keyboardFocus();
        if(surface->client()->client() == resource->client()){
        	m_compositor->findView(surface)->tiV2 = this;
        }
    }

	emit showInputPanel();
}

void TextInputV2::zwp_text_input_v2_hide_input_panel(Resource *resource)
{
	emit hideInputPanel();
}

void TextInputV2::zwp_text_input_v2_destroy_resource(Resource *resource)
{

}