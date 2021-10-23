#include "hover_clock.h"

#include <QApplication>
#include "run_guard.h"

int main(int argc, char *argv[])
{
    RunGuard guard("hoverclock");

    if (!guard.tryToRun())
        return 0;

    QApplication a(argc, argv);

    a.setApplicationDisplayName("HoverClock");
    a.setOrganizationDomain("https://github.com/kostoskistefan/hoverclock");
    a.setOrganizationName("hoverclock");
    a.setApplicationName("hoverclock");
    a.setApplicationVersion(APP_VERSION);

    HoverClock w;
    w.show();

    return a.exec();
}
