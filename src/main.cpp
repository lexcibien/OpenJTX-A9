#include "mainWindow.h"
#include <QSettings>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QApplication::setApplicationName("OpenJTX_A9");
    QApplication::setOrganizationName("lexcibien");
    QApplication::setOrganizationDomain("lexcibien.me");

    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings settings;

    MainWindow window;

    window.show();

    QFont sysFont;
    QApplication::setFont(sysFont);

    int retCode = QApplication::exec();

    return retCode;
}
