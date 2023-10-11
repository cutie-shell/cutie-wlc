#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtGui/QGuiApplication>

#include <launcher.h>

Launcher::Launcher(QObject *parent) : QObject(parent) {
    this->desktopFormat = QSettings::registerFormat("desktop", readDesktopFile, nullptr);
}

bool Launcher::readDesktopFile(QIODevice &device, QSettings::SettingsMap &map) {
    QTextStream in(&device);
    QString header;
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.startsWith("[") && line.endsWith("]")) {
            header = line.sliced(1).chopped(1);
        } else if (line.contains("=")) {
            map.insert(header + "/" + line.split("=").at(0), 
                line.sliced(line.indexOf('=') + 1));
        } else if (!line.isEmpty() && !line.startsWith("#")) {
            return false;
        }
    }

    return true;
}

void Launcher::execApp(QString command)
{
    qputenv("CUTIE_SHELL", QByteArray("true"));
    qputenv("QT_QPA_PLATFORM", QByteArray("wayland"));
    qputenv("EGL_PLATFORM", QByteArray("wayland"));
    qunsetenv("QT_QPA_GENERIC_PLUGINS");
    qunsetenv("QT_SCALE_FACTOR");
    qputenv("WAYLAND_DISPLAY", QByteArray("wayland-1"));
    QStringList args = QStringList();
    args.append("-c");
    args.append(command);
    if (!QProcess::startDetached("bash", args))
        qDebug() << "Failed to run";
}

void Launcher::loadAppList() {
    QString xdgDataDirs = QString(qgetenv("XDG_DATA_DIRS"));
    QStringList dataDirList = xdgDataDirs.split(':');
    for (int dirI = 0; dirI < dataDirList.count(); dirI++) {
        QDir *curAppDir = new QDir(dataDirList.at(dirI) + "/applications");
        if (curAppDir->exists()) {
            QStringList entryFiles = curAppDir->entryList(QDir::Files);
            for (int fileI = 0; fileI < entryFiles.count(); fileI++) {
                QString curEntryFileName = entryFiles.at(fileI);
                QSettings *curEntryFile = new QSettings(dataDirList.at(dirI) + "/applications/" + curEntryFileName, desktopFormat);
                QString desktopType = curEntryFile->value("Desktop Entry/Type").toString();
                if (desktopType == "Application") {
                    QVariantMap appData;
                    QStringList keys = curEntryFile->allKeys();
                    foreach (QString key, keys) {
                        appData.insert(key, curEntryFile->value(key));
                    }
                    
                    QString appHidden = curEntryFile->value("Desktop Entry/Hidden").toString();
                    QString appNoDisplay = curEntryFile->value("Desktop Entry/NoDisplay").toString();
                    if (appHidden != "true" && appNoDisplay != "true")
                        QMetaObject::invokeMethod(((QQmlApplicationEngine *)parent())->rootObjects()[0], "addApp", Q_ARG(QVariant, appData));
                }
                delete curEntryFile;
            }
        }
        delete curAppDir;
    }
}