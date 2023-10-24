#include <cutie-shell.h>
#include <QProcess>

CutieShell::CutieShell(QWaylandCompositor *compositor)
    :QWaylandCompositorExtensionTemplate(compositor)
{
    m_compositor = compositor;
}

void CutieShell::initialize()
{
	QWaylandCompositorExtensionTemplate::initialize();
    QWaylandCompositor *compositor = static_cast<QWaylandCompositor *>(extensionContainer());
    init(compositor->display(), 1);
}

void CutieShell::cutie_shell_private_exec_app(Resource *resource, const QString &path)
{
	qputenv("CUTIE_SHELL", QByteArray("true"));
    qputenv("QT_QPA_PLATFORM", QByteArray("wayland"));
    qputenv("EGL_PLATFORM", QByteArray("wayland"));
    qunsetenv("QT_QPA_GENERIC_PLUGINS");
    qunsetenv("QT_SCALE_FACTOR");
    qputenv("WAYLAND_DISPLAY", m_compositor->socketName());
    QStringList args = QStringList();
    args.append("-c");
    args.append(path);
    if (!QProcess::startDetached("/bin/sh", args))
        qDebug() << "Failed to run";
}