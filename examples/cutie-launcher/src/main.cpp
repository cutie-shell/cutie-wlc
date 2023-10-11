#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <launcher.h>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts, true);

    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    Launcher *launcher = new Launcher(&engine);
    engine.rootContext()->setContextProperty("launcher", launcher);

    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    launcher->loadAppList();

    return app.exec();
}