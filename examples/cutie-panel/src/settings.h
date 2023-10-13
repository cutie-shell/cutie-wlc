#ifndef BACKLIGHT_H
#define BACKLIGHT_H

#include <QObject>
#include <QDebug>

#include "dbus_interface.h"
#include <libudev.h>

class Settings : public QObject
{
    Q_OBJECT
    Q_PROPERTY (unsigned int brightness READ GetBrightness WRITE SetBrightness NOTIFY brightnessChanged)
    Q_PROPERTY (unsigned int maxBrightness READ GetMaxBrightness)
private:
    org::freedesktop::DBus::Properties *battery;
    QSettings *settingsStore;

    struct udev *udevInstance;
    struct udev_enumerate *udevEnumerator;
    struct udev_list_entry *udevEntry;
    struct udev_device *udevDevice;
    int p_maxBrightness;
    
public:
    Settings(QObject* parent = 0);
    Q_INVOKABLE unsigned int GetMaxBrightness();
    Q_INVOKABLE unsigned int GetBrightness();
    Q_INVOKABLE void SetBrightness(unsigned int value);
    Q_INVOKABLE void StoreBrightness(unsigned int value);
    void refreshBatteryInfo();
public Q_SLOTS:
    void onUPowerInfoChanged(QString interface, QVariantMap, QStringList);
signals:
    void brightnessChanged(unsigned int brightness);
};

#endif // BACKLIGHT_H