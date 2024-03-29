VERSION=1.0.1
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

QT       += core gui svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

LIBS += -lxcb

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    hoverclock-qt.cpp \
    main.cpp \
    run_guard.cpp \
    settings_dialog.cpp \
    focus_event.cpp

HEADERS += \
    clock_position.h \
    hoverclock-qt.h \
    run_guard.h \
    settings_dialog.h \
    focus_event.h

FORMS += \
    hoverclock-qt.ui \
    settings_dialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
