#include <virtual-keyboard-v1.h>
#include <QtGui/private/qxkbcommon_p.h>

VirtualKeyboardManager::VirtualKeyboardManager()
{
}

VirtualKeyboardManager::VirtualKeyboardManager(QWaylandCompositor *compositor)
    :QWaylandCompositorExtensionTemplate(compositor)
{
}

void VirtualKeyboardManager::initialize()
{
	QWaylandCompositorExtensionTemplate::initialize();
    QWaylandCompositor *compositor = static_cast<QWaylandCompositor *>(extensionContainer());
    init(compositor->display(), 1);
}

void VirtualKeyboardManager::zwp_virtual_keyboard_manager_v1_create_virtual_keyboard(Resource *resource, struct ::wl_resource *seat, uint32_t id)
{
    
}

VirtualKeyboard::VirtualKeyboard(wl_client *client, uint32_t id, int version)
    :QtWaylandServer::zwp_virtual_keyboard_v1(client, id, version)
{
}

void VirtualKeyboard::zwp_virtual_keyboard_v1_key(Resource *resource, uint32_t time, uint32_t key, uint32_t state)
{

}