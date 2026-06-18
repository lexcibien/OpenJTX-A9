#include "mainWindow.h"
#include "ComboBoxHelper.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->continuityText->setText("Fechado");
    // const QStringList baudList = { "9600", "19200", "38400", "115200", "230400" };

    // ui->baudComboBox->addItems(baudList);
    ComboBoxHelper::setup<GoodThings>(ui->baudComboBox, GoodThings::KDABVideos,
        { { "Angels", GoodThings::Angels }, { "Depeche Mode", GoodThings::DepecheMode }, { "KDAB Videos on YouTube", GoodThings::KDABVideos } }, this, &MainWindow::doGoodThings);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::doGoodThings(MainWindow::GoodThings which) const
{
    switch (which) {
        using enum GoodThings;
    case Angels:
        qDebug() << "Angels are good assuming they exists.";
        break;
    case DepecheMode:
        qDebug() << "Enjoy the Silence...";
        break;
    case KDABVideos:
        qDebug() << "http://kdab.tv rocks!";
    }
}
