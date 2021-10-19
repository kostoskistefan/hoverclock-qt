#ifndef HOVERCLOCK_H
#define HOVERCLOCK_H

#include <QMainWindow>
#include <QTime>
#include <QScreen>
#include <QPainter>
#include <QPainterPath>

QT_BEGIN_NAMESPACE
namespace Ui { class HoverClock; }
QT_END_NAMESPACE

#define PADDING 55
#define PAINT_OFFSET 8

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
};

#endif // HOVERCLOCK_H
