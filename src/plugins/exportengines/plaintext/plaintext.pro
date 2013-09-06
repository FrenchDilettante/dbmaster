TEMPLATE=lib
CONFIG+=release plugin
VERSION=0.8
INCLUDEPATH+=../../../src/plugins
TARGET=plaintextexportengine

# ##
# MS Windows
win32: {
    isEmpty(PREFIX):PREFIX = ..\\..\\..\\src\\install
    DEFINES += PREFIX=\\\"$${PREFIX}\\\"
    target.path = $${PREFIX}\\plugins
    INSTALLS = target
}

HEADERS += \
    plaintextexportengine.h \
    plaintextwizardpage.h

SOURCES += \
    plaintextexportengine.cpp \
    plaintextwizardpage.cpp

FORMS += \
    plaintextwizardpage.ui
