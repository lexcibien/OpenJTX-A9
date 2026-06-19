#include "mainWindow.h"
#include "ComboBoxHelper.h"
#include "ui_MainWindow.h"
#include <QSerialPortInfo>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , serialWorker(new SerialManager)
{
    ui->setupUi(this);

    createComboPorts();
    createComboBaudRate();
    connectButtons();
    serialWorker->listPorts();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::createComboPorts()
{
    QVector<ComboBoxHelper::Item<QString>> portsList;

    for (const QSerialPortInfo &port : QSerialPortInfo::availablePorts()) {
        if (port.systemLocation().startsWith("/dev/ttyS")) {
            continue;
        }
        qDebug() << port.portName();
        portsList.append({ port.portName(), port.systemLocation() });
    }

    QString defaultPort;

    if (!portsList.isEmpty()) {
        defaultPort = portsList.first().value;
    }

    ComboBoxHelper::setup<QString>(ui->portComboBox, defaultPort, portsList, this, &MainWindow::comboPorts);
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

void MainWindow::comboPorts(const QString &portName) { qDebug() << "Porta selecionada:" << portName; }

void MainWindow::connectButtons() const { connect(ui->changeModeButton, &QAbstractButton::clicked, this, &MainWindow::changeText); }

void MainWindow::changeText() { ui->continuityText->setText("Fechado"); }
