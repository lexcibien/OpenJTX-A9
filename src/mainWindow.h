#pragma once

#include "SerialManager.h"
#include "types.h"
#include "ui_MainWindow.h"
#include <tabs/ChargingUSBTab.h>
#include <tabs/CurveHistoryTab.h>
#include <tabs/DiodeVoltageTab.h>
#include <tabs/VoltageTab.h>

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

    static void addShortcuts(MainWindow *window);

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;
};
