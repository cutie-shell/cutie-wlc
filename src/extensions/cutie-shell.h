#ifndef CUTIE_SHELL
#define CUTIE_SHELL

#include "wayland-util.h"

#include <QtWaylandCompositor/QWaylandCompositorExtensionTemplate>
#include <QtWaylandCompositor/QWaylandCompositor>

#include "qwayland-server-cutie-shell-private.h"

class CwlCompositor;
class CwlView;

class CutieShell : public QWaylandCompositorExtensionTemplate<CutieShell>,
		   public QtWaylandServer::cutie_shell_private

{
	Q_OBJECT
    public:
	enum SpecialKey : uint32_t {
		POWER_PRESS = 0,
		POWER_RELEASE = 1,
		VOLUME_UP_PRESS = 2,
		VOLUME_UP_RELEASE = 3,
		VOLUME_DOWN_PRESS = 4,
		VOLUME_DOWN_RELEASE = 5
	};
	
	CutieShell(CwlCompositor *compositor);
	void initialize() override;

    signals:

    public Q_SLOTS:
	void onBlurChanged(double blur);
	void onSpecialKey(SpecialKey key);
	void onThumbnailDamage(CwlView *view);

    protected:
	void cutie_shell_private_exec_app(Resource *resource,
					  const QString &path) override;
	void cutie_shell_private_get_thumbnail(
		Resource *resource, uint32_t id,
		struct ::wl_resource *toplevel) override;

    private:
	CwlCompositor *m_compositor;
};

#endif //CUTIE_SHELL
