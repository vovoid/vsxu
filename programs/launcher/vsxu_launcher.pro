#-------------------------------------------------
#
# Project created by QtCreator 2014-09-24T14:25:27
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = vsxu_launcher
TEMPLATE = app

CONFIG += static

SOURCES += main.cpp\
        mainwindow.cpp

DEFINES += VSXU_BIN_DIR=\\\".\\\"

HEADERS  += mainwindow.h

FORMS    += \
    mainwindow.ui
