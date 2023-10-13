#ifndef CUTIE_SHELL
#define CUTIE_SHELL

#include "wayland-util.h"

#include <QtWaylandClient/QWaylandClientExtensionTemplate>
#include <QtWaylandClient/QtWaylandClient>

#include <QtQml/QQmlEngine>

#include "qwayland-cutie-shell-private.h"

class CutieShell : public QWaylandClientExtensionTemplate<CutieShell>
	, public QtWayland::cutie_shell_private

{
	Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
public:
	CutieShell();
	Q_INVOKABLE void execApp(const QString &path);

};

#endif //CUTIE_SHELL