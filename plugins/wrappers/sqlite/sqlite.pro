TEMPLATE=lib
CONFIG+=release plugin
QT = core gui sql
VERSION=0.8
INCLUDEPATH+=../../../src/plugins
TARGET=sqlitewrapper
HEADERS += \
    sqlitewrapper.h

SOURCES += \
    sqlitewrapper.cpp
