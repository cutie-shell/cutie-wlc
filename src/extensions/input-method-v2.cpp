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
	delete m_inputmethod;
	delete m_textinputV1;
	delete m_textinputV2;
	delete m_textinputV3;

	emit imDestroyed();
}

void InputMethodManagerV2::zwp_input_method_manager_v2_get_input_method(Resource *resource, struct ::wl_resource *seat, uint32_t input_method)
{
	if(m_inputmethod == nullptr){
		m_inputmethod = new InputMethodV2(resource->client(), input_method, resource->version());

		m_textinputV1 = new TextInputManagerV1(m_compositor);
		m_textinputV2 = new TextInputManagerV2(m_compositor);
    	m_textinputV3 = new TextInputManagerV3(m_compositor);

		connect(m_textinputV1, &TextInputManagerV1::showInputPanel, m_inputmethod, &InputMethodV2::onShowInputPanel);
		connect(m_textinputV1, &TextInputManagerV1::hideInputPanel, m_inputmethod, &InputMethodV2::onHideInputPanel);

		connect(m_textinputV2, &TextInputManagerV2::showInputPanel, m_inputmethod, &InputMethodV2::onShowInputPanel);
		connect(m_textinputV2, &TextInputManagerV2::hideInputPanel, m_inputmethod, &InputMethodV2::onHideInputPanel);

		connect(m_textinputV3, &TextInputManagerV3::showInputPanel, m_inputmethod, &InputMethodV2::onShowInputPanel);
		connect(m_textinputV3, &TextInputManagerV3::hideInputPanel, m_inputmethod, &InputMethodV2::onHideInputPanel);

	} else {
		qWarning()<<"Another input method active, ignoring this request!!!";
	}
}

void InputMethodManagerV2::zwp_input_method_manager_v2_destroy(Resource *resource)
{

}

InputMethodV2::InputMethodV2(struct ::wl_client *client, uint32_t id, int version)
    :QtWaylandServer::zwp_input_method_v2(client, id, version)
{

}

void InputMethodV2::zwp_input_method_v2_commit_string(Resource *resource, const QString &text)
{
	qDebug()<<"COMMIT STRING SERVER";
}

void InputMethodV2::zwp_input_method_v2_set_preedit_string(Resource *resource, const QString &text, int32_t cursor_begin, int32_t cursor_end)
{

}

void InputMethodV2::zwp_input_method_v2_delete_surrounding_text(Resource *resource, uint32_t before_length, uint32_t after_length)
{

}

void InputMethodV2::zwp_input_method_v2_commit(Resource *resource, uint32_t serial)
{
	qDebug()<<"COMMIT SERVER";
}

void InputMethodV2::zwp_input_method_v2_get_input_popup_surface(Resource *resource, uint32_t id, struct ::wl_resource *surface)
{

}

void InputMethodV2::zwp_input_method_v2_grab_keyboard(Resource *resource, uint32_t keyboard)
{
	qDebug()<<"GRAB KEYBOARD SERVER";
}

void InputMethodV2::zwp_input_method_v2_destroy(Resource *resource)
{

}

void InputMethodV2::onShowInputPanel()
{
	qDebug()<<"SHOW INPUT PANEL SERVER";
	this->send_activate();
	this->send_done();
}

void InputMethodV2::onHideInputPanel()
{
	qDebug()<<"HIDE INPUT PANEL SERVER";
	this->send_deactivate();
	this->send_done();
}