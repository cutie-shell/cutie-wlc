#include <cutie-shell.h>
#include <cutie-wlc.h>
#include <foreign-toplevel-management.h>
#include <screencopy.h>

#include <QProcess>
#include <QDateTime>

CutieShell::CutieShell(CwlCompositor *compositor)
    : QWaylandCompositorExtensionTemplate(compositor)
    , m_compositor(compositor) {
    connect(compositor, &CwlCompositor::blurChanged, this, &CutieShell::onBlurChanged);
    connect(compositor, &CwlCompositor::specialKey, this, &CutieShell::onSpecialKey);
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

void CutieShell::cutie_shell_private_get_thumbnail(Resource *resource, uint32_t id, struct ::wl_resource *toplevel) {
    ForeignToplevelHandleV1 *toplevelHandle = static_cast<ForeignToplevelHandleV1*>(
        QtWaylandServer::zwlr_foreign_toplevel_handle_v1::Resource::fromResource(toplevel)->object());
    ScreencopyFrameV1 *frame = new ScreencopyFrameV1(resource->client(), id, resource->version());
	QImage fb = toplevelHandle->view()->currentBuffer().image();
	fb.convertTo(QImage::Format_ARGB32_Premultiplied);
	frame->setFrameBuffer(fb, QDateTime::currentSecsSinceEpoch(), (QDateTime::currentMSecsSinceEpoch()%1000)*1000000);
	frame->send_buffer(0 /* 0 = ARGB32 */, fb.width(), fb.height(), fb.bytesPerLine());
	frame->send_buffer_done();
}


void CutieShell::onBlurChanged(double blur) {
    QMultiMapIterator<struct ::wl_client*, Resource*> i(resourceMap());
	while (i.hasNext()) {
	    i.next();
	    send_blur(i.value()->handle, wl_fixed_from_double(blur));
	}
}

void CutieShell::onSpecialKey(SpecialKey key) {
    QMultiMapIterator<struct ::wl_client*, Resource*> i(resourceMap());
	while (i.hasNext()) {
	    i.next();
	    send_key(i.value()->handle, (uint32_t)key);
	}
}