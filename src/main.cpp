#include "hoverclock.h"

#include <QApplication>
#include "run_guard.h"

int main(int argc, char *argv[])
{
    RunGuard guard("hoverclock");

    if (!guard.tryToRun())
        return 0;

    QApplication a(argc, argv);

    a.setWindowIcon(QIcon(":/resources/icons/icon.png"));

    a.setApplicationDisplayName("Hoverclock");
    a.setOrganizationDomain("https://github.com/kostoskistefan/hoverclock");
    a.setOrganizationName("hoverclock");
    a.setApplicationName("hoverclock");
    a.setApplicationVersion(APP_VERSION);

    Hoverclock w;
    w.show();

    return a.exec();
}
