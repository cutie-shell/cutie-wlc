#include <text-input-v3.h>

#include <QWaylandSeat>

TextInputManagerV3::TextInputManagerV3()
{
}

TextInputManagerV3::TextInputManagerV3(CwlCompositor *compositor)
	: QWaylandCompositorExtensionTemplate(compositor)
	, m_compositor(compositor)
{
}

void TextInputManagerV3::initialize()
{
	QWaylandCompositorExtensionTemplate::initialize();
	init(m_compositor->display(), 1);
}

void TextInputManagerV3::zwp_text_input_manager_v3_get_text_input(
	Resource *resource, uint32_t id, struct ::wl_resource *seat)
{
	QWaylandSurface *surface;
	TextInputV3 *textInput = new TextInputV3(
		resource->client(), id, resource->version(), m_compositor);

	connect(textInput, &TextInputV3::showInputPanel, this,
		&TextInputManagerV3::showInputPanel);
	connect(textInput, &TextInputV3::hideInputPanel, this,
		&TextInputManagerV3::hideInputPanel);
	connect(textInput, &TextInputV3::contentTypeChanged, this,
		&TextInputManagerV3::contentTypeChanged);

	if (m_compositor->defaultSeat()->keyboardFocus() != nullptr) {
		surface = m_compositor->defaultSeat()->keyboardFocus();
		if (surface->client()->client() == resource->client()) {
			textInput->send_enter(surface->resource());
			m_compositor->findTlView(surface)->tiV3 = textInput;
		}
	}
}

void TextInputManagerV3::zwp_text_input_manager_v3_destroy(Resource *resource)
{
}

TextInputV3::TextInputV3(wl_client *client, uint32_t id, int version,
			 CwlCompositor *compositor)
	: QtWaylandServer::zwp_text_input_v3(client, id, version)
{
	m_compositor = compositor;
}

void TextInputV3::zwp_text_input_v3_enable(Resource *resource)
{
	m_newEnabled = true;
}

void TextInputV3::zwp_text_input_v3_disable(Resource *resource)
{
	m_newEnabled = false;
}

void TextInputV3::zwp_text_input_v3_set_surrounding_text(Resource *resource,
							 const QString &text,
							 int32_t cursor,
							 int32_t anchor)
{
}

void TextInputV3::zwp_text_input_v3_set_text_change_cause(Resource *resource,
							  uint32_t cause)
{
}

void TextInputV3::zwp_text_input_v3_set_content_type(Resource *resource,
						     uint32_t hint,
						     uint32_t purpose)
{
	m_newContentHint = hint;
	m_newContentPurpose = purpose;
}

void TextInputV3::zwp_text_input_v3_set_cursor_rectangle(Resource *resource,
							 int32_t x, int32_t y,
							 int32_t width,
							 int32_t height)
{
}

void TextInputV3::zwp_text_input_v3_commit(Resource *resource)
{
	if (m_newEnabled != m_enabled) {
		m_enabled = m_newEnabled;
		if (m_enabled) {
			QWaylandSurface *surface =
				m_compositor->defaultSeat()->keyboardFocus();
			if (surface &&
			    surface->client()->client() == resource->client())
				m_compositor->findTlView(surface)->tiV3 = this;

			emit showInputPanel();
		} else {
			emit hideInputPanel();
		}
	}

	if (m_newContentHint != m_contentHint) {
		m_contentHint = m_newContentHint;
	}

	if (m_newContentPurpose != m_contentPurpose) {
		m_contentPurpose = m_newContentPurpose;
		emit contentTypeChanged(m_contentPurpose);
	}
}

void TextInputV3::zwp_text_input_v3_bind_resource(Resource *resource)
{
}

void TextInputV3::zwp_text_input_v3_destroy_resource(Resource *resource)
{
}