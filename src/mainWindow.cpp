#include "mainWindow.h"
#include "ComboBoxHelper.h"
#include "PortComboBox.h"
#include "SerialManager.h"
#include "types.h"
#include "ui_MainWindow.h"
#include <QSerialPortInfo>
#include <algorithm>
#include <memory>

constexpr float OPEN_VOLTAGE_THRS = 2.8F;
constexpr float MAX_A9_READ_VOLTAGE = 20.0;
constexpr float MAX_A9_READ_CURRENT = 2.5;
constexpr float RANGE_TIME_VOLTAGE_GEAR = 20.0;
constexpr float RANGE_TIME_CURVE = 10.0;
constexpr double MS_TO_SECONDS = 1000.0;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(std::make_shared<Ui::MainWindow>())
    , serialWorker(std::make_unique<SerialManager>())
    , chartCurvePage(new QChart())
    , voltageSeries(new QLineSeries())
    , currentSeries(new QLineSeries())
    , axisXCurveTime(new QValueAxis())
    , axisYVoltage(new QValueAxis())
    , axisYCurrent(new QValueAxis())
    , chartVoltageGearPage(new QChart())
    , voltageGearSeries(new QLineSeries())
    , axisXVoltageGearTime(new QValueAxis())
    , axisYVoltageGear(new QValueAxis())
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
    ui->portComboBox->clear();

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
    } else {
        return;
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
    ui->currentGaugeWidget->setRange(0.0, MAX_A9_READ_CURRENT);

    configureCurveGraph();
    configureVoltageGearGraph();
}

void MainWindow::connectButtons() const
{
    connect(ui->connectButton, &QPushButton::clicked, this, &MainWindow::changeText);
    connect(ui->portComboBox, &PortComboBox::popupOpened, this, &MainWindow::createComboPorts);
}

void MainWindow::changeText()
{
    serialWorker->connectDevice();
    connect(serialWorker.get(), &SerialManager::newData, this, &MainWindow::setValuesFromSerial);

    //TODO make it receive from emit status
    ui->connectionText->setText("Conectado");
}

void MainWindow::setValuesFromSerial(const QString &data)
{

    if (data.startsWith("dv")) {
        double time = static_cast<double>(graphTimer.elapsed()) / MS_TO_SECONDS;

        voltagePage.voltage = data.sliced(3);

        qDebug() << "Tensão:" << voltagePage.voltage;

        ui->voltageGearDisplay->setText(voltagePage.voltage);

        voltageGearSeries->append(time, voltagePage.voltage.toDouble());

        axisXVoltageGearTime->setRange(std::max(0.0, time - RANGE_TIME_CURVE), time);

        // set the y range based on max value
        setAxisRange(voltageGearSeries, axisYVoltageGear);

        while (!voltageGearSeries->points().isEmpty() && voltageGearSeries->points().front().x() < time - RANGE_TIME_CURVE) {
            voltageGearSeries->removePoints(0, 1);
        }

        return;
    }
    if (data.startsWith("zd")) { // TODO Add variables to registry 7-12 (maybe a list)
        QString voltage = data.sliced(3).section(" ", DiodePage::VOLTAGE_TEST, DiodePage::VOLTAGE_TEST);
        diodePage.voltageTest = voltage.toFloat() < OPEN_VOLTAGE_THRS ? voltage : "Aberto";
        diodePage.registry_1 = data.section(" ", DiodePage::REGISTRY_1, DiodePage::REGISTRY_1);
        diodePage.registry_2 = data.section(" ", DiodePage::REGISTRY_2, DiodePage::REGISTRY_2);
        diodePage.registry_3 = data.section(" ", DiodePage::REGISTRY_3, DiodePage::REGISTRY_3);
        diodePage.registry_4 = data.section(" ", DiodePage::REGISTRY_4, DiodePage::REGISTRY_4);
        diodePage.registry_5 = data.section(" ", DiodePage::REGISTRY_5, DiodePage::REGISTRY_5);
        diodePage.registry_6 = data.section(" ", DiodePage::REGISTRY_6, DiodePage::REGISTRY_6);

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
        chargingUSBPage.current = data.sliced(3).section(" ", ChargingUSBPage::CURRENT, ChargingUSBPage::CURRENT);
        chargingUSBPage.voltage = data.section(" ", ChargingUSBPage::VOLTAGE, ChargingUSBPage::VOLTAGE);
        chargingUSBPage.power = data.section(" ", ChargingUSBPage::POWER, ChargingUSBPage::POWER);
        chargingUSBPage.mAh = data.section(" ", ChargingUSBPage::MAH, ChargingUSBPage::MAH);
        chargingUSBPage.mWh = data.section(" ", ChargingUSBPage::MWH, ChargingUSBPage::MWH);
        chargingUSBPage.hour = data.section(" ", ChargingUSBPage::HOUR, ChargingUSBPage::HOUR);

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
        curveHistoryPage.current = data.sliced(3).section(" ", CurveHistoryPage::CURRENT, CurveHistoryPage::CURRENT);
        curveHistoryPage.voltage = data.section(" ", CurveHistoryPage::VOLTAGE, CurveHistoryPage::VOLTAGE);

        qDebug() << "Corrente:" << curveHistoryPage.current;
        qDebug() << "Tensão:" << curveHistoryPage.voltage;

        ui->voltageGaugeWidget->setValue(curveHistoryPage.voltage.toDouble());
        ui->currentGaugeWidget->setValue(curveHistoryPage.current.toDouble());

        double time = static_cast<double>(graphTimer.elapsed()) / MS_TO_SECONDS;

        voltageSeries->append(time, curveHistoryPage.voltage.toDouble());
        currentSeries->append(time, curveHistoryPage.current.toDouble());

        setAxisRange(currentSeries, axisYCurrent);
        setAxisRange(voltageSeries, axisYVoltage);
        axisXCurveTime->setRange(std::max(0.0, time - RANGE_TIME_CURVE), time);

        while (!voltageSeries->points().isEmpty() && voltageSeries->points().front().x() < time - RANGE_TIME_CURVE) {
            voltageSeries->removePoints(0, 1);
        }

        while (!currentSeries->points().isEmpty() && currentSeries->points().front().x() < time - RANGE_TIME_CURVE) {
            currentSeries->removePoints(0, 1);
        }

        return;
    }
}

void MainWindow::setAxisRange(const QLineSeries *series, QValueAxis *axis)
{
    if (QList<QPointF> points = series->points(); !points.isEmpty()) {
        auto maxPoint = std::ranges::max_element(points, [](const QPointF &point1, const QPointF &point2) { return point1.y() < point2.y(); });
        qreal maxRealValue = maxPoint->y();
        axis->setRange(0, static_cast<int>(maxRealValue + 1));
    }
}

void MainWindow::configureCurveGraph() const
{
    voltageSeries->setName("Tensão");
    currentSeries->setName("Corrente");

    voltageSeries->setColor(Qt::green);
    currentSeries->setColor(Qt::red);

    chartCurvePage->addSeries(voltageSeries);
    chartCurvePage->addSeries(currentSeries);

    axisXCurveTime->setTitleText("Tempo (s)");
    axisYVoltage->setTitleText("Tensão (V)");
    axisYCurrent->setTitleText("Corrente");

    axisXCurveTime->setRange(0, RANGE_TIME_CURVE);
    axisYVoltage->setRange(0, MAX_A9_READ_VOLTAGE);
    axisYCurrent->setRange(0, MAX_A9_READ_CURRENT);

    chartCurvePage->addAxis(axisXCurveTime, Qt::AlignBottom);
    chartCurvePage->addAxis(axisYVoltage, Qt::AlignLeft);
    chartCurvePage->addAxis(axisYCurrent, Qt::AlignRight);

    voltageSeries->attachAxis(axisXCurveTime);
    voltageSeries->attachAxis(axisYVoltage);

    currentSeries->attachAxis(axisXCurveTime);
    currentSeries->attachAxis(axisYCurrent);

    ui->curveHistoryGraph->setChart(chartCurvePage);
}

void MainWindow::configureVoltageGearGraph() const
{
    voltageGearSeries->setName("Tensão");

    voltageGearSeries->setColor(Qt::green);

    chartVoltageGearPage->addSeries(voltageGearSeries);

    axisXVoltageGearTime->setTitleText("Tempo (s)");
    axisYVoltageGear->setTitleText("Tensão (V)");

    axisXVoltageGearTime->setRange(0, RANGE_TIME_VOLTAGE_GEAR);
    axisYVoltageGear->setRange(0, 1.0);

    chartVoltageGearPage->addAxis(axisXVoltageGearTime, Qt::AlignBottom);
    chartVoltageGearPage->addAxis(axisYVoltageGear, Qt::AlignLeft);

    voltageGearSeries->attachAxis(axisXVoltageGearTime);
    voltageGearSeries->attachAxis(axisYVoltageGear);

    ui->voltageGearGraph->setChart(chartVoltageGearPage);
}
