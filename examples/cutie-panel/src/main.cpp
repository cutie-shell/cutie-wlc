#include <QGuiApplication>
#include <QQuickView>
#include <QIcon>
#include <LayerShellQt/shell.h>
#include <LayerShellQt/window.h>

#include "quicksettings.h"

int main(int argc, char *argv[])
{
    LayerShellQt::Shell::useLayerShell();

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QCoreApplication::setOrganizationName("Cutie Community Project");
    QCoreApplication::setApplicationName("Cutie Panel");

    QGuiApplication app(argc, argv);

    QQuickView view;

    LayerShellQt::Window *layerShell = LayerShellQt::Window::get(&view);
    layerShell->setLayer(LayerShellQt::Window::LayerOverlay);
    layerShell->setAnchors(LayerShellQt::Window::AnchorTop);
    layerShell->setKeyboardInteractivity(LayerShellQt::Window::KeyboardInteractivityNone);
    layerShell->setExclusiveZone(30);

    //window.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl("qrc:/main.qml"));
    view.setColor(QColor(Qt::transparent));
    //view.setColor(QColor("black"));

    QuickSettings *quicksettings = new QuickSettings(view.engine());
    view.engine()->rootContext()->setContextProperty("quicksettings", quicksettings);

    view.show();

    // QQmlApplicationEngine engine;

    // Settings *settings = new Settings(&engine);
    // settings->refreshBatteryInfo();

    // engine.rootContext()->setContextProperty("shellScaleFactor", shellScaleFactor);
    // engine.rootContext()->setContextProperty("settings", settings);
    // const QUrl url(QStringLiteral("qrc:/compositor.qml"));
    // QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
    //                  &app, [url](QObject *obj, const QUrl &objUrl) {
    //     if (!obj && url == objUrl)
    //         QCoreApplication::exit(-1);
    // }, Qt::QueuedConnection);
    // engine.load(url);

    return app.exec();
}
