VERSION=0.0.2
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    hoverclock.cpp \
    main.cpp \
    run_guard.cpp \
    settings_dialog.cpp

HEADERS += \
    clock_position.h \
    hoverclock.h \
    run_guard.h \
    settings_dialog.h

FORMS += \
    hoverclock.ui \
    settings_dialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
