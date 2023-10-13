#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtGui/QGuiApplication>
#include <QScreen>
#include <qpa/qplatformscreen.h>

#include "settings.h"

Settings::Settings(QObject *parent) : QObject(parent) {
    this->settingsStore = new QSettings("Cutie Community Project", "Cutie Shell");
    this->battery = new org::freedesktop::DBus::Properties(
        "org.freedesktop.UPower", "/org/freedesktop/UPower/devices/DisplayDevice",
        QDBusConnection::systemBus());    
    connect(this->battery, SIGNAL(PropertiesChanged(QString, QVariantMap, QStringList)), this, SLOT(onUPowerInfoChanged(QString, QVariantMap, QStringList)));
    
    udevInstance = udev_new();
    udevEnumerator = udev_enumerate_new(udevInstance);
    udev_enumerate_add_match_subsystem(udevEnumerator, "backlight");
    udev_enumerate_scan_devices(udevEnumerator);
    udevEntry = udev_enumerate_get_list_entry(udevEnumerator);
    const char *udevPath = udev_list_entry_get_name(udevEntry);
    udevDevice = udev_device_new_from_syspath(udevInstance, udevPath);
    if (!udevDevice)
        udevDevice = udev_device_new_from_syspath(udevInstance, "/sys/class/leds/lcd-backlight");
    p_maxBrightness = QString(udev_device_get_sysattr_value(udevDevice, "max_brightness")).toInt();

    ((QQmlApplicationEngine *)parent)->rootContext()->setContextProperty("screenBrightness", this->settingsStore->value("screenBrightness", 100));
}

unsigned int Settings::GetMaxBrightness() {
    return p_maxBrightness;
}

unsigned int Settings::GetBrightness() {
    return udevDevice ? QString(udev_device_get_sysattr_value(udevDevice, "brightness")).toInt() : 0;
}

void Settings::SetBrightness(unsigned int value) {
    if (udevDevice)
        udev_device_set_sysattr_value(udevDevice, "brightness", std::to_string(value).c_str());
	QPlatformScreen *pscreen = ((QGuiApplication *)QCoreApplication::instance())
			->screens().first()->handle();
	if (value == 0) {
		pscreen->setPowerState(QPlatformScreen::PowerStateOff);
	}
	if (value > 0 && pscreen->powerState() != QPlatformScreen::PowerStateOn) {
		pscreen->setPowerState(QPlatformScreen::PowerStateOn);
	}
	emit brightnessChanged(value);
}

void Settings::StoreBrightness(unsigned int value) {
    settingsStore->setValue("screenBrightness", value);
    settingsStore->sync();
}

void Settings::onUPowerInfoChanged(QString interface, QVariantMap, QStringList) {
    if (interface == "org.freedesktop.UPower.Device") {
        refreshBatteryInfo();
    }
}

void Settings::refreshBatteryInfo() {
    QVariantMap upower_display = this->battery->GetAll("org.freedesktop.UPower.Device");
    ((QQmlApplicationEngine *)parent())->rootContext()->setContextProperty("batteryStatus", upower_display);
}
