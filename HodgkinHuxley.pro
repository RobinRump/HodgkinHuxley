#-------------------------------------------------
#
# Project created by QtCreator 2013-07-29T14:10:40
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = HodgkinHuxley
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qcustomplot.cpp \
    welcome.cpp \
    preferences.cpp \
    config.cpp

HEADERS  += mainwindow.h \
    qcustomplot.h \
    welcome.h \
    preferences.h \
    config.h

FORMS    += mainwindow.ui \
    welcome.ui \
    preferences.ui

OTHER_FILES += \
    README.md \
    LICENSE \
    CHANGELOG.md

RESOURCES += \
    resources.qrc
