#ifndef HOVER_CLOCK_H
#define HOVER_CLOCK_H

#include <QMainWindow>
#include <QTime>
#include <QMenu>
#include <QTimer>
#include <QScreen>
#include <QPainter>
#include <QPalette>
#include <QSettings>
#include <QPainterPath>
#include <QSystemTrayIcon>

#include "clock_position.h"
#include "settings_dialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class HoverClock; }
QT_END_NAMESPACE

#define PAINT_OFFSET 5

class HoverClock : public QMainWindow
{
    Q_OBJECT

    public:
        HoverClock(QWidget *parent = nullptr);
        ~HoverClock();

    protected:
        void timerEvent(QTimerEvent *event);
        void paintEvent(QPaintEvent *event);

    private:
        Ui::HoverClock *ui;
        QHash<QString, QVariant> settings;

        void resizeWindow();
        void showOptions();
        void createSystemTray();
        void initializeSettings();
        void updateClockPosition();
        float getTextWidth(QString text, QFont font);

    private slots:
        void toggleVisibility();
};

#endif // HOVER_CLOCK_H
