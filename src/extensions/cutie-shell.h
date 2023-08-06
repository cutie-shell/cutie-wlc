#ifndef CUTIE_SHELL
#define CUTIE_SHELL

#include "wayland-util.h"

#include <QtWaylandCompositor/QWaylandCompositorExtensionTemplate>
#include <QtWaylandCompositor/QWaylandCompositor>

#include "qwayland-server-cutie-shell-private.h"

class CutieShell : public QWaylandCompositorExtensionTemplate<CutieShell>
	, public QtWaylandServer::cutie_shell_private

{
	Q_OBJECT
public:
	CutieShell(QWaylandCompositor *compositor);
	void initialize() override;

signals:

public Q_SLOTS:

protected:
	void cutie_shell_private_exec_app(Resource *resource, const QString &path) override;

private:
	QWaylandCompositor *m_compositor;

};

#endif //CUTIE_SHELL