#-------------------------------------------------
#
# Project created by QtCreator 2016-05-17T17:59:03
#
#-------------------------------------------------

QT       += core gui network printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = exchangeCalculator
TEMPLATE = app

TRANSLATIONS = fileName_ua.ts fileName_en.ts

SOURCES += main.cpp\
        mainwindow.cpp \
    downloader.cpp \
    parser.cpp \
    filename.cpp \
    parsworker.cpp

HEADERS  += mainwindow.h \
    downloader.h \
    parser.h \
    filename.h \
    parsworker.h

FORMS    += mainwindow.ui \
    filename.ui

RESOURCES += \
    res.qrc
