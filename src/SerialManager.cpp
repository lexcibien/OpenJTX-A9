#include "SerialManager.h"
#include <QDebug>
#include <QSerialPortInfo>
#include <QSettings>
#include <QStringBuilder>
#include <qdebug.h>
#include <utility>

SerialManager::SerialManager()
    : serial(nullptr)
{
}

void SerialManager::connectDevice()
{
    /* disconnect device */
    if (serial != nullptr) {
        disconnectDevice();
    }

    /* open new device */
    qDebug() << "Serial connection to a GVRET device";
    serial = new QSerialPort(QSerialPortInfo("Device"));
    if (serial == nullptr) {
        qDebug() << "can't open serial port ";
        return;
    }
    qDebug() << "Created Serial Port Object";

    /* configure */
    serial->setBaudRate(baudRate); // most GVRET devices ignore baud, ESP32 needs it set explicitly to the proper value
    serial->setDataBits(QSerialPort::Data8);

    qDebug() << "Trying Standard Serial Mode";
    serial->setFlowControl(QSerialPort::HardwareControl); // Most GVRET style devices use hardware flow control
    if (!serial->open(QIODevice::ReadWrite)) {
        qDebug() << ("Error returned during port opening: " + serial->errorString());
    }
}

void SerialManager::disconnectDevice()
{
    if (serial != nullptr) {
        if (serial->isOpen()) {
            serial->close();
        }
        serial->disconnect(); // disconnect all signals
        delete serial;
        serial = nullptr;
    }
}

void SerialManager::listPorts() const
{
#pragma unroll 2
    for (const QSerialPortInfo &portInfor : QSerialPortInfo::availablePorts()) {
        qDebug() << "Port Name: " << portInfor.portName();
        qDebug() << "Port Location: " << portInfor.systemLocation();
    }
}

void SerialManager::sendToSerial(const QByteArray &bytes)
{
    if (serial == nullptr) {
        qDebug() << "Attempt to write to serial port when it has not been initialized!";
        return;
    }

    if ((serial != nullptr) && !serial->isOpen()) {
        qDebug() << "Attempt to write to serial port when it is not open!";
        return;
    }

    QString buildDebug;
    buildDebug = "Write to serial -> ";
    foreach (int byt, bytes) {
        byt = static_cast<unsigned char>(byt);
        buildDebug = buildDebug % QString::number(byt, 16) % " ";
    }
    qDebug() << buildDebug;

    if (serial != nullptr) {
        serial->write(bytes);
    }
}

void SerialManager::readSerialData()
{
    QByteArray data;
    unsigned char character = 0;
    QString debugBuild;

    if (serial != nullptr) {
        data = serial->readAll();
    }

    qDebug() << ("Got data from serial. Len = " % QString::number(data.length()));
    for (int i = 0; i < data.length(); i++) {
        character = data.at(i);
        debugBuild = debugBuild % QString::number(character, 16).rightJustified(2, '0') % " ";
    }
    qDebug() << debugBuild;
}

void SerialManager::setBaudRate(BaudRateValues baud)
{
    baudRate = std::to_underlying(baud);
    qDebug() << "A baud rate é de:" << baudRate;
}

qint32 SerialManager::getBaudRate() const { return baudRate; }
