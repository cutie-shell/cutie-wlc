#pragma once

#include <QOpenGLWindow>
#include <QOpenGLTextureBlitter>
#include <QOpenGLTexture>
#include <QEventPoint>
#include <QPointer>
#include <QScopedPointer>

#include <cutie-wlc.h>
#include "gesture/gesture.h"

QT_BEGIN_NAMESPACE

class GlWindow : public QOpenGLWindow {
	Q_OBJECT
    public:
	GlWindow();
	void setCompositor(CwlCompositor *cwlcompositor);
	bool displayOff();
	void setDisplayOff(bool displayOff);
	inline CwlGesture *gesture()
	{
		return m_gesture.data();
	}

    signals:
	void glReady();
	void displayOffChanged(bool displayOff);

    protected:
	void initializeGL() override;
	void paintGL() override;

	void touchEvent(QTouchEvent *ev) override;
	void mouseMoveEvent(QMouseEvent *ev) override;
	void mousePressEvent(QMouseEvent *ev) override;
	void mouseReleaseEvent(QMouseEvent *ev) override;
	void keyPressEvent(QKeyEvent *event) override;
	void keyReleaseEvent(QKeyEvent *event) override;

    private:
	void renderView(CwlView *view);

	// Helper methods for rendering pipeline
	void setupRenderingContext();
	qreal calculateViewOpacity(CwlView *view) const;
	void renderViews(const QList<CwlView *> &views);

	QOpenGLTextureBlitter m_textureBlitter;
	GLenum m_currentTarget;
	QScopedPointer<QOpenGLTexture> m_wallpaper;

	QList<QEventPoint *> m_evPoint;
	bool m_displayOff = false;

	QPointer<CwlCompositor> m_cwlcompositor;
	QScopedPointer<CwlGesture> m_gesture;
};

QT_END_NAMESPACE
