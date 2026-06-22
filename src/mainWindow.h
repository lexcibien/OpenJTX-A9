#pragma once

#include "SerialManager.h"
#include "types.h"
#include <QMainWindow>
#include <memory>
#include <QElapsedTimer>

namespace Ui {
class MainWindow;
} // namespace Ui

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    std::shared_ptr<Ui::MainWindow> ui;
    std::unique_ptr<SerialManager> serialWorker;

    QList<QSerialPortInfo> availablePorts;

    VoltagePage voltagePage;
    ChargingUSBPage chargingUSBPage;
    DiodePage diodePage;
    CurveHistoryPage curveHistoryPage;

    QTimer *timer;

    int timeSeconds = 0;
    QElapsedTimer graphTimer;
    QVector<double> timeData;
    QVector<double> voltageData;

    void createComboBaudRate();
    void createComboPorts();

    void comboBaudRate(BaudRateValues which);
    void comboPorts(int index);

    void configureWidgets() const;
    void connectButtons() const;
    void changeText();
    void setValuesFromSerial();

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;
};
