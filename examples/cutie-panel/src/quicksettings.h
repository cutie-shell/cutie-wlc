#pragma once
#include <QDebug>
#include <QObject>
#include <QQmlEngine>
#include <QQmlContext>
#include <QSettings>

#include <libudev.h>

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
    void refreshBatteryInfo();

private:
    org::freedesktop::DBus::Properties *battery;
    QSettings *settingsStore;

    struct udev *udevInstance;
    struct udev_enumerate *udevEnumerator;
    struct udev_list_entry *udevEntry;
    struct udev_device *udevDevice;
    int p_maxBrightness;

public Q_SLOTS:
    void onUPowerInfoChanged(QString interface, QVariantMap, QStringList);

signals:
    void brightnessChanged(unsigned int brightness);
};