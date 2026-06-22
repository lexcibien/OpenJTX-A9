#include "mainWindow.h"
#include "ComboBoxHelper.h"
#include "ui_MainWindow.h"
#include <QSerialPortInfo>
#include <memory>

const float OPEN_VOLTAGE_THRS = 2.8F;
const float MAX_A9_READ_VOLTAGE = 20.0;
const float MAX_A9_READ_CURRENT = 2.5;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(std::make_shared<Ui::MainWindow>())
    , serialWorker(std::make_unique<SerialManager>())
    , chart(new QChart())
    , voltageSeries(new QLineSeries())
    , currentSeries(new QLineSeries())
    , axisX(new QValueAxis())
    , axisYVoltage(new QValueAxis())
    , axisYCurrent(new QValueAxis())
{
    ui->setupUi(this);

    graphTimer.start();
    createComboPorts();
    createComboBaudRate();
    connectButtons();
    configureWidgets();
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

    ComboBoxHelper::Item<int>::setup(ui->portComboBox, defaultPort, portsList, this, &MainWindow::comboPorts);
}

void MainWindow::createComboBaudRate()
{
    using enum BaudRateValues;
    ComboBoxHelper::Item<BaudRateValues>::setup(ui->baudComboBox, BAUD_115200,
        { { "9600", BAUD_9600 }, { "19200", BAUD_19200 }, { "38400", BAUD_38400 }, { "115200", BAUD_115200 }, { "230400", BAUD_230400 } }, this,
        &MainWindow::comboBaudRate);
}

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

void MainWindow::configureWidgets() const
{
    ui->voltageGaugeWidget->setRange(0.0, MAX_A9_READ_VOLTAGE);
    ui->voltageGaugeWidget->setMaximumSize(500, 200);
    ui->currentGaugeWidget->setRange(0.0, MAX_A9_READ_CURRENT);
    ui->currentGaugeWidget->setMaximumSize(500, 200);

    voltageSeries->setName("Tensão");
    currentSeries->setName("Corrente");

    voltageSeries->setColor(Qt::green);
    currentSeries->setColor(Qt::red);

    chart->addSeries(voltageSeries);
    chart->addSeries(currentSeries);

    axisX->setTitleText("Tempo (s)");
    axisYVoltage->setTitleText("Tensão");
    axisYCurrent->setTitleText("Corrente");

    axisX->setRange(0, 10);
    axisYVoltage->setRange(0, MAX_A9_READ_VOLTAGE);
    axisYCurrent->setRange(0, MAX_A9_READ_CURRENT);

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisYVoltage, Qt::AlignLeft);
    chart->addAxis(axisYCurrent, Qt::AlignRight);

    voltageSeries->attachAxis(axisX);
    voltageSeries->attachAxis(axisYVoltage);

    currentSeries->attachAxis(axisX);
    currentSeries->attachAxis(axisYCurrent);

    ui->curveHistoryGraph->setChart(chart);
}

void MainWindow::connectButtons() const { connect(ui->connectButton, &QAbstractButton::clicked, this, &MainWindow::changeText); }

void MainWindow::changeText()
{
    serialWorker->connectDevice();
    connect(serialWorker.get(), &SerialManager::newData,this, &MainWindow::setValuesFromSerial);

    ui->connectionText->setText("Conectado");
}

void MainWindow::setValuesFromSerial(const QString &data)
{

    if (data.startsWith("dv")) {
        double time = static_cast<double>(graphTimer.elapsed()) / 1000.0;

        voltagePage.voltage = data.sliced(3);

        qDebug() << "Tensão:" << voltagePage.voltage;

        ui->continuityText->setText(voltagePage.voltage);

        return;
    }
    if (data.startsWith("zd")) { // TODO Add variables to registry 7-12
        QString voltage = data.sliced(3).section(" ", 0, 0);
        diodePage.voltageTest = voltage.toFloat() < OPEN_VOLTAGE_THRS ? voltage : "Aberto";
        diodePage.registry_1 = data.section(" ", 1, 1);
        diodePage.registry_2 = data.section(" ", 2, 2);
        diodePage.registry_3 = data.section(" ", 3, 3);
        diodePage.registry_4 = data.section(" ", 4, 4);
        diodePage.registry_5 = data.section(" ", 5, 5);
        diodePage.registry_6 = data.section(" ", 6, 6);

        qDebug() << "Tensão:" << diodePage.voltageTest;
        qDebug() << "Registro 1:" << diodePage.registry_1;
        qDebug() << "Registro 2:" << diodePage.registry_2;
        qDebug() << "Registro 3:" << diodePage.registry_3;
        qDebug() << "Registro 4:" << diodePage.registry_4;
        qDebug() << "Registro 5:" << diodePage.registry_5;
        qDebug() << "Registro 6:" << diodePage.registry_6;

        ui->continuityText->setText(diodePage.voltageTest);
        ui->reg1Display->setText(diodePage.registry_1);
        ui->reg2Display->setText(diodePage.registry_2);
        ui->reg3Display->setText(diodePage.registry_3);
        ui->reg4Display->setText(diodePage.registry_4);
        ui->reg5Display->setText(diodePage.registry_5);
        ui->reg6Display->setText(diodePage.registry_6);
        ui->reg7Display->setText(diodePage.registry_1);
        ui->reg8Display->setText(diodePage.registry_2);
        ui->reg9Display->setText(diodePage.registry_3);
        ui->reg10Display->setText(diodePage.registry_4);
        ui->reg11Display->setText(diodePage.registry_5);
        ui->reg12Display->setText(diodePage.registry_6);

        return;
    }
    if (data.startsWith("db")) {
        chargingUSBPage.current = data.sliced(3).section(" ", 0, 0);
        chargingUSBPage.voltage = data.section(" ", 1, 1);
        chargingUSBPage.power = data.section(" ", 2, 2);
        chargingUSBPage.mAh = data.section(" ", 3, 3);
        chargingUSBPage.mWh = data.section(" ", 4, 4);
        chargingUSBPage.hour = data.section(" ", 5, 5);

        qDebug() << "Corrente:" << chargingUSBPage.current;
        qDebug() << "Tensão:" << chargingUSBPage.voltage;
        qDebug() << "Potência:" << chargingUSBPage.power;
        qDebug() << "mAh:" << chargingUSBPage.mAh;
        qDebug() << "mWh:" << chargingUSBPage.mWh;
        qDebug() << "Hora:" << chargingUSBPage.hour;

        ui->voltageDisplay->setText(chargingUSBPage.voltage);
        ui->currentDisplay->setText(chargingUSBPage.current);
        ui->powerDisplay->setText(chargingUSBPage.power);
        ui->mAhDisplay->setText(chargingUSBPage.mAh);
        ui->mWhDisplay->setText(chargingUSBPage.mWh);
        ui->timeDisplay->setText(chargingUSBPage.hour);

        return;
    }
    if (data.startsWith("qc")) {
        double time = static_cast<double>(graphTimer.elapsed()) / 1000.0;
        curveHistoryPage.current = data.sliced(3).section(" ", 0, 0);
        curveHistoryPage.voltage = data.section(" ", 1, 1);

        qDebug() << "Corrente:" << curveHistoryPage.current;
        qDebug() << "Tensão:" << curveHistoryPage.voltage;

        ui->voltageGaugeWidget->setValue(curveHistoryPage.voltage.toDouble());
        ui->currentGaugeWidget->setValue(curveHistoryPage.current.toDouble());

        voltageSeries->append(time, curveHistoryPage.voltage.toDouble());
        currentSeries->append(time, curveHistoryPage.current.toDouble());

        axisX->setRange(std::max(0.0, time - 10.0), time);

        while (!voltageSeries->points().isEmpty() && voltageSeries->points().front().x() < time - 10.0) {
            voltageSeries->removePoints(0, 1);
        }

        while (!currentSeries->points().isEmpty() && currentSeries->points().front().x() < time - 10.0) {
            currentSeries->removePoints(0, 1);
        }

        return;
    }
}
