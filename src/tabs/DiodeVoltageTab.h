#pragma once

#include "types.h"
#include "ui_MainWindow.h"

class DiodeVoltageTab
{
private:
    DiodePage diodePage;

    QList<QString> registry = { "0.0000", "0.0000", "0.0000", "0.0000", "0.0000", "0.0000" };

public:
    DiodeVoltageTab() = default;

    void setState(const DiodePage &value) { diodePage = value; }

    void update(Ui::MainWindow *uic)
    {
        //? This might ignore equal values
        if (diodePage.registry_6 != registry.at(0)) {
            registry.push_front(diodePage.registry_6); // bug ta copiando algo aqui para o o 6 (fora de ordem)
        }

        uic->continuityText->setText(diodePage.voltageTest);
        uic->reg1Display->setText(diodePage.registry_1);
        uic->reg2Display->setText(diodePage.registry_2);
        uic->reg3Display->setText(diodePage.registry_3);
        uic->reg4Display->setText(diodePage.registry_4);
        uic->reg5Display->setText(diodePage.registry_5);
        uic->reg6Display->setText(diodePage.registry_6);
        uic->reg7Display->setText(registry.at(0));
        uic->reg8Display->setText(registry.at(1));
        uic->reg9Display->setText(registry.at(2));
        uic->reg10Display->setText(registry.at(3));
        uic->reg11Display->setText(registry.at(4));
        uic->reg12Display->setText(registry.at(5));
    }
};
