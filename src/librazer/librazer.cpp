/*
 * Copyright (C) 2016-2017  Luca Weiss <luca (at) z3ntu (dot) xyz>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <QDBusMessage>
#include <QDBusConnection>
#include <QDebug>
#include <QDomDocument>
//TODO Remove QDBusArgument include
#include <QDBusArgument>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariantHash>
#include <QtGui/qcolor.h>

#include <iostream>
#include <unistd.h> // TODO Remove again

#include "librazer.h"

namespace librazer
{

/**
 * Fill "introspection" variable with data from the dbus introspection xml
 */
void Device::Introspect()
{
    QStringList intr;

    QDBusMessage m = prepareDeviceQDBusMessage("org.freedesktop.DBus.Introspectable", "Introspect");
    QDomDocument doc = QDBusMessageToXML(m);

    QDomNodeList nodes = doc.documentElement().childNodes();
    for(int i = 0; i<nodes.count(); i++) {
        // Check if "interface" and also not org.freedesktop.DBus.Introspectable
        QDomElement element = nodes.at(i).toElement();
        QString interfacename = element.attributeNode("name").value();

        QDomNodeList methodnodes = element.childNodes();
        for(int ii = 0; ii<methodnodes.count(); ii++) {
            QDomElement methodelement = methodnodes.at(ii).toElement();
            intr.append(interfacename + ";" + methodelement.attributeNode("name").value());
        }
        intr.append(interfacename);
    }
    introspection = intr;
}

/**
 * Fill "capabilities" list with the capabilities of the device. Names are from the pylib, parsed with the script ./capabilities_to_cpp.sh in the root of this repo.
 */
void Device::setupCapabilities()
{
    capabilities.insert("name", true);
    capabilities.insert("type", true);
    capabilities.insert("firmware_version", true);
    capabilities.insert("serial", true);
    capabilities.insert("dpi", hasCapabilityInternal("razer.device.dpi", "setDPI"));
    capabilities.insert("brightness", hasCapabilityInternal("razer.device.lighting.brightness"));
    capabilities.insert("get_brightness", hasCapabilityInternal("razer.device.lighting.brightness", "setBrightness"));
    capabilities.insert("battery", hasCapabilityInternal("razer.device.power"));
    capabilities.insert("poll_rate", hasCapabilityInternal("razer.device.misc", "setPollRate"));

    capabilities.insert("macro_logic", hasCapabilityInternal("razer.device.macro"));

    // Default device is a chroma so lighting capabilities
    capabilities.insert("lighting", hasCapabilityInternal("razer.device.lighting.chroma"));
    capabilities.insert("lighting_breath_single", hasCapabilityInternal("razer.device.lighting.chroma", "setBreathSingle"));
    capabilities.insert("lighting_breath_dual", hasCapabilityInternal("razer.device.lighting.chroma", "setBreathDual"));
    capabilities.insert("lighting_breath_triple", hasCapabilityInternal("razer.device.lighting.chroma", "setBreathTriple"));
    capabilities.insert("lighting_breath_random", hasCapabilityInternal("razer.device.lighting.chroma", "setBreathRandom"));
    capabilities.insert("lighting_charging", hasCapabilityInternal("razer.device.lighting.power"));
    capabilities.insert("lighting_wave", hasCapabilityInternal("razer.device.lighting.chroma", "setWave"));
    capabilities.insert("lighting_reactive", hasCapabilityInternal("razer.device.lighting.chroma", "setReactive"));
    capabilities.insert("lighting_none", hasCapabilityInternal("razer.device.lighting.chroma", "setNone"));
    capabilities.insert("lighting_spectrum", hasCapabilityInternal("razer.device.lighting.chroma", "setSpectrum"));
    capabilities.insert("lighting_static", hasCapabilityInternal("razer.device.lighting.chroma", "setStatic"));
    capabilities.insert("lighting_ripple", hasCapabilityInternal("razer.device.lighting.custom", "setRipple")); // Thinking of extending custom to do more hence the key check
    capabilities.insert("lighting_ripple_random", hasCapabilityInternal("razer.device.lighting.custom", "setRippleRandomColour"));

    capabilities.insert("lighting_pulsate", hasCapabilityInternal("razer.device.lighting.chroma", "setPulsate"));

    capabilities.insert("lighting_led_matrix", hasMatrix());
    capabilities.insert("lighting_led_single", hasCapabilityInternal("razer.device.lighting.chroma", "setKey"));

    // Mouse lighting attrs
    capabilities.insert("lighting_logo", hasCapabilityInternal("razer.device.lighting.logo"));
    capabilities.insert("lighting_logo_active", hasCapabilityInternal("razer.device.lighting.logo", "setLogoActive"));
    capabilities.insert("lighting_logo_blinking", hasCapabilityInternal("razer.device.lighting.logo", "setLogoBlinking"));
    capabilities.insert("lighting_logo_brightness", hasCapabilityInternal("razer.device.lighting.logo", "setLogoBrightness"));
    capabilities.insert("get_lighting_logo_brightness", hasCapabilityInternal("razer.device.lighting.logo", "getLogoBrightness"));
    capabilities.insert("lighting_logo_pulsate", hasCapabilityInternal("razer.device.lighting.logo", "setLogoPulsate"));
    capabilities.insert("lighting_logo_spectrum", hasCapabilityInternal("razer.device.lighting.logo", "setLogoSpectrum"));
    capabilities.insert("lighting_logo_static", hasCapabilityInternal("razer.device.lighting.logo", "setLogoStatic"));
    capabilities.insert("lighting_logo_none", hasCapabilityInternal("razer.device.lighting.logo", "setLogoNone"));
    capabilities.insert("lighting_logo_reactive", hasCapabilityInternal("razer.device.lighting.logo", "setLogoReactive"));
    capabilities.insert("lighting_logo_breath_single", hasCapabilityInternal("razer.device.lighting.logo", "setLogoBreathSingle"));
    capabilities.insert("lighting_logo_breath_dual", hasCapabilityInternal("razer.device.lighting.logo", "setLogoBreathDual"));
    capabilities.insert("lighting_logo_breath_random", hasCapabilityInternal("razer.device.lighting.logo", "setLogoBreathRandom"));

    capabilities.insert("lighting_scroll", hasCapabilityInternal("razer.device.lighting.scroll"));
    capabilities.insert("lighting_scroll_active", hasCapabilityInternal("razer.device.lighting.scroll", "setScrollActive"));
    capabilities.insert("lighting_scroll_blinking", hasCapabilityInternal("razer.device.lighting.scroll", "setScrollBlinking"));
    capabilities.insert("lighting_scroll_brightness", hasCapabilityInternal("razer.device.lighting.scroll", "setScrollBrightness"));
    capabilities.insert("get_lighting_scroll_brightness", hasCapabilityInternal("razer.device.lighting.scroll", "getScrollBrightness"));
    capabilities.insert("lighting_scroll_pulsate", hasCapabilityInternal("razer.device.lighting.scroll", "setScrollPulsate"));
    capabilities.insert("lighting_scroll_spectrum", hasCapabilityInternal("razer.device.lighting.scroll", "setScrollSpectrum"));
    capabilities.insert("lighting_scroll_static", hasCapabilityInternal("razer.device.lighting.scroll", "setScrollStatic"));
    capabilities.insert("lighting_scroll_none", hasCapabilityInternal("razer.device.lighting.scroll", "setScrollNone"));
    capabilities.insert("lighting_scroll_reactive", hasCapabilityInternal("razer.device.lighting.scroll", "setScrollReactive"));
    capabilities.insert("lighting_scroll_breath_single", hasCapabilityInternal("razer.device.lighting.scroll", "setScrollBreathSingle"));
    capabilities.insert("lighting_scroll_breath_dual", hasCapabilityInternal("razer.device.lighting.scroll", "setScrollBreathDual"));
    capabilities.insert("lighting_scroll_breath_random", hasCapabilityInternal("razer.device.lighting.scroll", "setScrollBreathRandom"));
}

bool isDaemonRunning()
{
    QDBusMessage m = prepareGeneralQDBusMessage("razer.daemon", "version");
    QDBusMessage msg = QDBusConnection::sessionBus().call(m);
    if(msg.type() == QDBusMessage::ReplyMessage) {
        return true;
    } else {
        return false;
    }
}

/**
 * Gets a list of connected devices in form of their serial number.
 * Can be used to create a 'Device' object and get further information about the devices.
 */
QStringList getConnectedDevices()
{
    QDBusMessage m = prepareGeneralQDBusMessage("razer.devices", "getDevices");
    return QDBusMessageToStringList(m);
}

/**
 * If devices should sync effects.
 * Example: Set it to 'on', set the lighting on one device to something, other devices connected will automatically get set to the same effect.
 */
bool syncEffects(bool yes)
{
    QDBusMessage m = prepareGeneralQDBusMessage("razer.devices", "syncEffects");

    // Set arguments
    QList<QVariant> args;
    //yes ? args.append("True") : args.append("False"); // maybe bool works here
    args.append(yes);
    m.setArguments(args);

    bool queued = QDBusConnection::sessionBus().send(m);
    qDebug() << "Queued: " << queued;
    return queued;
}

/**
 * Returns the daemon version currently running.
 */
bool getSyncEffects()
{
    QDBusMessage m = prepareGeneralQDBusMessage("razer.devices", "getSyncEffects");
    return QDBusMessageToBool(m);
}

/**
 * Returns the daemon version currently running.
 */
QString getDaemonVersion()
{
    QDBusMessage m = prepareGeneralQDBusMessage("razer.daemon", "version");
    return QDBusMessageToString(m);
}

/**
 * Stops the daemon. WARNING: FURTHER COMMUNICATION WILL NOT BE POSSIBLE.
 */
bool stopDaemon()
{
    QDBusMessage m = prepareGeneralQDBusMessage("razer.daemon", "stop");
    return QDBusMessageToVoid(m);
}

/**
 * Sets if the Chroma lighting should turn off if the screensaver is turned on.
 */
bool setTurnOffOnScreensaver(bool turnOffOnScreensaver)
{
    QDBusMessage m = prepareGeneralQDBusMessage("razer.devices", "enableTurnOffOnScreensaver");
    QList<QVariant> args;
    args.append(turnOffOnScreensaver);
    m.setArguments(args);

    return QDBusMessageToVoid(m);
}

/**
 * Gets if the Chroma lighting should turn off if the screensaver is turned on.
 */
bool getTurnOffOnScreensaver()
{
    QDBusMessage m = prepareGeneralQDBusMessage("razer.devices", "getOffOnScreensaver");
    return QDBusMessageToBool(m);
}

// ====== DEVICE CLASS ======

/**
 * Constructs a new device object with the given serial.
 */
Device::Device(QString s)
{
    serial = s;
    Introspect();
    setupCapabilities();
}

/**
 * Destructor
 */
Device::~Device()
{
    //TODO Write
}

// TODO New Qt5 connect style syntax
bool connectDeviceAdded(QObject *receiver, const char *slot)
{
    return QDBusConnection::sessionBus().connect("org.razer", "/org/razer", "razer.devices", "device_added", receiver, slot);
}

// TODO New Qt5 connect style syntax
bool connectDeviceRemoved(QObject *receiver, const char *slot)
{
    return QDBusConnection::sessionBus().connect("org.razer", "/org/razer", "razer.devices", "device_removed", receiver, slot);
}

// ---- MISC METHODS ----
/**
 * Internal method to determine whether a device has a given capability based on interface and method names.
 */
bool Device::hasCapabilityInternal(const QString &interface, const QString &method)
{
    if(method.isNull()) {
        return introspection.contains(interface);
    }
    return introspection.contains(interface + ";" + method);
}

/**
 * Returns if a device has a given capability.
 */
bool Device::hasCapability(const QString &name)
{
    return capabilities.value(name);
}

/**
 * Returns a QHash object with all capabilities.
 */
QHash<QString, bool> Device::getAllCapabilities()
{
    return capabilities;
}

/**
 * Returns the .png filename for pictures.
 * Could return an empty string (qstring.isEmpty()) if no picture was found.
 */
QString Device::getPngFilename()
{
    return getRazerUrls().value("top_img").toString().split("/").takeLast();
}

/**
 * Returns the download url for pictures.
 * Could return an empty string (qstring.isEmpty()) if no picture was found.
 */
QString Device::getPngUrl()
{
    return getRazerUrls().value("top_img").toString();
}


// ----- DBUS METHODS -----

/**
 * Returns a human readable device name like "Razer DeathAdder Chroma" or "Razer Kraken 7.1 (Rainie)".
 */
QString Device::getDeviceName()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.misc", "getDeviceName");
    return QDBusMessageToString(m);
}

/**
 * Returns the type of the device. Could be one of 'headset', 'mouse', 'mug', 'keyboard', 'tartarus' or another type, if added to the daemon.
 */
QString Device::getDeviceType()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.misc", "getDeviceType");
    return QDBusMessageToString(m);
}

/**
 * Returns the firmware version of the device.
 */
QString Device::getFirmwareVersion()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.misc", "getFirmware");
    return QDBusMessageToString(m);
}

/**
 * Returns a QVariantHash (QHash\<QString, QVariant>).
 * Most likely contains keys "top_img", "side_img", "store", "perspective_img".
 * Values are QVariant\<QString> with a full url as value.
 */
QVariantHash Device::getRazerUrls()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.misc", "getRazerUrls");
    QString ret = QDBusMessageToString(m);
    return QJsonDocument::fromJson(ret.toUtf8()).object().toVariantHash();
}

/**
 * Return usb vendor id as integer in decimal notation. Should always be 5426 (-> Hex 1532)
 */
int Device::getVid()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.misc", "getVidPid");
    return QDBusMessageToIntArray(m)[0];
}

/**
 * Return usb product id as integer in decimal notation.
 */
int Device::getPid()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.misc", "getVidPid");
    return QDBusMessageToIntArray(m)[1];
}

/**
 * Returns if the device has dedicated macro keys.
 */
bool Device::hasDedicatedMacroKeys()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.misc", "hasDedicatedMacroKeys");
    return QDBusMessageToBool(m);
}

/**
 * Returns if the device has a matrix. Dimensions can be gotten with getMatrixDimensions()
 */
bool Device::hasMatrix()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.misc", "hasMatrix");
    return QDBusMessageToBool(m);
}

/**
 * Returns the matrix dimensions. If the device has no matrix, it will return -1 for both numbers.
 */
QList<int> Device::getMatrixDimensions()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.misc", "getMatrixDimensions");
    return QDBusMessageToIntArray(m);
}

/**
 * Returns the poll rate.
 */
int Device::getPollRate()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.misc", "getPollRate");
    return QDBusMessageToInt(m);
}

/**
 * Sets the poll rate. Use one of librazer::POLL_125HZ, librazer::POLL_500HZ or librazer::POLL_1000HZ.
 */
bool Device::setPollRate(ushort pollrate)
{
    if(pollrate != POLL_125HZ && pollrate != POLL_500HZ && pollrate != POLL_1000HZ) {
        qDebug() << "setPollRate(): Has to be one of librazer::POLL_125HZ, librazer::POLL_500HZ or librazer::POLL_1000HZ";
        return false;
    }
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.misc", "setPollRate");
    QList<QVariant> args;
    args.append(pollrate);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/**
 * Sets the DPI.
 */
bool Device::setDPI(int dpi_x, int dpi_y)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.dpi", "setDPI");
    QList<QVariant> args;
    args.append(dpi_x);
    args.append(dpi_y);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/**
 * Returns the DPI.
 */
QList<int> Device::getDPI()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.dpi", "getDPI");
    return QDBusMessageToIntArray(m);
}

/**
 * Returns the maximum DPI possible for the device.
 */
int Device::maxDPI()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.dpi", "maxDPI");
    return QDBusMessageToInt(m);
}

// BATTERY
/**
 * Returns if the device is charging.
 */
bool Device::isCharging()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.power", "isCharging");
    return QDBusMessageToBool(m);
}

/**
 * Returns the battery level between 0 and 100. Could maybe be -1 ???
 */
double Device::getBatteryLevel()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.power", "getBattery");
    return QDBusMessageToDouble(m);
}

/**
 * Sets the idle time of the device.
 */
bool Device::setIdleTime(ushort idle_time)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.power", "setIdleTime");
    QList<QVariant> args;
    args.append(idle_time);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/**
 * Sets the low battery threshold.
 */
bool Device::setLowBatteryThreshold(uchar threshold)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.power", "setLowBatteryThreshold");
    QList<QVariant> args;
    args.append(threshold);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

// ------ LIGHTING EFFECTS ------

/**
 * Sets the normal lighting to static lighting.
 */
bool Device::setStatic(uchar r, uchar g, uchar b)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.chroma", "setStatic");
    QList<QVariant> args;
    args.append(r);
    args.append(g);
    args.append(b);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/**
 * Sets the normal lighting to random breath lighting.
 */
bool Device::setBreathSingle(uchar r, uchar g, uchar b)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.chroma", "setBreathSingle");
    QList<QVariant> args;
    args.append(r);
    args.append(g);
    args.append(b);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/**
 * Sets the normal lighting to dual breath lighting.
 */
bool Device::setBreathDual(uchar r, uchar g, uchar b, uchar r2, uchar g2, uchar b2)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.chroma", "setBreathDual");
    QList<QVariant> args;
    args.append(r);
    args.append(g);
    args.append(b);
    args.append(r2);
    args.append(g2);
    args.append(b2);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

bool Device::setBreathTriple(uchar r, uchar g, uchar b, uchar r2, uchar g2, uchar b2, uchar r3, uchar g3, uchar b3)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.chroma", "setBreathTriple");
    QList<QVariant> args;
    args.append(r);
    args.append(g);
    args.append(b);
    args.append(r2);
    args.append(g2);
    args.append(b2);
    args.append(r3);
    args.append(g3);
    args.append(b3);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/**
 * Sets the normal lighting to random breath lighting.
 */
bool Device::setBreathRandom()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.chroma", "setBreathRandom");
    return QDBusMessageToVoid(m);
}

/**
 * Sets the lighting to reactive mode.
 * Use one of librazer::REACTIVE_* for speed
 */
bool Device::setReactive(uchar r, uchar g, uchar b, uchar speed)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.chroma", "setReactive");
    QList<QVariant> args;
    args.append(r);
    args.append(g);
    args.append(b);
    args.append(speed);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/**
 * Sets the lighting to spectrum mode.
 */
bool Device::setSpectrum()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.chroma", "setSpectrum");
    return QDBusMessageToVoid(m);
}

/**
 * Use librazer::WAVE_RIGHT & librazer::WAVE_LEFT
 */
bool Device::setWave(const int direction)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.chroma", "setWave");
    QList<QVariant> args;
    args.append(direction);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/**
 * Sets the lighting to "off"
 */
bool Device::setNone()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.chroma", "setNone");
    return QDBusMessageToVoid(m);
}

bool Device::setPulsate()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.bw2013", "setPulsate");
    return QDBusMessageToVoid(m);
}

bool Device::setCustom()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.chroma", "setCustom");
    return QDBusMessageToVoid(m);
}

bool Device::setKeyRow(uchar row, uchar startcol, uchar endcol, QList<QColor> colors)
{
    /*
    if(colors.count() != (endcol+1)-startcol) {
        qWarning() << "Invalid 'colors' length. startcol:" << startcol << " - endcol:" << endcol << " needs " << (endcol+1)-startcol << " entries in colors!";
        return false;
    }
    int len = (endcol+1)-startcol + ((endcol+1)-startcol*3);
    */
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.chroma", "setKeyRow");

    QByteArray parameters;
    parameters[0] = row;
    parameters[1] = startcol;
    parameters[2] = endcol;
    int counter = 3;
    foreach(QColor c, colors) {
        // set the rgb to the parameters[i]
        parameters[counter++] = c.red();
        parameters[counter++] = c.green();
        parameters[counter++] = c.blue();
    }
    qDebug() << parameters;

    QList<QVariant> args;
    args.append(parameters);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

bool Device::setRipple(uchar r, uchar g, uchar b, double refresh_rate)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.custom", "setRipple");
    QList<QVariant> args;
    args.append(r);
    args.append(g);
    args.append(b);
    args.append(refresh_rate);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

bool Device::setRippleRandomColor(double refresh_rate)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.custom", "setRippleRandomColour");
    QList<QVariant> args;
    args.append(refresh_rate);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/**
 * Sets the brightness (0-100).
 */
bool Device::setBrightness(double brightness)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.brightness", "setBrightness");
    QList<QVariant> args;
    args.append(brightness);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/**
 * Gets the current brightness (0-100).
 */
double Device::getBrightness()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.brightness", "getBrightness");
    return QDBusMessageToDouble(m);
}

/**
 * Sets the logo to static lighting.
 */
bool Device::setLogoStatic(uchar r, uchar g, uchar b)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.logo", "setLogoStatic");
    QList<QVariant> args;
    args.append(r);
    args.append(g);
    args.append(b);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

bool Device::setLogoActive(bool active)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.logo", "setLogoActive");
    QList<QVariant> args;
    args.append(active);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

bool Device::getLogoActive()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.logo", "getLogoActive");
    return QDBusMessageToBool(m);
}

uchar Device::getLogoEffect()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.logo", "getLogoEffect");
    return QDBusMessageToByte(m);
}

bool Device::setLogoBlinking(uchar r, uchar g, uchar b)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.logo", "setLogoBlinking");
    QList<QVariant> args;
    args.append(r);
    args.append(g);
    args.append(b);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

bool Device::setLogoPulsate(uchar r, uchar g, uchar b)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.logo", "setLogoPulsate");
    QList<QVariant> args;
    args.append(r);
    args.append(g);
    args.append(b);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

bool Device::setLogoSpectrum()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.logo", "setLogoSpectrum");
    return QDBusMessageToVoid(m);
}

bool Device::setLogoNone()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.logo", "setLogoNone");
    return QDBusMessageToVoid(m);
}

bool Device::setLogoReactive(uchar r, uchar g, uchar b, uchar speed)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.logo", "setLogoReactive");
    QList<QVariant> args;
    args.append(r);
    args.append(g);
    args.append(b);
    args.append(speed);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

bool Device::setLogoBreathSingle(uchar r, uchar g, uchar b)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.logo", "setLogoBreathSingle");
    QList<QVariant> args;
    args.append(r);
    args.append(g);
    args.append(b);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

bool Device::setLogoBreathDual(uchar r, uchar g, uchar b, uchar r2, uchar g2, uchar b2)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.logo", "setLogoBreathSingle");
    QList<QVariant> args;
    args.append(r);
    args.append(g);
    args.append(b);
    args.append(r2);
    args.append(g2);
    args.append(b2);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

bool Device::setLogoBreathRandom()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.logo", "setLogoBreathRandom");
    return QDBusMessageToVoid(m);
}

/**
 * Sets the logo brightness (0-100).
 */
bool Device::setLogoBrightness(double brightness)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.logo", "setLogoBrightness");
    QList<QVariant> args;
    args.append(brightness);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/**
 * Gets the current logo brightness (0-100).
 */
double Device::getLogoBrightness()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.logo", "getLogoBrightness");
    return QDBusMessageToDouble(m);
}

/**
 * Sets the scrollwheel to static lighting.
 */
bool Device::setScrollStatic(uchar r, uchar g, uchar b)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.scroll", "setScrollStatic");
    QList<QVariant> args;
    args.append(r);
    args.append(g);
    args.append(b);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

bool Device::setScrollActive(bool active)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.scroll", "setScrollActive");
    QList<QVariant> args;
    args.append(active);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

bool Device::getScrollActive()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.scroll", "getScrollActive");
    return QDBusMessageToBool(m);
}

uchar Device::getScrollEffect()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.scroll", "getScrollEffect");
    return QDBusMessageToByte(m);
}

bool Device::setScrollBlinking(uchar r, uchar g, uchar b)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.scroll", "setScrollBlinking");
    QList<QVariant> args;
    args.append(r);
    args.append(g);
    args.append(b);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

bool Device::setScrollPulsate(uchar r, uchar g, uchar b)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.scroll", "setScrollPulsate");
    QList<QVariant> args;
    args.append(r);
    args.append(g);
    args.append(b);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

bool Device::setScrollSpectrum()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.scroll", "setScrollSpectrum");
    return QDBusMessageToVoid(m);
}

bool Device::setScrollNone()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.scroll", "setScrollNone");
    return QDBusMessageToVoid(m);
}

bool Device::setScrollReactive(uchar r, uchar g, uchar b, uchar speed)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.scroll", "setScrollReactive");
    QList<QVariant> args;
    args.append(r);
    args.append(g);
    args.append(b);
    args.append(speed);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

bool Device::setScrollBreathSingle(uchar r, uchar g, uchar b)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.scroll", "setScrollBreathSingle");
    QList<QVariant> args;
    args.append(r);
    args.append(g);
    args.append(b);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

bool Device::setScrollBreathDual(uchar r, uchar g, uchar b, uchar r2, uchar g2, uchar b2)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.scroll", "setScrollBreathSingle");
    QList<QVariant> args;
    args.append(r);
    args.append(g);
    args.append(b);
    args.append(r2);
    args.append(g2);
    args.append(b2);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

bool Device::setScrollBreathRandom()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.scroll", "setScrollBreathRandom");
    return QDBusMessageToVoid(m);
}


/**
 * Sets the scroll wheel brightness (0-100).
 */
bool Device::setScrollBrightness(double brightness)
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.scroll", "setScrollBrightness");
    QList<QVariant> args;
    args.append(brightness);
    m.setArguments(args);
    return QDBusMessageToVoid(m);
}

/**
 * Gets the current scroll brightness (0-100).
 */
double Device::getScrollBrightness()
{
    QDBusMessage m = prepareDeviceQDBusMessage("razer.device.lighting.scroll", "getScrollBrightness");
    return QDBusMessageToDouble(m);
}

// ----- MISC METHODS FOR LIBRAZER -----

/**
 * Returns a QDBusMessage object for the given device ("org/razer/serial").
 */
QDBusMessage Device::prepareDeviceQDBusMessage(const QString &interface, const QString &method)
{
    return QDBusMessage::createMethodCall("org.razer", "/org/razer/device/" + serial, interface, method);
}

/**
 * Returns a QDBusMessage object for general daemon use ("/org/razer").
 */
QDBusMessage prepareGeneralQDBusMessage(const QString &interface, const QString &method)
{
    return QDBusMessage::createMethodCall("org.razer", "/org/razer", interface, method);
}

/**
 * Sends a QDBusMessage and returns the boolean value.
 */
bool QDBusMessageToBool(const QDBusMessage &message)
{
    QDBusMessage msg = QDBusConnection::sessionBus().call(message);
    if(msg.type() == QDBusMessage::ReplyMessage) {
        // Everything went fine.
        return msg.arguments()[0].toBool();
    }
    // TODO: Handle error
    return false;
}

/**
 * Sends a QDBusMessage and returns the integer value.
 */
int QDBusMessageToInt(const QDBusMessage &message)
{
    QDBusMessage msg = QDBusConnection::sessionBus().call(message);
    if(msg.type() == QDBusMessage::ReplyMessage) {
        // Everything went fine.
        return msg.arguments()[0].toInt();
    }
    // TODO: Handle error
    return false;
}

/**
 * Sends a QDBusMessage and returns the double value.
 */
double QDBusMessageToDouble(const QDBusMessage &message)
{
    QDBusMessage msg = QDBusConnection::sessionBus().call(message);
    if(msg.type() == QDBusMessage::ReplyMessage) {
        // Everything went fine.
        return msg.arguments()[0].toDouble();
    }
    // TODO: Handle error
    return false;
}

/**
 * Sends a QDBusMessage and returns the string value.
 */
QString QDBusMessageToString(const QDBusMessage &message)
{
    QDBusMessage msg = QDBusConnection::sessionBus().call(message);
    if(msg.type() == QDBusMessage::ReplyMessage) {
        // Everything went fine.
        return msg.arguments()[0].toString();
    }
    // TODO: Handle error
    return "error";
}

/**
 * Sends a QDBusMessage and returns the string value.
 */
uchar QDBusMessageToByte(const QDBusMessage &message)
{
    QDBusMessage msg = QDBusConnection::sessionBus().call(message);
    if(msg.type() == QDBusMessage::ReplyMessage) {
        // Everything went fine.
        return msg.arguments()[0].value<uchar>();
    }
    // TODO: Handle error
    return 0x00;
}

/**
 * Sends a QDBusMessage and returns the stringlist value.
 */
QStringList QDBusMessageToStringList(const QDBusMessage &message)
{
    QDBusMessage msg = QDBusConnection::sessionBus().call(message);
    if(msg.type() == QDBusMessage::ReplyMessage) {
        return msg.arguments()[0].toStringList();// VID / PID
    }
    // TODO: Handle errror
    return msg.arguments()[0].toStringList();
}

/**
 * Sends a QDBusMessage and returns the int array value.
 */
//QList<int>
QList<int> QDBusMessageToIntArray(const QDBusMessage &message)
{
    QList<int> *retList = new QList<int>();
    QDBusMessage msg = QDBusConnection::sessionBus().call(message);
    if(msg.type() == QDBusMessage::ReplyMessage) {
//         qDebug() << "reply :" << msg; // sth like QDBusMessage(type=MethodReturn, service=":1.1482", signature="ai", contents=([Argument: ai {5426, 67}]) )
//         qDebug() << "reply arguments : " << msg.arguments();
//         qDebug() << "reply[0] :" << msg.arguments().at(0);
        // ugh
        const QDBusArgument myArg = msg.arguments().at(0).value<QDBusArgument>();
        myArg.beginArray();
        while (!myArg.atEnd()) {
            int myElement = qdbus_cast<int>(myArg);
            retList->append(myElement);
        }
        myArg.endArray();
    }
    // TODO: Handle errror
    //return msg.arguments()[0].toList();
    return *retList;
}

/**
 * Sends a QDBusMessage and returns the xml value.
 */
QDomDocument QDBusMessageToXML(const QDBusMessage &message)
{
    QDBusMessage msg = QDBusConnection::sessionBus().call(message);
    QDomDocument doc;
    if(msg.type() == QDBusMessage::ReplyMessage) {
        doc.setContent(msg.arguments()[0].toString());
    }
    // TODO: Handle errror
    return doc;
}

/**
 * Sends a QDBusMessage and returns if the call was successful.
 */
bool QDBusMessageToVoid(const QDBusMessage &message)
{
    return QDBusConnection::sessionBus().send(message);
    // TODO: Handle errror
}
}

// Main method for testing / playing.
int main()
{
    qDebug() << "Daemon version: " << librazer::getDaemonVersion();
    QStringList serialnrs = librazer::getConnectedDevices();
    librazer::syncEffects(false);
    foreach (const QString &str, serialnrs) {
        qDebug() << "-----------------";
//         qDebug() << "Serial: " << str;
        librazer::Device device = librazer::Device(str);
        qDebug() << "Devicename:" << device.getDeviceName();
//         qDebug() << device.getRazerUrls();
//         if(device.hasCapability("dpi")) {
//             qDebug() << "DEVICE:";
//             qDebug() << device.getDPI();
//             device.setDPI(500, 500);
//             qDebug() << device.getDPI();
//             qDebug() << device.maxDPI();
//         }
//         QList<QColor> list = QList<QColor>() << QColor(255, 0, 0) << QColor(0, 255, 255) << QColor(255, 255, 0);
//         qDebug() << "setKeyRow():" << device.setKeyRow(4, 0, 2, list);

        if(device.hasCapability("poll_rate")) {
            for(int a=0; a<100; a++) {
                usleep(100000);
//                 qDebug() << "Pollrate:" << device.getPollRate();
                qDebug() << "DPI:" << device.getDPI();
            }
//             qDebug() << "Set_pollrate:" << device.setPollRate(librazer::POLL_125HZ);
//             qDebug() << "Pollrate:" << device.getPollRate();
//             qDebug() << "Set_pollrate:" << device.setPollRate(librazer::POLL_1000HZ);
//             qDebug() << "Pollrate:" << device.getPollRate();
        }
        qDebug() << "setCustom():" << device.setCustom();
        if(device.hasMatrix()) {
            QList<int> dimen = device.getMatrixDimensions();
            qDebug() << dimen;
            qDebug() << dimen[0] << "-" << dimen[1];
            QList<QColor> colors = QList<QColor>();
            for(int i=0; i<dimen[1]; i++)
                colors << QColor("yellow");
            qDebug() << "size:" << colors.size();
            for(int i=0; i<dimen[0]; i++) {
//                 for(int j=0; j<dimen[1]-1; j++) {
                qDebug() << i;
//                     qDebug() << j;
                device.setKeyRow(i, 0, dimen[1]-1, colors);
                device.setCustom();
                qDebug() << "Press Enter to continue.";
                std::cin.ignore();
//                 }
            }
        }
//         qDebug() << device.getLogoEffect();
//         device.setLogoStatic(0, 255, 0);
//         device.getVid();
//         device.getPid();
//         qDebug() << "Name:   " << device.getDeviceName();
//         qDebug() << "Type:   " << device.getDeviceType();
//         bool logostatic = device.hasCapabilityInternal("razer.device.lighting.logo", "setLogoStatic");
//         qDebug() << "Can logo static: " << logostatic;
//         QHash<QString, bool> hash = device.getAllCapabilities();
//         for (QHash<QString, bool>::iterator i = hash.begin(); i != hash.end(); ++i)
//             qDebug() << i.key() << ": " << i.value();
    }
}


