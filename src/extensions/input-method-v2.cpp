#include <input-method-v2.h>

#include <view.h>
#include <QWaylandSeat>
#include <QKeyEvent>

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
		m_inputmethod = new InputMethodV2(resource->client(), input_method, resource->version(), m_compositor);

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

InputMethodV2* InputMethodManagerV2::getInputMethod()
{
	return m_inputmethod;
}

InputMethodV2::InputMethodV2(struct ::wl_client *client, uint32_t id, int version, CwlCompositor *compositor)
    :QtWaylandServer::zwp_input_method_v2(client, id, version)
{
	m_compositor = compositor;
}

void InputMethodV2::hidePanel()
{
	if(!m_panelHidden){
		this->send_deactivate();
		this->send_done();
		m_serial += 1;
		m_panelHidden = true;
	}

}

void InputMethodV2::zwp_input_method_v2_commit_string(Resource *resource, const QString &text)
{
	if(text == "\u21E6"){
		m_compositor->defaultSeat()->sendKeyEvent(Qt::Key_Backspace, 1);
	} else if(text == "\u21E5"){
		m_compositor->defaultSeat()->sendKeyEvent(Qt::Key_Tab, 1);
	} else if(text == "\u21D1"){
		m_compositor->defaultSeat()->sendKeyEvent(Qt::Key_Up, 1);
	} else if(text == "\u21D3"){
		m_compositor->defaultSeat()->sendKeyEvent(Qt::Key_Down, 1);
	} else if(text == "\u21D0"){
		m_compositor->defaultSeat()->sendKeyEvent(Qt::Key_Left, 1);
	} else if(text == "\u21D2"){
		m_compositor->defaultSeat()->sendKeyEvent(Qt::Key_Right, 1);
	} else if(text == "\u21B5"){
		m_compositor->defaultSeat()->sendKeyEvent(Qt::Key_Enter, 1);
	} else if(text == "CUTIE_SPACE"){
		m_compositor->defaultSeat()->sendKeyEvent(Qt::Key_Space, 1);
	}
	m_lastString = text;
}

void InputMethodV2::zwp_input_method_v2_set_preedit_string(Resource *resource, const QString &text, int32_t cursor_begin, int32_t cursor_end)
{

}

void InputMethodV2::zwp_input_method_v2_delete_surrounding_text(Resource *resource, uint32_t before_length, uint32_t after_length)
{

}

void InputMethodV2::zwp_input_method_v2_commit(Resource *resource, uint32_t serial)
{
	if(m_compositor->defaultSeat()->keyboardFocus() == nullptr)
		return;

	if(m_lastString == ""){
		return;;
	}

	if(m_lastString == "\u21E6"){
		m_compositor->defaultSeat()->sendKeyEvent(Qt::Key_Backspace, 0);
		m_lastString = "";
		return;
	} else if(m_lastString == "\u21E5"){
		m_compositor->defaultSeat()->sendKeyEvent(Qt::Key_Tab, 0);
		m_lastString = "";
		return;
	} else if(m_lastString == "\u21D1"){
		m_compositor->defaultSeat()->sendKeyEvent(Qt::Key_Up, 0);
		m_lastString = "";
		return;
	} else if(m_lastString == "\u21D3"){
		m_compositor->defaultSeat()->sendKeyEvent(Qt::Key_Down, 0);
		m_lastString = "";
		return;
	} else if(m_lastString == "\u21D0"){
		m_compositor->defaultSeat()->sendKeyEvent(Qt::Key_Left, 0);
		m_lastString = "";
		return;
	} else if(m_lastString == "\u21D2"){
		m_compositor->defaultSeat()->sendKeyEvent(Qt::Key_Right, 0);
		m_lastString = "";
		return;
	} else if(m_lastString == "\u21B5"){
		m_compositor->defaultSeat()->sendKeyEvent(Qt::Key_Enter, 0);
		m_lastString = "";
		return;
	} else if(m_lastString == "CUTIE_SPACE"){
		m_compositor->defaultSeat()->sendKeyEvent(Qt::Key_Space, 0);
		m_lastString = "";
		return;
	} else if(m_lastString == "Ctrl"){
		m_ctrlModifier = !m_ctrlModifier;
		m_lastString = "";
		return;
	} else if(m_lastString == "Alt"){
		m_altModifier = !m_altModifier;
		m_lastString = "";
		return;
	}

	if(m_ctrlModifier || m_altModifier){
		sendWithModifier();
		m_lastString = "";
		m_ctrlModifier = false;
		m_altModifier = false;
		return;
	}

	CwlView *v = m_compositor->findView(m_compositor->defaultSeat()->keyboardFocus());

	if(v->tiV1 != nullptr){
		v->tiV1->send_commit_string(0, m_lastString);
	} else if(v->tiV2 != nullptr){
		v->tiV2->send_commit_string(m_lastString);
	} else if(v->tiV3 != nullptr){
		v->tiV3->send_commit_string(m_lastString);
		v->tiV3->send_done(0);
	}
	m_lastString = "";
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
	this->send_activate();
	this->send_done();
	m_serial += 1;
	m_panelHidden = false;
}

void InputMethodV2::onHideInputPanel()
{
	this->send_deactivate();
	this->send_done();
	m_serial += 1;
	m_panelHidden = true;
}

void InputMethodV2::sendWithModifier()
{
	QKeySequence qtkey = QKeySequence(m_lastString);

	Qt::KeyboardModifiers modifiers;

	if(m_ctrlModifier)
		modifiers |= Qt::ControlModifier;

	if(m_altModifier)
		modifiers |= Qt::AltModifier;

    QKeyEvent *key_press = new QKeyEvent(QEvent::KeyPress, qtkey[0].key(), modifiers);
    QKeyEvent *key_release = new QKeyEvent(QEvent::KeyRelease, qtkey[0].key(), Qt::KeyboardModifiers{});

    m_compositor->defaultSeat()->sendFullKeyEvent(key_press);
    m_compositor->defaultSeat()->sendFullKeyEvent(key_release);

	key_press = new QKeyEvent(QEvent::KeyPress, Qt::Key_AltGr, Qt::KeyboardModifiers{});
	key_release = new QKeyEvent(QEvent::KeyRelease, Qt::Key_AltGr, Qt::KeyboardModifiers{});
	m_compositor->defaultSeat()->sendFullKeyEvent(key_press);
	m_compositor->defaultSeat()->sendFullKeyEvent(key_release);
}

bool InputMethodV2::isPanelHidden() {
	return m_panelHidden;
}
