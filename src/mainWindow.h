#pragma once

#include "SerialManager.h"
#include <QMainWindow>

namespace Ui {
class MainWindow;
} // namespace Ui

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow *ui;

    SerialManager *serialWorker;
    void createComboBaudRate();

    void connectButtons() const;
    void changeText();

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    void comboBaudRate(BaudRateValues which);
};
