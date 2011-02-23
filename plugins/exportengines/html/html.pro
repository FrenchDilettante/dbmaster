TEMPLATE=lib
CONFIG+=release plugin
VERSION=0.8
INCLUDEPATH+=../../../src/plugins
TARGET=htmlexportengine

HEADERS += \
    htmlexportengine.h \
    htmlwizardpage.h

SOURCES += \
    htmlexportengine.cpp \
    htmlwizardpage.cpp

FORMS += \
    htmlwizardpage.ui
