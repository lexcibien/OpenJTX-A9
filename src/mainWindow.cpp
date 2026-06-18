#include "mainWindow.h"
#include "ComboBoxHelper.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    createComboBaudRate();
    connectButtons();
}

MainWindow::~MainWindow() { delete ui; }

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
        baudRate = std::to_underlying(BAUD_9600);
        break;
    case BAUD_19200:
        baudRate = std::to_underlying(BAUD_19200);
        break;
    case BAUD_38400:
        baudRate = std::to_underlying(BAUD_38400);
        break;
    case BAUD_115200:
        baudRate = std::to_underlying(BAUD_115200);
        break;
    case BAUD_230400:
        baudRate = std::to_underlying(BAUD_230400);
        break;
    }
    qDebug() << "A baud rate é de:" << baudRate;
}

void MainWindow::connectButtons() const { connect(ui->changeModeButton, &QAbstractButton::clicked, this, &MainWindow::changeText); }

void MainWindow::changeText() { ui->continuityText->setText("Fechado"); }
