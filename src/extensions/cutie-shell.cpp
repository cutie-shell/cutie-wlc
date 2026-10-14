#include <cutie-shell.h>
#include <cutie-wlc.h>
#include <foreign-toplevel-management.h>
#include <screencopy.h>

#include <QtWaylandCompositor/QWaylandSeat>
#include <QtWaylandCompositor/QWaylandClient>

#include <QProcess>
#include <QRegularExpression>
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
	QWaylandCompositor *compositor =
		static_cast<QWaylandCompositor *>(extensionContainer());
	init(compositor->display(), 1);
}

void CutieShell::cutie_shell_private_exec_app(Resource *resource,
					      const QString &path)
{
	// Learn the app id of apps we started, once their toplevel has mapped
	for (CwlView *view : m_compositor->getToplevelViews()) {
		QWaylandSurface *surface = view->surface();
		if (!surface || !surface->client() || view->getAppId().isEmpty())
			continue;
		qint64 pid = surface->client()->processId();
		for (auto it = m_pendingPids.begin(); it != m_pendingPids.end();
		     ++it) {
			if (it.value() != pid)
				continue;
			m_appIds.insert(it.key(), view->getAppId());
			qDebug() << "cutie-shell: got app id" << view->getAppId()
				 << "for" << it.key() << "(pid" << pid << ")";
			m_pendingPids.erase(it);
			break;
		}
	}

	// Already running: focus it instead of starting a second instance
	QString appId = m_appIds.value(path);
	if (!appId.isEmpty()) {
		for (CwlView *view : m_compositor->getToplevelViews()) {
			if (view->getAppId() != appId)
				continue;
			CwlView *top = view->parentView() ? view->parentView() :
							    view;
			m_compositor->raise(top);
			if (top->getChildViews().size() > 0)
				m_compositor->defaultSeat()->setKeyboardFocus(
					top->getChildViews().last()->surface());
			qDebug() << "cutie-shell: focused running app" << appId;
			return;
		}
	}

	qputenv("CUTIE_SHELL", QByteArray("true"));
	qputenv("QT_QPA_PLATFORM", QByteArray("wayland"));
	qputenv("EGL_PLATFORM", QByteArray("wayland"));
	qunsetenv("QT_QPA_GENERIC_PLUGINS");
	qunsetenv("QT_SCALE_FACTOR");
	qputenv("WAYLAND_DISPLAY", m_compositor->socketName());

	// Let sh replace itself with the app so the pid we get is the app's.
	// Skip it for env assignments / compound commands, where exec breaks.
	bool simple = !path.trimmed().isEmpty() &&
		      !path.trimmed().split(' ').first().contains('=') &&
		      !path.contains(QRegularExpression("[;&|<>$`()]"));
	QStringList args = QStringList();
	args.append("-c");
	args.append(simple ? "exec " + path : path);
	qint64 pid = 0;
	if (!QProcess::startDetached("/bin/sh", args, QString(), &pid)) {
		qDebug() << "Failed to run";
		return;
	}
	m_pendingPids.insert(path, pid);
	qDebug() << "cutie-shell: launched" << path << "(pid" << pid << ")";
}

void CutieShell::cutie_shell_private_get_thumbnail(
	Resource *resource, uint32_t id, struct ::wl_resource *toplevel)
{
	auto *r = std::remove_pointer<ForeignToplevelHandleV1 *>::type::
		Resource::fromResource(toplevel);
	ForeignToplevelHandleV1 *toplevelHandle =
		static_cast<ForeignToplevelHandleV1 *>(r->object());
	ScreencopyFrameV1 *frame = new ScreencopyFrameV1(resource->client(), id,
							 resource->version());
	if (!toplevelHandle->view()->grabber())
		return;

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