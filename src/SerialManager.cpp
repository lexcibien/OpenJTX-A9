#include "SerialManager.h"
#include <QDebug>
#include <QSerialPortInfo>
#include <QSettings>
#include <QStringBuilder>
#include <memory>

SerialManager::SerialManager()
    : serial(nullptr)
    , timer(nullptr)
{
}

void SerialManager::connectDevice()
{
    timer = new QTimer(this);
    /* disconnect device */
    if (serial != nullptr) {
        disconnectDevice();
    }

    /* open new device */
    qDebug() << "Serial connection to a GVRET device";
    serial = std::make_unique<QSerialPort>(QSerialPortInfo(port.portName()));
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

    connect(timer, &QTimer::timeout, this, &SerialManager::readSerialData);
    timer->start(17);
}

void SerialManager::disconnectDevice()
{
    if (serial != nullptr) {
        if (serial->isOpen()) {
            serial->close();
        }
        serial->disconnect(); // disconnect all signals
        serial.reset();
        serial = nullptr;
    }
}

void SerialManager::listPorts()
{
    for (const QSerialPortInfo &portInfo : QSerialPortInfo::availablePorts()) {
        if (portInfo.systemLocation().startsWith("/dev/ttyS")) {
            continue;
        }
        if (portInfo.systemLocation().startsWith("/dev/cu.")) {
            continue;
        }
        qDebug() << "Porta USB Encontrada:" << portInfo.systemLocation();
        qDebug() << "Descrição:" << portInfo.description();
        qDebug() << "Fabricante:" << portInfo.manufacturer();
        qDebug() << "-----------------------------------------";
    }
}

QString SerialManager::getSerialName() const { return port.portName(); }

void SerialManager::setPort(const QSerialPortInfo &outPort) { port = outPort; }

void SerialManager::setBaudRate(BaudRateValues baud)
{
    baudRate = std::to_underlying(baud);
    qDebug() << "A baud rate é de:" << baudRate;
}

qint32 SerialManager::getBaudRate() const { return baudRate; }

QVector<QSerialPortInfo> SerialManager::getPortsList()
{
    QVector<QSerialPortInfo> portsList;

    for (const QSerialPortInfo &port : QSerialPortInfo::availablePorts()) {
        if (port.systemLocation().startsWith("/dev/ttyS")) {
            continue;
        }
        if (port.systemLocation().startsWith("/dev/cu.")) {
            continue;
        }
        qDebug() << "Porta USB Encontrada:" << port.systemLocation();
        qDebug() << "Descrição:" << port.description();
        qDebug() << "Fabricante:" << port.manufacturer();
        qDebug() << "-----------------------------------------";

        portsList.append(port);
    }
    return portsList;
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
    QString debugBuild;

    if (serial != nullptr) {
        data = serial->readLine();
    }

    receivedData = QString::fromUtf8(data).trimmed();
}

QString SerialManager::getSerialData() const { return receivedData; }
