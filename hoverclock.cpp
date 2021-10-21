#include "hoverclock.h"
#include "ui_hoverclock.h"

HoverClock::HoverClock(QWidget *parent) : QMainWindow(parent), ui(new Ui::HoverClock)
{
    ui->setupUi(this);
    initializeSettings();

    setStyleSheet("background:transparent;");

    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_TranslucentBackground, true);

    setWindowFlags(Qt::FramelessWindowHint);
    setWindowFlags(Qt::FramelessWindowHint |
                   Qt::WindowStaysOnTopHint |
                   Qt::X11BypassWindowManagerHint);

    startTimer(1000);

    int timeWidth = getTextWidth(settingsHash["timeFormat"].toString(),
                                 settingsHash["timeFont"].value<QFont>());

    int dateWidth = getTextWidth(settingsHash["dateFormat"].toString(),
                                 settingsHash["dateFont"].value<QFont>());

    resize(qMax(timeWidth, dateWidth) + PAINT_OFFSET * 2,
           settingsHash["timeFont"].value<QFont>().pointSize() +
           settingsHash["dateFont"].value<QFont>().pointSize() * 2 + PAINT_OFFSET * 2);

    updateClockPosition();
    createSystemTray();
}

HoverClock::~HoverClock()
{
    delete ui;
}

void HoverClock::initializeSettings()
{
    QSettings settings;

    QFont timeFont("Montserrat Medium", 9);
    timeFont.setLetterSpacing(QFont::AbsoluteSpacing, 1.5);

    QFont dateFont("Montserrat Medium", 7);
    dateFont.setLetterSpacing(QFont::AbsoluteSpacing, 1.5);

    settings.setValue("opacity", settings.value("opacity", 0.5).toFloat());
    settings.setValue("timeFont", settings.value("timeFont", timeFont).value<QFont>());
    settings.setValue("dateFont", settings.value("dateFont", dateFont).value<QFont>());
    settings.setValue("position", settings.value("position", ClockPosition::BOTTOM_RIGHT).toInt());
    settings.setValue("fillColor", settings.value("fillColor", QColor("white")).value<QColor>());
    settings.setValue("timeFormat", settings.value("timeFormat", "hh:mm").toString());
    settings.setValue("dateFormat", settings.value("dateFormat", "dd.MM.yyyy").toString());
    settings.setValue("strokeColor", settings.value("strokeColor", QColor("darkGray")).value<QColor>());
    settings.setValue("strokeThickness", settings.value("strokeThickness", 1.5).toFloat());
    settings.setValue("verticalPadding", settings.value("verticalPadding", 50).toInt());
    settings.setValue("horizontalPadding", settings.value("horizontalPadding", 50).toInt());

    QStringList keys = settings.allKeys();

    for (QString& key : keys)
        settingsHash[key] = settings.value(key);
}

void HoverClock::updateClockPosition()
{
    int screenWidth = QGuiApplication::primaryScreen()->geometry().width();
    int screenHeight = QGuiApplication::primaryScreen()->geometry().height();

    switch (settingsHash["position"].toInt())
    {
        case ClockPosition::TOP_LEFT:
            move(settingsHash["horizontalPadding"].toInt(),
                 settingsHash["verticalPadding"].toInt());
            break;
        case ClockPosition::TOP_RIGHT:
            move(screenWidth - width() - settingsHash["horizontalPadding"].toInt(),
                 settingsHash["verticalPadding"].toInt());
            break;
        case ClockPosition::BOTTOM_LEFT:
            move(settingsHash["horizontalPadding"].toInt(),
                 screenHeight - height() - settingsHash["verticalPadding"].toInt());
            break;
        case ClockPosition::BOTTOM_RIGHT:
            move(screenWidth - width() - settingsHash["horizontalPadding"].toInt(),
                 screenHeight - height() - settingsHash["verticalPadding"].toInt());
            break;
    }
}

void HoverClock::createSystemTray()
{
    QAction *exitAction = new QAction("Quit", this);
    connect(exitAction, &QAction::triggered, this, &QCoreApplication::quit);

    QAction *hideAction = new QAction("Toggle visibility", this);
    connect(hideAction, &QAction::triggered, this, &HoverClock::toggleVisibility);

    QAction *optionsAction = new QAction("Options", this);
    connect(optionsAction, &QAction::triggered, this, &HoverClock::showOptions);

    QMenu *trayMenu = new QMenu(this);
    trayMenu->addAction(hideAction);
    trayMenu->addAction(optionsAction);
    trayMenu->addSeparator();
    trayMenu->addAction(exitAction);

    QSystemTrayIcon *tray = new QSystemTrayIcon(this);
    tray->setContextMenu(trayMenu);

    QPixmap iconPixmap(":/icons/resources/icon.svg");
    QIcon icon(iconPixmap);
    icon.setIsMask(true);

    tray->setIcon(icon);
    tray->show();

    connect(tray, &QSystemTrayIcon::activated, this, [=]() {
        setVisible(!isVisible());
    });
}

void HoverClock::showOptions()
{
    SettingsDialog *settingsDialog = new SettingsDialog(nullptr, &settingsHash);
    settingsDialog->setModal(true);

    connect(settingsDialog, &SettingsDialog::updateClock, this, &HoverClock::updateClockPosition);

    settingsDialog->exec();
}

void HoverClock::toggleVisibility()
{
    setVisible(!isVisible());
}

void HoverClock::timerEvent(QTimerEvent * event)
{
    Q_UNUSED(event);
    repaint();
}

void HoverClock::paintEvent(QPaintEvent * event)
{
    Q_UNUSED(event);

    QString time = QTime::currentTime().toString(settingsHash["timeFormat"].toString());
    QString date = QDate::currentDate().toString(settingsHash["dateFormat"].toString());

    QPainter painter(this);
    painter.setOpacity(settingsHash["opacity"].toFloat());
    painter.setRenderHints(QPainter::Antialiasing |
                           QPainter::TextAntialiasing |
                           QPainter::SmoothPixmapTransform |
                           QPainter::HighQualityAntialiasing);

    QPainterPath path;

    int timeStringWidth = getTextWidth(time, settingsHash["timeFont"].value<QFont>());
    int dateStringWidth = getTextWidth(date, settingsHash["dateFont"].value<QFont>());

    int textPadding = qMax(timeStringWidth, dateStringWidth) + PAINT_OFFSET * 2;

    path.addText(textPadding  / 2 - timeStringWidth / 2,
                 settingsHash["timeFont"].value<QFont>().pointSize() + PAINT_OFFSET,
                 settingsHash["timeFont"].value<QFont>(),
                 time);

    path.addText(textPadding  / 2 - dateStringWidth / 2,
                 settingsHash["dateFont"].value<QFont>().pointSize() * 3 + PAINT_OFFSET,
                 settingsHash["dateFont"].value<QFont>(),
                 date);

    painter.fillPath(path, QBrush(QColor(settingsHash["fillColor"].value<QColor>())));
    painter.strokePath(path, QPen(QColor(settingsHash["strokeColor"].value<QColor>()),
                                  settingsHash["strokeThickness"].toFloat() + 0.0001));
}

float HoverClock::getTextWidth(QString text, QFont font)
{
    QFontMetrics metrics(font);
    return metrics.horizontalAdvance(text);
}
