#ifndef VIRTUAL_KEYBOARD
#define VIRTUAL_KEYBOARD

#include <wayland-util.h>

#include <QtWaylandCompositor/QWaylandCompositorExtensionTemplate>
#include <QtWaylandCompositor/QWaylandCompositor>
#include <QWaylandKeyboard>
#include <QWaylandSeat>
#include <QWaylandKeymap>
#include <qwayland-server-virtual-keyboard-unstable-v1.h>

class VirtualKeyboard;

class VirtualKeyboardManager : public QWaylandCompositorExtensionTemplate<VirtualKeyboardManager>
	, public QtWaylandServer::zwp_virtual_keyboard_manager_v1

{
	Q_OBJECT
public:
	VirtualKeyboardManager();
	VirtualKeyboardManager(QWaylandCompositor *compositor);
	void initialize() override;
signals:
	
public Q_SLOTS:
	
protected:
	void zwp_virtual_keyboard_manager_v1_create_virtual_keyboard(Resource *resource, struct ::wl_resource *seat, uint32_t id) override;

private:
    QWaylandCompositor *compositor;
    VirtualKeyboard *m_vkbd;
    QWaylandSeat *m_wlseat;
};

class VirtualKeyboard : public QWaylandCompositorExtensionTemplate<VirtualKeyboard>
	, public QtWaylandServer::zwp_virtual_keyboard_v1
{
	Q_OBJECT
public:
	VirtualKeyboard(struct ::wl_client *client, uint32_t id, int version);

signals:

protected:
	void zwp_virtual_keyboard_v1_key(Resource *resource, uint32_t time, uint32_t key, uint32_t state) override;

};

#endif //VIRTUAL_KEYBOARD