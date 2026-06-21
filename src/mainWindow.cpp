#include "mainWindow.h"
#include "ComboBoxHelper.h"
#include "ui_MainWindow.h"
#include <QSerialPortInfo>
#include <memory>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(std::make_shared<Ui::MainWindow>())
    , serialWorker(std::make_unique<SerialManager>())
    , timer(nullptr)
{
    ui->setupUi(this);

    createComboPorts();
    createComboBaudRate();
    connectButtons();
    SerialManager::listPorts();
}

void MainWindow::createComboPorts()
{
    QVector<ComboBoxHelper::Item<int>> portsList;
    availablePorts = SerialManager::getPortsList();

    int index = 0;
    for (const QSerialPortInfo &port : availablePorts) {
        portsList.append({ port.portName(), index });
        index++;
    }

    int defaultPort = 0;

    if (!portsList.isEmpty()) {
        defaultPort = portsList.first().value();
    }

    ComboBoxHelper::setup<int>(ui->portComboBox, defaultPort, portsList, this, &MainWindow::comboPorts);
}

void MainWindow::createComboBaudRate()
{
    using enum BaudRateValues;
    ComboBoxHelper::setup<BaudRateValues>(ui->baudComboBox, BAUD_115200,
        { { "9600", BAUD_9600 }, { "19200", BAUD_19200 }, { "38400", BAUD_38400 }, { "115200", BAUD_115200 }, { "230400", BAUD_230400 } }, this,
        &MainWindow::comboBaudRate);
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void MainWindow::comboBaudRate(BaudRateValues which)
{
    switch (which) {
        using enum BaudRateValues;
    case BAUD_9600:
        serialWorker->setBaudRate(BAUD_9600);
        break;
    case BAUD_19200:
        serialWorker->setBaudRate(BAUD_19200);
        break;
    case BAUD_38400:
        serialWorker->setBaudRate(BAUD_38400);
        break;
    case BAUD_115200:
        serialWorker->setBaudRate(BAUD_115200);
        break;
    case BAUD_230400:
        serialWorker->setBaudRate(BAUD_230400);
        break;
    }
}

void MainWindow::comboPorts(int index)
{
    serialWorker->setPort(availablePorts.at(index));
    qDebug() << "Porta selecionada:" << serialWorker->getSerialName();
}

void MainWindow::connectButtons() const { connect(ui->connectButton, &QAbstractButton::clicked, this, &MainWindow::changeText); }

void MainWindow::changeText()
{
    timer = new QTimer(this);
    serialWorker->connectDevice();
    connect(timer, &QTimer::timeout, this, &MainWindow::setValuesFromSerial);
    timer->start(50);
    ui->connectionText->setText("Conectado");
}

void MainWindow::setValuesFromSerial()
{
    QString serialData = serialWorker->getSerialData();
    if (serialData.startsWith("dv")) {
        voltagePage.voltage = serialData.sliced(3);
    }
    if (serialData.startsWith("db")) {
        chargingUSBPage.current = serialData.sliced(3).section(" ", 0, 0);
        chargingUSBPage.voltage = serialData.section(" ", 1, 1);
        chargingUSBPage.power = serialData.section(" ", 2, 2);
        chargingUSBPage.mAh = serialData.section(" ", 3, 3);
        chargingUSBPage.mWh = serialData.section(" ", 4, 4);
        chargingUSBPage.hour = serialData.section(" ", 5, 5);
    }

    qDebug() << "corrente" << chargingUSBPage.current;
    qDebug() << "tensão" << chargingUSBPage.voltage;
    qDebug() << "potência" << chargingUSBPage.power;
    qDebug() << "mAh" << chargingUSBPage.mAh;
    qDebug() << "mWh" << chargingUSBPage.mWh;
    qDebug() << "hora" << chargingUSBPage.hour;

    ui->voltageDisplay->setText(voltagePage.voltage);
}
