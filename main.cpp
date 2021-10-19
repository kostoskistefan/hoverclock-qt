#include "hoverclock.h"

#include <QApplication>
#include <RunGuard.h>

int main(int argc, char *argv[])
{
    RunGuard guard("hoverclock");

    if (!guard.tryToRun())
        return 0;

    QApplication a(argc, argv);

    HoverClock w;
    w.show();

    return a.exec();
}
