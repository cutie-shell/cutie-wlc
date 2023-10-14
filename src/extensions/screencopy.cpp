#include <screencopy.h>

#include <QDateTime>

ScreencopyManagerV1::ScreencopyManagerV1(CwlCompositor *compositor)
    :QWaylandCompositorExtensionTemplate(compositor)
{
    m_compositor = compositor;
}

void ScreencopyManagerV1::initialize()
{
	QWaylandCompositorExtensionTemplate::initialize();
    CwlCompositor *compositor = static_cast<CwlCompositor *>(extensionContainer());
    init(compositor->display(), 3);
}

void ScreencopyManagerV1::zwlr_screencopy_manager_v1_capture_output(Resource *resource, uint32_t id, int32_t overlay_cursor, struct ::wl_resource *output)
{
	ScreencopyFrameV1 *frame = new ScreencopyFrameV1(resource->client(), id, resource->version());
	QImage fb = m_compositor->glWindow()->grabFramebuffer();
	fb.convertTo(QImage::Format_ARGB32_Premultiplied);
	frame->setFrameBuffer(fb, QDateTime::currentSecsSinceEpoch(), (QDateTime::currentMSecsSinceEpoch()%1000)*1000000);
	frame->send_buffer(0 /* 0 = ARGB32 */, fb.width(), fb.height(), fb.bytesPerLine());
	frame->send_buffer_done();
}

ScreencopyFrameV1::ScreencopyFrameV1(wl_client *client, uint32_t id, int version)
    :QtWaylandServer::zwlr_screencopy_frame_v1(client, id, version)
{
}

void ScreencopyFrameV1::setFrameBuffer(QImage image, uint64_t tv_sec, uint32_t tv_nsec) {
	m_frameBuffer = image;
	m_tv_sec = tv_sec;
	m_tv_nsec = tv_nsec;
}

void ScreencopyFrameV1::zwlr_screencopy_frame_v1_copy(Resource *resource, struct ::wl_resource *buffer) {
	wl_shm_buffer *shmBuffer = wl_shm_buffer_get(buffer);
	if (!shmBuffer) {
		send_failed();
		return;
	}

	int width = wl_shm_buffer_get_width(shmBuffer);
	int height = wl_shm_buffer_get_height(shmBuffer);
	int bytesPerLine = wl_shm_buffer_get_stride(shmBuffer);
	if (width != m_frameBuffer.width() || height != m_frameBuffer.height() || bytesPerLine != m_frameBuffer.bytesPerLine()) {
		send_failed();
		return;
	}

	uchar *data = static_cast<uchar *>(wl_shm_buffer_get_data(shmBuffer));
	if (!data) {
		send_failed();
		return;
	}
    memcpy(data, m_frameBuffer.bits(), m_frameBuffer.sizeInBytes());

	send_flags(0);
	send_ready((m_tv_sec >> 32) & 0xFFFFFFFF, m_tv_sec, m_tv_nsec);
}
