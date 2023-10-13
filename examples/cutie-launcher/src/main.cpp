#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QQmlContext>
#include <launcher.h>
#include <extensions/cutie-shell.h>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts, true);

    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    Launcher *launcher = new Launcher(&engine);
    engine.rootContext()->setContextProperty("launcher", launcher);

    CutieShell shellProtocol;
    qmlRegisterSingletonInstance<CutieShell>("Cutie.CutieWlc", 1, 0, "CutieShell", &shellProtocol);

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