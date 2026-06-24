#pragma once

#include "SerialManager.h"
#include "types.h"
#include <ui/ChargingUSBTab.h>
#include <ui/CurveHistoryTab.h>
#include <ui/DiodeVoltageTab.h>
#include <ui/VoltageTab.h>

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

    VoltageTab voltageTab;
    ChargingUSBTab chargingUSBTab;
    CurveHistoryTab curveHistoryTab;
    DiodeVoltageTab diodeVoltageTab;

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
