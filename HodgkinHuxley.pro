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
    settings.cpp \
    welcome.cpp

HEADERS  += mainwindow.h \
    qcustomplot.h \
    settings.h \
    welcome.h

FORMS    += mainwindow.ui \
    settings.ui \
    welcome.ui

OTHER_FILES +=

RESOURCES += \
    resources.qrc
