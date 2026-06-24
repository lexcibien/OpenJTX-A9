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

    QChart *chartCurvePage;
    QLineSeries *voltageSeries;
    QLineSeries *currentSeries;

    QValueAxis* axisXCurveTime;
    QValueAxis* axisYVoltage;
    QValueAxis* axisYCurrent;

    QChart *chartVoltageGearPage;
    QLineSeries *voltageGearSeries;

    QValueAxis* axisXVoltageGearTime;
    QValueAxis* axisYVoltageGear;

    QList<QSerialPortInfo> availablePorts;

    QList<QString> registry = { "0.0000", "0.0000", "0.0000", "0.0000", "0.0000", "0.0000" };

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
    void configureCurveGraph() const;
    void configureVoltageGearGraph() const;
    static void setAxisRange(const QLineSeries* series, QValueAxis* axis);
    void connectButtons() const;
    void changeText();
    void setValuesFromSerial(const QString &data);

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;
};
