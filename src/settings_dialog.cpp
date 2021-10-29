#include "settings_dialog.h"
#include "ui_settings_dialog.h"

SettingsDialog::SettingsDialog(QWidget *parent, QHash<QString, QVariant> *settings) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    setWindowIcon(QIcon(":/resources/icons/hoverclock.png"));

    this->settings = settings;

    initializeSettingsDialog();

    connectSignals();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::initializeSettingsDialog()
{
    ui->saveButton->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_DialogApplyButton)));
    ui->cancelButton->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_DialogCancelButton)));

    QPixmap image(":/resources/icons/hoverclock.png");
    ui->imageContainer->setPixmap(image.scaled(rect().width(), rect().height() / 3,
                                               Qt::AspectRatioMode::KeepAspectRatio,
                                               Qt::TransformationMode::SmoothTransformation));

    ui->versionLabel->setText(QString("Version: ") + APP_VERSION);

    ui->verticalPaddingSpinBox->setMaximum(QGuiApplication::primaryScreen()->geometry().height());
    ui->horizontalPaddingSpinBox->setMaximum(QGuiApplication::primaryScreen()->geometry().width());

    ui->showTimeCheckBox->setCheckState(static_cast<Qt::CheckState>((*settings)["showTime"].toInt()));
    ui->showDateCheckBox->setCheckState(static_cast<Qt::CheckState>((*settings)["showDate"].toInt()));
    ui->enableBlacklistCheckBox->setCheckState(static_cast<Qt::CheckState>((*settings)["enableBlacklist"].toInt()));

    ui->positionComboBox->addItem("Top Left");
    ui->positionComboBox->addItem("Top Right");
    ui->positionComboBox->addItem("Bottom Left");
    ui->positionComboBox->addItem("Bottom Right");

    ui->opacityDoubleSpinBox->setValue((*settings)["opacity"].toFloat());
    ui->timeFormatLineEdit->setText((*settings)["timeFormat"].toString());
    ui->dateFormatLineEdit->setText((*settings)["dateFormat"].toString());
    ui->positionComboBox->setCurrentIndex((*settings)["position"].toInt());
    ui->verticalPaddingSpinBox->setValue((*settings)["verticalPadding"].toInt());
    ui->blacklistTextEdit->setText((*settings)["applicationBlacklist"].toString());
    ui->strokeThicknessSpinBox->setValue((*settings)["strokeThickness"].toFloat());
    ui->horizontalPaddingSpinBox->setValue((*settings)["horizontalPadding"].toInt());

    setColorPickerPalette(ui->fillColorPicker, (*settings)["fillColor"].value<QColor>());
    setColorPickerPalette(ui->strokeColorPicker, (*settings)["strokeColor"].value<QColor>());

    setFontPickerFont(ui->timeFontPicker, (*settings)["timeFont"].value<QFont>());
    setFontPickerFont(ui->dateFontPicker, (*settings)["dateFont"].value<QFont>());
}

void SettingsDialog::connectSignals()
{
    connect(ui->saveButton, &QPushButton::clicked, this, &SettingsDialog::save);
    connect(ui->cancelButton, &QPushButton::clicked, this, &SettingsDialog::cancel);

    connect(ui->opacityDoubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [=](){
        updateSetting("opacity", ui->opacityDoubleSpinBox->value());
    });

    connect(ui->positionComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](){
        updateSetting("position", static_cast<ClockPosition>(ui->positionComboBox->currentIndex()));
    });

    connect(ui->timeFormatLineEdit, &QLineEdit::textChanged, this, [=](){
        updateSetting("timeFormat", ui->timeFormatLineEdit->text());
    });

    connect(ui->dateFormatLineEdit, &QLineEdit::textChanged, this, [=](){
        updateSetting("dateFormat", ui->dateFormatLineEdit->text());
    });

    connect(ui->strokeThicknessSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [=](){
        updateSetting("strokeThickness", ui->strokeThicknessSpinBox->value());
    });

    connect(ui->verticalPaddingSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](){
        updateSetting("verticalPadding", ui->verticalPaddingSpinBox->value());
    });

    connect(ui->horizontalPaddingSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](){
        updateSetting("horizontalPadding", ui->horizontalPaddingSpinBox->value());
    });

    connect(ui->fillColorPicker, &QPushButton::clicked, this, [=](){
        changeColorSetting(ui->fillColorPicker, "fillColor");
    });

    connect(ui->strokeColorPicker, &QPushButton::clicked, this, [=](){
        changeColorSetting(ui->strokeColorPicker, "strokeColor");
    });

    connect(ui->timeFontPicker, &QPushButton::clicked, this, [=](){
        changeFontSetting(ui->timeFontPicker, "timeFont");
    });

    connect(ui->dateFontPicker, &QPushButton::clicked, this, [=](){
        changeFontSetting(ui->dateFontPicker, "dateFont");
    });

    connect(ui->showTimeCheckBox, &QCheckBox::clicked, this, [=](){
        updateSetting("showTime", ui->showTimeCheckBox->checkState());
    });

    connect(ui->showDateCheckBox, &QCheckBox::clicked, this, [=](){
        updateSetting("showDate", ui->showDateCheckBox->checkState());
    });

    connect(ui->enableBlacklistCheckBox, &QCheckBox::clicked, this, [=](){
        updateSetting("enableBlacklist", ui->enableBlacklistCheckBox->checkState());
    });

    ui->blacklistTextEdit->installEventFilter(this);
}

bool SettingsDialog::eventFilter(QObject *watched, QEvent *event)
{
    Q_UNUSED(watched);

    if(event->type() == QEvent::FocusOut)
    {
        (*settings)["applicationBlacklist"] = ui->blacklistTextEdit->toPlainText();
        emit updateBlacklist();
    }

    return false;
}

void SettingsDialog::changeColorSetting(QPushButton *colorPicker, QString colorSettingName)
{
    QColor color = QColorDialog::getColor((*settings)[colorSettingName].value<QColor>());

    if(color.isValid())
    {
        updateSetting(colorSettingName, color);
        setColorPickerPalette(colorPicker, color);
    }
}

void SettingsDialog::changeFontSetting(QPushButton *fontPicker, QString fontSettingName)
{
    bool fontFound;
    QFont font = QFontDialog::getFont(&fontFound, (*settings)[fontSettingName].value<QFont>());

    if (fontFound)
    {
        updateSetting(fontSettingName, font);
        setFontPickerFont(fontPicker, font);
    }
}

void SettingsDialog::setColorPickerPalette(QPushButton *colorPicker, QColor color)
{
    QPalette palette = colorPicker->palette();
    palette.setColor(QPalette::Button, color);
    colorPicker->setPalette(palette);
    colorPicker->update();
}

void SettingsDialog::setFontPickerFont(QPushButton *fontPicker, QFont font)
{
    QString fontLabel = font.family();
    fontLabel += " ";
    fontLabel += QString::number(font.pointSize());
    fontPicker->setText(fontLabel);
}

template <typename SettingValue>
void SettingsDialog::updateSetting(QString settingName, SettingValue value)
{
    (*settings)[settingName] = value;
    emit updateClock();
}

void SettingsDialog::save()
{
    QSettings configuration;

    QStringList keys = settings->keys();

    for (QString& key : keys)
        configuration.setValue(key, (*settings)[key]);

    emit updateClock();

    accept();
}

void SettingsDialog::cancel()
{
    QSettings configuration;

    QStringList keys = configuration.allKeys();

    for (QString& key : keys)
        (*settings)[key] = configuration.value(key);

    emit updateClock();

    reject();
}
