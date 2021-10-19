#include "hoverclock.h"
#include "ui_hoverclock.h"

HoverClock::HoverClock(QWidget *parent) : QMainWindow(parent), ui(new Ui::HoverClock)
{
    ui->setupUi(this);

    setStyleSheet("background:transparent;");

    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint);

    move(QGuiApplication::primaryScreen()->geometry().width() - PADDING * 2,
         QGuiApplication::primaryScreen()->geometry().height() - PADDING);

    startTimer(1000);
}

HoverClock::~HoverClock()
{
    delete ui;
}

void HoverClock::timerEvent(QTimerEvent * event)
{
    Q_UNUSED(event);
    repaint();
}

void HoverClock::paintEvent(QPaintEvent * event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    QPainterPath path;
    QFont drawFont("Montserrat Medium", 9);
    drawFont.setLetterSpacing(QFont::AbsoluteSpacing, 1.5);

    painter.setOpacity(0.5);

    path.addText(PAINT_OFFSET,
                 drawFont.pointSize() + PAINT_OFFSET,
                 drawFont,
                 QTime::currentTime().toString("hh:mm"));

    painter.setRenderHints(QPainter::Antialiasing |
                           QPainter::TextAntialiasing |
                           QPainter::SmoothPixmapTransform |
                           QPainter::HighQualityAntialiasing);

    painter.fillPath(path, QBrush(Qt::white));
    painter.strokePath(path, QPen(Qt::darkGray, 1.5));

    resize(path.boundingRect().size().toSize().width() + PAINT_OFFSET * 2,
           path.boundingRect().size().toSize().height() + PAINT_OFFSET * 2);
}
