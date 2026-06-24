#pragma once

#include <QComboBox>
#include <qcombobox.h>

class PortComboBox : public QComboBox
{
    Q_OBJECT

public:
    using QComboBox::QComboBox;

signals:
    void popupOpened();

protected:
    void showPopup() override
    {
        emit popupOpened();
        QComboBox::showPopup();
    }
};
