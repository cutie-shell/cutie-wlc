#include <cutie-shell.h>
#include <cutie-wlc.h>
#include <foreign-toplevel-management.h>
#include <screencopy.h>

#include <QProcess>
#include <QDateTime>
#include <QOpenGLTexture>

CutieShell::CutieShell(CwlCompositor *compositor)
	: QWaylandCompositorExtensionTemplate(compositor)
	, m_compositor(compositor)
{
	connect(compositor, &CwlCompositor::blurChanged, this,
		&CutieShell::onBlurChanged);
	connect(compositor, &CwlCompositor::specialKey, this,
		&CutieShell::onSpecialKey);
}

void CutieShell::initialize()
{
	QWaylandCompositorExtensionTemplate::initialize();

	// Validate extension container before casting
	QObject *container = extensionContainer();
	if (!container) {
		qWarning()
			<< "CutieShell::initialize: No extension container available";
		return;
	}

	QWaylandCompositor *compositor =
		static_cast<QWaylandCompositor *>(container);
	if (!compositor) {
		qWarning()
			<< "CutieShell::initialize: Failed to cast extension container to QWaylandCompositor";
		return;
	}

	init(compositor->display(), 1);
}

void CutieShell::cutie_shell_private_exec_app(Resource *resource,
					      const QString &path)
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

void CutieShell::cutie_shell_private_get_thumbnail(
	Resource *resource, uint32_t id, struct ::wl_resource *toplevel)
{
	// Validate resource parameters first
	if (!resource) {
		qWarning()
			<< "CutieShell::cutie_shell_private_get_thumbnail: Invalid resource";
		return;
	}

	if (!toplevel) {
		qWarning()
			<< "CutieShell::cutie_shell_private_get_thumbnail: Invalid toplevel resource";
		return;
	}

	auto *r = std::remove_pointer<ForeignToplevelHandleV1 *>::type::
		Resource::fromResource(toplevel);
	if (!r) {
		qWarning()
			<< "CutieShell::cutie_shell_private_get_thumbnail: Failed to get resource from toplevel";
		return;
	}

	// Get the object directly as ForeignToplevelHandleV1 - it should inherit from QObject
	ForeignToplevelHandleV1 *toplevelHandle =
		static_cast<ForeignToplevelHandleV1 *>(r->object());
	if (!toplevelHandle) {
		qWarning()
			<< "CutieShell::cutie_shell_private_get_thumbnail: Failed to cast to ForeignToplevelHandleV1";
		return;
	}

	// Validate view before proceeding
	if (!toplevelHandle->view()) {
		qWarning()
			<< "CutieShell::cutie_shell_private_get_thumbnail: No view available for toplevel";
		return;
	}

	ScreencopyFrameV1 *frame = new ScreencopyFrameV1(resource->client(), id,
							 resource->version());
	if (!frame) {
		qWarning()
			<< "CutieShell::cutie_shell_private_get_thumbnail: Failed to create ScreencopyFrameV1";
		return;
	}

	if (!toplevelHandle->view()->grabber()) {
		delete frame;
		return;
	}

	connect(toplevelHandle->view()->grabber(),
		&QWaylandSurfaceGrabber::success, [=](const QImage &_fb) {
			QImage fb = _fb.convertedTo(
				QImage::Format_ARGB32_Premultiplied);
			frame->setFrameBuffer(
				fb, QDateTime::currentSecsSinceEpoch(),
				(QDateTime::currentMSecsSinceEpoch() % 1000) *
					1000000);
			frame->send_buffer(0, fb.width(), fb.height(),
					   fb.bytesPerLine());
			frame->send_buffer_done();
			disconnect(toplevelHandle->view()->grabber(), nullptr,
				   nullptr, nullptr);
		});
	connect(toplevelHandle->view()->grabber(),
		&QWaylandSurfaceGrabber::failed,
		[=](QWaylandSurfaceGrabber::Error) {
			frame->send_buffer_done();
			disconnect(toplevelHandle->view()->grabber(), nullptr,
				   nullptr, nullptr);
		});

	toplevelHandle->view()->grabber()->grab();
}

void CutieShell::onBlurChanged(double blur)
{
	QMultiMapIterator<struct ::wl_client *, Resource *> i(resourceMap());
	while (i.hasNext()) {
		i.next();
		send_blur(i.value()->handle, wl_fixed_from_double(blur));
	}
}

void CutieShell::onSpecialKey(SpecialKey key)
{
	QMultiMapIterator<struct ::wl_client *, Resource *> i(resourceMap());
	while (i.hasNext()) {
		i.next();
		send_key(i.value()->handle, (uint32_t)key);
	}
}

void CutieShell::onThumbnailDamage(CwlView *view)
{
	if (resourceMap().isEmpty())
		return;

	QMultiMapIterator<struct ::wl_client *, Resource *> i(resourceMap());
	while (i.hasNext()) {
		i.next();
		ForeignToplevelHandleV1 *handle =
			m_compositor->foreignTlManagerV1()->handleForView(
				i.key(), view);
		if (!handle)
			continue;
		send_thumbnail_damage(i.value()->handle,
				      handle->resource()->handle);
	}
}
