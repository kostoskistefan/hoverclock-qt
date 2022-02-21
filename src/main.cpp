#include "hoverclock.h"

#include <QApplication>
#include <run_guard.h>
#include <QFile>

int main(int argc, char *argv[])
{
    RunGuard guard("hoverclock");

    if (!guard.tryToRun())
        return 0;

    QApplication application(argc, argv);

    application.setWindowIcon(QIcon(":/resources/icons/hoverclock.svg"));

    application.setApplicationDisplayName("Hoverclock");
    application.setOrganizationDomain("https://github.com/kostoskistefan/hoverclock");
    application.setOrganizationName("hoverclock");
    application.setApplicationName("hoverclock");
    application.setApplicationVersion(APP_VERSION);

    QFile themeFile(":/resources/styles/dark-theme.qss");
    themeFile.open(QFile::ReadOnly);
    QString theme = QLatin1String(themeFile.readAll());
    application.setStyleSheet(theme);

    Hoverclock w;
    w.show();

    return application.exec();
}
