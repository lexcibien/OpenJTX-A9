#pragma once

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QStringView>
#include <QTimer>
#include <QVariant>
#include <memory>

enum class BaudRateValues : qint32 { BAUD_9600 = 9600, BAUD_19200 = 19200, BAUD_38400 = 38400, BAUD_115200 = 115200, BAUD_230400 = 230400 };

class SerialManager : public QObject
{
    Q_OBJECT
public:
    explicit SerialManager();
    ~SerialManager() override = default;
    void setBaudRate(BaudRateValues baud);
    [[nodiscard]] qint32 getBaudRate() const;
    static void listPorts();
    static QVector<QSerialPortInfo> getPortsList();
    [[nodiscard]] QString getSerialName() const;
    void setPort(const QSerialPortInfo &outPort);
    void connectDevice();
    void disconnectDevice();
    void readSerialData();
    [[nodiscard]] QString getSerialData() const;
    void sendToSerial(const QByteArray &bytes);

private:
    std::unique_ptr<QSerialPort> serial;
    qint32 baudRate = 0;
    QSerialPortInfo port;

    QString receivedData;

    std::unique_ptr<QTimer> timer;
};
