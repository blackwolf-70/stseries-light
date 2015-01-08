#-------------------------------------------------
#
# Project created by QtCreator 2015-01-08T11:51:21
#
#-------------------------------------------------

QT       += core
QT       -= gui

TARGET = key-light
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp
LIBS    += -lhidapi-hidraw -lhidapi-libusb

