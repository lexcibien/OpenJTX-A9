#pragma once

#include "SerialManager.h"
#include "types.h"
#include <QElapsedTimer>
#include <QLineSeries>
#include <QMainWindow>
#include <QValueAxis>
#include <memory>

namespace Ui {
class MainWindow;
} // namespace Ui

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    std::shared_ptr<Ui::MainWindow> ui;
    std::unique_ptr<SerialManager> serialWorker;

    QChart *chart;
    QLineSeries *voltageSeries;
    QLineSeries *currentSeries;

    QValueAxis* axisX;
    QValueAxis* axisYVoltage;
    QValueAxis* axisYCurrent;

    QList<QSerialPortInfo> availablePorts;

    VoltagePage voltagePage;
    ChargingUSBPage chargingUSBPage;
    DiodePage diodePage;
    CurveHistoryPage curveHistoryPage;

    QElapsedTimer graphTimer;

    void createComboBaudRate();
    void createComboPorts();

    void comboBaudRate(BaudRateValues which);
    void comboPorts(int index);

    void configureWidgets() const;
    void connectButtons() const;
    void changeText();
    void setValuesFromSerial(const QString &data);

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;
};
