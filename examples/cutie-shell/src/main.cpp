#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDBusPendingReply>
#include "settings.h"
#include "hwbuttons.h"
#include "notifications.h"
#include "NotificationsAdaptor.h"
#include <QLoggingCategory>
#include <QIcon>
#include <QQuickView>
#include <LayerShellQt6/shell.h>
#include <LayerShellQt6/window.h>
#include "extensions/cutie-shell.h"

int main(int argc, char *argv[])
{
    LayerShellQt::Shell::useLayerShell();

    //int shellScaleFactor = qEnvironmentVariable("QT_SCALE_FACTOR", "1").toDouble();
    qunsetenv("QT_IM_MODULE");

    QIcon::setThemeName("hicolor");
    QIcon::setThemeSearchPaths(QStringList("/usr/share/icons"));

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QCoreApplication::setOrganizationName("Cutie Community Project");
    QCoreApplication::setApplicationName("Cutie Shell");

    QGuiApplication app(argc, argv);

    QQuickView view;

    LayerShellQt::Window *layerShell = LayerShellQt::Window::get(&view);
    layerShell->setLayer(LayerShellQt::Window::LayerBottom);
    layerShell->setAnchors(LayerShellQt::Window::AnchorTop);
    layerShell->setKeyboardInteractivity(LayerShellQt::Window::KeyboardInteractivityNone);
    layerShell->setExclusiveZone(-1);

    Settings *settings = new Settings(view.engine());
    //window.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl("qrc:/compositor.qml"));
    //view.setColor(QColor(Qt::transparent));
    view.engine()->rootContext()->setContextProperty("settings", settings);
    // view.engine()->rootContext()->setContextProperty("shellScaleFactor", shellScaleFactor);
    settings->refreshBatteryInfo();

//    Notifications *notifications = new Notifications(view.engine());
//    new NotificationsAdaptor(notifications);
//    QDBusConnection::sessionBus().registerObject("/org/freedesktop/Notifications", notifications);
//    QDBusConnection::sessionBus().registerService("org.freedesktop.Notifications");

    settings->loadAppList();
    // settings->autostart();

    // HWButtons *btns = new HWButtons(view.engine());
    // app.installEventFilter(btns);

    view.show();

    return app.exec();
}
