#include "SerialManager.h"
#include <QDebug>
#include <memory>

SerialManager::SerialManager()
    : serial(nullptr)
{
}

void SerialManager::connectDevice() //TODO Add ability to connect via VID and PID
{
    if (serial != nullptr) {
        disconnectDevice();
    }

    /* open new device */
    qDebug() << "Conexão serial com um dispositivo JTXTools A9";
    serial = std::make_unique<QSerialPort>(QSerialPortInfo(port.portName()));
    if (serial == nullptr) {
        qDebug() << "Não foi possível abrir a porta serial";
        return;
    }
    qDebug() << "Criado um objeto porta serial";

    /* configure */
    serial->setBaudRate(baudRate);
    serial->setDataBits(QSerialPort::Data8);

    serial->setFlowControl(QSerialPort::NoFlowControl);
    if (!serial->open(QIODevice::ReadWrite)) {
        qDebug() << ("Erro ao abrir a porta: " + serial->errorString());
    }

    // emit status(SERIAL::CONNECTED);
    connect(serial.get(), &QSerialPort::readyRead, this, &SerialManager::readSerialData);
}

void SerialManager::disconnectDevice()
{
    if (serial != nullptr) {
        if (serial->isOpen()) {
            serial->close();
        }

        // emit status(SERIAL::DISCONNECT);
        // recreate PortComboBox
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
        qDebug() << "Tentativa de escrever para a porta serial quando não foi inicializada!";
        return;
    }

    if ((serial != nullptr) && !serial->isOpen()) {
        qDebug() << "Tentativa de escrever na porta serial quando não foi aberta!";
        return;
    }

    // emit status(SERIAL::SENDING);
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
    // emit status(SERIAL::READING);
    serialBuffer += serial->readAll();

    while (serialBuffer.contains('\n'))
    {
        qsizetype pos = serialBuffer.indexOf('\n');

        QByteArray line = serialBuffer.left(pos);

        serialBuffer.remove(0, pos + 1);

        receivedData = QString::fromUtf8(line).trimmed();

        emit newData(receivedData);
    }
}

QString SerialManager::getSerialData() const { return receivedData; }
