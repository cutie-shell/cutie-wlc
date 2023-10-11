#pragma once
#include <QDebug>
#include <QObject>
#include <QQmlEngine>
#include <QQmlContext>
#include <QSettings>

#include "cutie_shell_interface.h"
#include "dbus_interface.h"

class QuickSettings : public QObject
{
    Q_OBJECT
    Q_PROPERTY (unsigned int brightness READ GetBrightness WRITE SetBrightness NOTIFY brightnessChanged)
    Q_PROPERTY (unsigned int maxBrightness READ GetMaxBrightness)
    
public:
    QuickSettings(QObject* parent = 0);
    ~QuickSettings();
    Q_INVOKABLE unsigned int GetMaxBrightness();
    Q_INVOKABLE unsigned int GetBrightness();
    Q_INVOKABLE void SetBrightness(unsigned int value);
    Q_INVOKABLE void StoreBrightness(unsigned int value);
    // Q_INVOKABLE void execApp(QString command);
    Q_INVOKABLE void setAtmospherePath(QString path);
    Q_INVOKABLE void setAtmosphereVariant(QString variant);
    void refreshBatteryInfo();
    // void autostart();
    // Q_INVOKABLE void loadAppList();

private:
    org::cutie_shell::SettingsDaemon::Backlight *backlight;
    org::cutie_shell::SettingsDaemon::Atmosphere *atmosphere;
    org::freedesktop::DBus::Properties *battery;
    QSettings *settingsStore;

public Q_SLOTS:
    void onUPowerInfoChanged(QString interface, QVariantMap, QStringList);
    void onAtmospherePathChanged();
    Q_INVOKABLE void onAtmosphereVariantChanged();

signals:
    void brightnessChanged(unsigned int brightness);
};