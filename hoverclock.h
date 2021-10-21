#ifndef HOVERCLOCK_H
#define HOVERCLOCK_H

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

#include "clockPosition.h"
#include "settingsdialog.h"

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
//        QSettings settings;
        QHash<QString, QVariant> settingsHash;
        Ui::HoverClock *ui;

        void showOptions();
        void createSystemTray();
        void initializeSettings();
        void updateClockPosition();
        float getTextWidth(QString text, QFont font);

    private slots:
        void toggleVisibility();
};

#endif // HOVERCLOCK_H
