#pragma once

#include <QObject>
#include <QSerialPort>
#include <QVariant>
#include <qserialport.h>
#include <qstringview.h>
#include <qtmetamacros.h>

enum class BaudRateValues : qint32 { BAUD_9600 = 9600, BAUD_19200 = 19200, BAUD_38400 = 38400, BAUD_115200 = 115200, BAUD_230400 = 230400 };
class SerialManager : public QObject
{
    Q_OBJECT
public:
    explicit SerialManager();
    ~SerialManager() override = default;
    void setBaudRate(BaudRateValues baud);
    [[nodiscard]] qint32 getBaudRate() const;
    void listPorts() const;

private:
    QSerialPort *serial;
    qint32 baudRate = 0;
    void disconnectDevice();
    void connectDevice();
    void readSerialData();
    void sendToSerial(const QByteArray &bytes);
};
