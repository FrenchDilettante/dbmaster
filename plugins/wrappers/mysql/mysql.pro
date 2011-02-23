TEMPLATE=lib
CONFIG+=release plugin
QT = core sql
VERSION=0.8
INCLUDEPATH+=../../../src/plugins
TARGET=mysqlwrapper
HEADERS += \
    mysqlwrapper.h

SOURCES += \
    mysqlwrapper.cpp
