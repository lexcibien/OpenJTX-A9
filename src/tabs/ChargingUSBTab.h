#pragma once

#include "types.h"
#include "ui_MainWindow.h"

class ChargingUSBTab
{
private:
    ChargingUSBPage chargingUSBPage;

public:
    ChargingUSBTab() = default;

    void setState(const ChargingUSBPage &value) { chargingUSBPage = value; }

    void update(Ui::MainWindow *uic) const
    {
        uic->voltageDisplay->setText(chargingUSBPage.voltage);
        uic->currentDisplay->setText(chargingUSBPage.current);
        uic->powerDisplay->setText(chargingUSBPage.power);
        uic->mAhDisplay->setText(chargingUSBPage.mAh);
        uic->mWhDisplay->setText(chargingUSBPage.mWh);
        uic->timeDisplay->setText(chargingUSBPage.hour);
    }
};
