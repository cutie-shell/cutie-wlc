#ifndef SCREENCOPY
#define SCREENCOPY

#include "wayland-util.h"

#include <QtWaylandCompositor/QWaylandCompositorExtensionTemplate>
#include <QtWaylandCompositor/QWaylandCompositor>
#include <QtWaylandCompositor/QWaylandSurface>

#include "qwayland-server-wlr-screencopy-unstable-v1.h"
#include <cutie-wlc.h>
#include <glwindow.h>

class ScreencopyManagerV1 : public QWaylandCompositorExtensionTemplate<ScreencopyManagerV1>
	, public QtWaylandServer::zwlr_screencopy_manager_v1

{
	Q_OBJECT
public:
	ScreencopyManagerV1(CwlCompositor *compositor);
	void initialize() override;

protected:
	void zwlr_screencopy_manager_v1_capture_output(Resource *resource, uint32_t id, int32_t overlay_cursor, struct ::wl_resource *output) override;

private:
	CwlCompositor *m_compositor;

};

class ScreencopyFrameV1 : public QWaylandCompositorExtensionTemplate<ScreencopyFrameV1>
	, public QtWaylandServer::zwlr_screencopy_frame_v1
{
	Q_OBJECT
public:
	ScreencopyFrameV1(struct ::wl_client *client, uint32_t id, int version);
	void setFrameBuffer(QImage image, uint64_t tv_sec, uint32_t tv_nsec);

protected:
	void zwlr_screencopy_frame_v1_copy(Resource *resource, struct ::wl_resource *buffer) override;

private:
	QImage m_frameBuffer;
	uint64_t m_tv_sec;
	uint32_t m_tv_nsec;
};


#endif //SCREENCOPY