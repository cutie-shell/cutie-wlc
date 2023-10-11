#include <QtGui/QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QScreen>
#include <qpa/qplatformscreen.h>

#include "quicksettings.h"

QuickSettings::QuickSettings(QObject *parent) : QObject(parent) {
    this->settingsStore = new QSettings("Cutie Community Project", "Cutie Shell");
    this->backlight = new org::cutie_shell::SettingsDaemon::Backlight(
        "org.cutie_shell.SettingsDaemon", "/backlight",
        QDBusConnection::systemBus());
    this->atmosphere = new org::cutie_shell::SettingsDaemon::Atmosphere(
        "org.cutie_shell.SettingsDaemon", "/atmosphere",
        QDBusConnection::systemBus());
    this->battery = new org::freedesktop::DBus::Properties(
        "org.freedesktop.UPower", "/org/freedesktop/UPower/devices/DisplayDevice",
        QDBusConnection::systemBus());    
    connect(this->battery, SIGNAL(PropertiesChanged(QString, QVariantMap, QStringList)), this, SLOT(onUPowerInfoChanged(QString, QVariantMap, QStringList)));
    connect(this->atmosphere, SIGNAL(PathChanged()), this, SLOT(onAtmospherePathChanged()));
    connect(this->atmosphere, SIGNAL(VariantChanged()), this, SLOT(onAtmosphereVariantChanged()));
    setAtmospherePath(this->settingsStore->value("atmospherePath", "/usr/share/atmospheres/city").toString());
    setAtmosphereVariant(this->settingsStore->value("atmosphereVariant", "dark").toString());
    onAtmospherePathChanged();
    onAtmosphereVariantChanged();
    ((QQmlApplicationEngine *)parent)->rootContext()->setContextProperty("screenBrightness", this->settingsStore->value("screenBrightness", 100));
}

QuickSettings::~QuickSettings() {}

unsigned int QuickSettings::GetMaxBrightness() {
    QDBusPendingReply<unsigned int> maxBrightnessReply = backlight->GetMaxBrightness();
    maxBrightnessReply.waitForFinished();
    if (maxBrightnessReply.isValid()) {
        return maxBrightnessReply.value();
    } else {
        return 0;
    }
}

unsigned int QuickSettings::GetBrightness() {
    QDBusPendingReply<unsigned int> brightnessReply = backlight->GetBrightness();
    brightnessReply.waitForFinished();
    if (brightnessReply.isValid()) {
        return brightnessReply.value();
    } else {
        return 0;
    }

}

void QuickSettings::SetBrightness(unsigned int value) {
    backlight->SetBrightness(value);
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

void QuickSettings::StoreBrightness(unsigned int value) {
    settingsStore->setValue("screenBrightness", value);
    settingsStore->sync();
}

void QuickSettings::onUPowerInfoChanged(QString interface, QVariantMap, QStringList) {
    if (interface == "org.freedesktop.UPower.Device") {
        refreshBatteryInfo();
    }
}

void QuickSettings::refreshBatteryInfo() {
    QVariantMap upower_display = this->battery->GetAll("org.freedesktop.UPower.Device");
    ((QQmlApplicationEngine *)parent())->rootContext()->setContextProperty("batteryStatus", upower_display);
}

void QuickSettings::onAtmospherePathChanged() {
    QString apath = this->atmosphere->GetPath();
    ((QQmlApplicationEngine *)parent())->rootContext()->setContextProperty("atmospherePath", apath);
    settingsStore->setValue("atmospherePath", apath);
    settingsStore->sync();
}

void QuickSettings::onAtmosphereVariantChanged() {
    QString avar = this->atmosphere->GetVariant();
    ((QQmlApplicationEngine *)parent())->rootContext()->setContextProperty("atmosphereVariant", avar);
    settingsStore->setValue("atmosphereVariant", avar);
    settingsStore->sync();
}

void QuickSettings::setAtmospherePath(QString path) {
    this->atmosphere->SetPath(path);
    settingsStore->setValue("atmospherePath", path);
    settingsStore->sync();
}

void QuickSettings::setAtmosphereVariant(QString variant) {
    this->atmosphere->SetVariant(variant);
    settingsStore->setValue("atmosphereVariant", variant);
    settingsStore->sync();
}