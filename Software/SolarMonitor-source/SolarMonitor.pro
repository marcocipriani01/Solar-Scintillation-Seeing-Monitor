#-------------------------------------------------
#
# Project created by QtCreator 2019-01-05T10:42:22
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets network serialport printsupport

TARGET = SolarMonitor
TEMPLATE = app
RC_FILE = SolarMonitor.rc
ICON = SolarMonitor.icns

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

unix: QMAKE_LFLAGS += -no-pie

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    config.cpp \
    configdialog.cpp \
    logfile.cpp \
    qcustomplot.cpp \
    ssmondevice.cpp \
    ssmonserver.cpp

HEADERS += \
        mainwindow.h \
    config.h \
    configdialog.h \
    logfile.h \
    mainwindow.h \
    qcustomplot.h \
    ssmondevice.h \
    ssmonserver.h \
    version.h \
    logfile.h

FORMS += \
        mainwindow.ui \
    configdialog.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    ssMon.qrc

DISTFILES += \
    README.md \
    LICENSE \
    resources/reload.png \
    resources/sun.png \
    SolarMonitor.ico \
    SolarMonitor.icns
