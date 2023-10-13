#include <extensions/cutie-shell.h>
#include <QProcess>

CutieShell::CutieShell()
    :QWaylandClientExtensionTemplate(1)
{}

void CutieShell::execApp(const QString &path)
{
	QtWayland::cutie_shell_private::exec_app(path);
}