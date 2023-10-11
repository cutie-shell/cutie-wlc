#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <QObject>
#include <QSettings>
#include <QProcess>
#include <QDir>

class Launcher : public QObject
{
    Q_OBJECT

public:
    Launcher(QObject* parent = 0);

    Q_INVOKABLE void execApp(QString command);
    Q_INVOKABLE void loadAppList();

public Q_SLOTS:

signals:

private:
    static bool readDesktopFile(QIODevice &device, QSettings::SettingsMap &map);
    QSettings::Format desktopFormat;

};

#endif // LAUNCHER_H