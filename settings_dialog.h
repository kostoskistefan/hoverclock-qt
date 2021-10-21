#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QScreen>
#include <QSettings>
#include <QFontDialog>
#include <QColorDialog>
#include "clock_position.h"

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr, QHash<QString, QVariant> *settings = nullptr);
    ~SettingsDialog();

signals:
    void updateClock();

private:
    Ui::SettingsDialog *ui;
    QHash<QString, QVariant> *settings;

    template <typename T>
    void updateSetting(QString settingName, T value);
    void setFontPickerFont(QPushButton *fontPicker, QFont font);
    void setColorPickerPalette(QPushButton *colorPicker, QColor color);
    void changeFontSetting(QPushButton *fontPicker, QString fontSettingName);
    void changeColorSetting(QPushButton *colorPicker, QString colorSettingName);

private slots:
    void save();
    void cancel();
};

#endif // SETTINGSDIALOG_H
