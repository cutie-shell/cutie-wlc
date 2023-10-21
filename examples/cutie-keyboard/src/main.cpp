#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QMetaEnum>
#include <QObject>

#include <cutiekeyboard.h>
#include <input-method-v2.h>

int main(int argc, char *argv[])
{

    
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);

    QScreen *screen = app.primaryScreen();
    QSize screenSize = screen->size();

    CutieKeyboard keyboard;

//    QObject::connect(keyboard.engine(), &QQmlEngine::quit, &keyboard, &CutieKeyboard::hideKeyboard);

//    InputMethodManagerV2 *input_method2 = new InputMethodManagerV2();

    // QObject::connect (input_method2, &InputMethodManagerV2::showKeyboard, &keyboard, &CutieKeyboard::showKeyboard);
    // QObject::connect (input_method2, &InputMethodManagerV2::hideKeyboard, &keyboard, &CutieKeyboard::hideKeyboard);

    return app.exec();
}
