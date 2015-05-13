#-------------------------------------------------
#
# Project created by QtCreator 2015-04-26T20:17:05
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Music
TEMPLATE = app


SOURCES += main.cpp\
        start.cpp \
    song.cpp

HEADERS  += start.h \
    song.h

FORMS    += start.ui

RESOURCES += \
    res.qrc
