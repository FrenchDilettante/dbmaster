TEMPLATE=lib
CONFIG+=release plugin
QT = core gui sql
VERSION=0.8
INCLUDEPATH+=../../../src/plugins
TARGET=mysqlwrapper
HEADERS += \
    mysqlwrapper.h

SOURCES += \
    mysqlwrapper.cpp

# ##
# MS Windows
win32: {
    isEmpty(PREFIX):PREFIX = ..\\..\\..\\src\\install
    DEFINES += PREFIX=\\\"$${PREFIX}\\\"
    target.path = $${PREFIX}\\plugins
    INSTALLS = target
}
