#include <QGuiApplication>
#include <QScreen>
#include <glwindow.h>
#include <cutie-wlc.h>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QScreen *screen = QGuiApplication::primaryScreen();

    GlWindow glwindow;
    glwindow.resize(screen->size().width(), screen->size().height() - 150);
    CwlCompositor cwlcompositor(&glwindow);
    glwindow.show();

    return app.exec();
}