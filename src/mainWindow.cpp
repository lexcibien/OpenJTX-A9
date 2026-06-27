#include "mainWindow.h"
#include "ComboBoxHelper.h"

constexpr float OPEN_VOLTAGE_THRS = 2.8F;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(std::make_shared<Ui::MainWindow>())
    , serialWorker(std::make_unique<SerialManager>())
{
    ui->setupUi(this);

    createComboPorts();
    createComboBaudRate();
    connectButtons();
    configureWidgets();
    addShortcuts(this);
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
    curveHistoryTab.configureGraph(ui.get());
    voltageTab.configureGraph(ui.get());
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

    // TODO make it receive from emit status
    ui->connectionText->setText("Conectado");
}

void MainWindow::setValuesFromSerial(const QString &data)
{

    if (data.startsWith("dv")) {
        voltagePage.voltage = data.sliced(3);

        qDebug() << "Tensão:" << voltagePage.voltage;

        voltageTab.setState(voltagePage);
        voltageTab.update(ui.get());

        return;
    }
    if (data.startsWith("zd")) {
        using enum DiodePage::Position;
        QString voltage = data.sliced(3).section(" ", VOLTAGE_TEST, VOLTAGE_TEST);
        diodePage.voltageTest = voltage.toFloat() < OPEN_VOLTAGE_THRS ? voltage : "Aberto";
        diodePage.registry_6 = data.section(" ", REGISTRY_6, REGISTRY_6);
        diodePage.registry_5 = data.section(" ", REGISTRY_5, REGISTRY_5);
        diodePage.registry_4 = data.section(" ", REGISTRY_4, REGISTRY_4);
        diodePage.registry_3 = data.section(" ", REGISTRY_3, REGISTRY_3);
        diodePage.registry_2 = data.section(" ", REGISTRY_2, REGISTRY_2);
        diodePage.registry_1 = data.section(" ", REGISTRY_1, REGISTRY_1);

        qDebug() << "Tensão:" << diodePage.voltageTest;
        qDebug() << "Registro 1:" << diodePage.registry_1;
        qDebug() << "Registro 2:" << diodePage.registry_2;
        qDebug() << "Registro 3:" << diodePage.registry_3;
        qDebug() << "Registro 4:" << diodePage.registry_4;
        qDebug() << "Registro 5:" << diodePage.registry_5;
        qDebug() << "Registro 6:" << diodePage.registry_6;

        diodeVoltageTab.setState(diodePage);
        diodeVoltageTab.update(ui.get());

        return;
    }
    if (data.startsWith("db")) {
        using enum ChargingUSBPage::Position;
        chargingUSBPage.current = data.sliced(3).section(" ", CURRENT, CURRENT);
        chargingUSBPage.voltage = data.section(" ", VOLTAGE, VOLTAGE);
        chargingUSBPage.power = data.section(" ", POWER, POWER);
        chargingUSBPage.mAh = data.section(" ", MAH, MAH);
        chargingUSBPage.mWh = data.section(" ", MWH, MWH);
        chargingUSBPage.hour = data.section(" ", HOUR, HOUR);

        qDebug() << "Corrente:" << chargingUSBPage.current;
        qDebug() << "Tensão:" << chargingUSBPage.voltage;
        qDebug() << "Potência:" << chargingUSBPage.power;
        qDebug() << "mAh:" << chargingUSBPage.mAh;
        qDebug() << "mWh:" << chargingUSBPage.mWh;
        qDebug() << "Hora:" << chargingUSBPage.hour;

        chargingUSBTab.setState(chargingUSBPage);
        chargingUSBTab.update(ui.get());

        return;
    }
    if (data.startsWith("qc")) {
        using enum CurveHistoryPage::Position;
        curveHistoryPage.current = data.sliced(3).section(" ", CURRENT, CURRENT);
        curveHistoryPage.voltage = data.section(" ", VOLTAGE, VOLTAGE);

        qDebug() << "Corrente:" << curveHistoryPage.current;
        qDebug() << "Tensão:" << curveHistoryPage.voltage;

        curveHistoryTab.setState(curveHistoryPage);
        curveHistoryTab.update(ui.get());

        return;
    }
}

void MainWindow::addShortcuts(MainWindow *window)
{
    auto *closeApp = new QAction(window);
    closeApp->setShortcut(QKeySequence("Ctrl+Q"));

    QApplication::connect(closeApp, &QAction::triggered, window, &QApplication::quit);

    window->addAction(closeApp);
}
