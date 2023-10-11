#ifndef BACKLIGHT_H
#define BACKLIGHT_H

#include <QObject>
#include <QDebug>

class Settings : public QObject
{<
    Q_OBJECT
    Q_PROPERTY (unsigned int brightness READ GetBrightness WRITE SetBrightness NOTIFY brightnessChanged)
    Q_PROPERTY (unsigned int maxBrightness READ GetMaxBrightness)
private:<
    org::cutie_shell::SettingsDaemon::Backlight *backlight;
    org::cutie_shell::SettingsDaemon::Atmosphere *atmosphere;
    org::freedesktop::DBus::Properties *battery;
    QSettings *settingsStore;
    
public:
    Settings(QObject* parent = 0);
    Q_INVOKABLE unsigned int GetMaxBrightness();
    Q_INVOKABLE unsigned int GetBrightness();
    Q_INVOKABLE void SetBrightness(unsigned int value);
    Q_INVOKABLE void StoreBrightness(unsigned int value);
    Q_INVOKABLE void setAtmospherePath(QString path);
    Q_INVOKABLE void setAtmosphereVariant(QString variant);
    void refreshBatteryInfo();
public Q_SLOTS:
    void onUPowerInfoChanged(QString interface, QVariantMap, QStringList);
    void onAtmospherePathChanged();
    Q_INVOKABLE void onAtmosphereVariantChanged();
signals:
    void brightnessChanged(unsigned int brightness);
};

#endif // BACKLIGHT_H