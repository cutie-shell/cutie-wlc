#include <QGuiApplication>
#include <QScreen>
#include <glwindow.h>
#include <cutie-wlc.h>

int main(int argc, char *argv[])
{
	// TODO: Using dconf or something like it to get the scale factor would be preferred.
	int shellScaleFactor = qEnvironmentVariable("QT_SCALE_FACTOR").toInt();
	qunsetenv("QT_SCALE_FACTOR");
	qunsetenv("GTK_IM_MODULE");

	QGuiApplication app(argc, argv);

	QScreen *screen = QGuiApplication::primaryScreen();

	GlWindow glwindow;
	glwindow.resize(screen->size().width(), screen->size().height());
	CwlCompositor cwlcompositor(&glwindow);
	cwlcompositor.setScaleFactor(shellScaleFactor);
	glwindow.show();

	return app.exec();
}