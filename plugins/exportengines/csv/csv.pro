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
