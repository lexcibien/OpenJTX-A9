#pragma once

#include "SerialManager.h"
#include <QMainWindow>
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

    void createComboBaudRate();
    void createComboPorts();

    void comboBaudRate(BaudRateValues which);
    void comboPorts(const QString &portName);

    void connectButtons() const;
    void changeText();

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;
};
