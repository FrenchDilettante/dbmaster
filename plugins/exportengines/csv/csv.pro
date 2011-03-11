TEMPLATE=lib
CONFIG+=release plugin
VERSION=0.8
INCLUDEPATH+=../../../src/plugins
TARGET=csvexportengine
HEADERS += \
    csvexportengine.h \
    csvwizardpage.h

SOURCES += \
    csvexportengine.cpp \
    csvwizardpage.cpp

FORMS += \
    csvwizardpage.ui

# ##
# MS Windows
win32: {
    isEmpty(PREFIX):PREFIX = ..\\..\\..\\src\\install
    DEFINES += PREFIX=\\\"$${PREFIX}\\\"
    target.path = $${PREFIX}\\plugins
    INSTALLS = target
}

