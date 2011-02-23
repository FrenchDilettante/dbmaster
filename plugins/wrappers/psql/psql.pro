TEMPLATE=lib
CONFIG+=release plugin
QT = core sql
VERSION=0.8
INCLUDEPATH+=../../../src/plugins
TARGET=psqlwrapper
HEADERS += \
    psqlwrapper.h

SOURCES += \
    psqlwrapper.cpp
