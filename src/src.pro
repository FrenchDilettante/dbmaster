# -------------------------------------------------
# Project created by QtCreator 2009-06-01T16:44:04
# -------------------------------------------------
QT += sql
TARGET = ../bin/dbmaster
TEMPLATE = app
MOC_DIR = ../build
OBJECTS_DIR = ../build
UI_DIR = ../build
SOURCES += main.cpp \
    mainwindow.cpp \
    dbmanager.cpp \
    tabwidget/tablewidget.cpp \
    tabwidget/queryeditorwidget.cpp \
    dialogs/dbdialog.cpp \
    tabwidget/abstracttabwidget.cpp \
    dialogs/configdialog.cpp \
    widgets/dbcombobox.cpp \
    sqlhighlighter.cpp \
    dialogs/logdialog.cpp \
    dialogs/aboutdialog.cpp \
    third/QColorButton.cpp \
    wizards/newdbwizard.cpp \
    config.cpp \
    widgets/querytextedit.cpp \
    wizards/firstlaunchwizard.cpp \
    widgets/resultview.cpp \
    wizards/exportwizard.cpp \
    widgets/dbtreeview.cpp \
    query/queryscheduler.cpp \
    query/querythread.cpp \
    query/querytoken.cpp \
    widgets/tooltipframe.cpp \
    plugins/wrappers/wrapper.cpp \
    plugins/plugindialog.cpp \
    plugins/pluginmanager.cpp \
    plugins/plugin.cpp \
    iconmanager.cpp \
    plugins/exportengines/exportengine.cpp \
    dialogs/searchdialog.cpp
HEADERS += mainwindow.h \
    dbmanager.h \
    tabwidget/tablewidget.h \
    tabwidget/queryeditorwidget.h \
    dialogs/dbdialog.h \
    tabwidget/abstracttabwidget.h \
    dialogs/configdialog.h \
    widgets/dbcombobox.h \
    sqlhighlighter.h \
    dialogs/logdialog.h \
    dialogs/aboutdialog.h \
    third/QColorButton.h \
    wizards/newdbwizard.h \
    config.h \
    widgets/querytextedit.h \
    wizards/firstlaunchwizard.h \
    widgets/resultview.h \
    wizards/exportwizard.h \
    widgets/dbtreeview.h \
    query/queryscheduler.h \
    query/querythread.h \
    query/querytoken.h \
    widgets/tooltipframe.h \
    plugins/wrappers/wrapper.h \
    plugins/plugindialog.h \
    plugins/plugin.h \
    plugins/pluginmanager.h \
    iconmanager.h \
    plugins/exportengines/exportengine.h \
    dialogs/searchdialog.h
FORMS += mainwindow.ui \
    dialogs/dbdialog.ui \
    tabwidget/queryeditorwidgetclass.ui \
    tabwidget/tablewidgetclass.ui \
    dialogs/configdialogclass.ui \
    dialogs/logdialogclass.ui \
    dialogs/aboutdialog.ui \
    wizards/newdbwizard.ui \
    wizards/ndw_firstpage.ui \
    wizards/ndw_secondpage.ui \
    wizards/firstlaunchwizard.ui \
    wizards/exportwizard.ui \
    wizards/ew_sqlpage.ui \
    wizards/ew_firstpage.ui \
    wizards/ew_exportpage.ui \
    wizards/ew_csvpage.ui \
    widgets/resultview.ui \
    widgets/tooltipframe.ui \
    plugins/plugindialog.ui \
    wizards/ew_htmlpage.ui \
    dialogs/searchdialog.ui
RESOURCES += icons.qrc
TRANSLATIONS += tr/fr_FR.po

# ##
# Common
translations.files = tr/fr_FR.qm
sqlsyntax.files = share/sql_basics \
    share/sql_functions \
    share/sql_operators \
    share/sql_types
share.files = share/prefs.xml \
    share/html/back.png \
    share/html/dbmaster.png \
    share/html/index_en.html \
    share/html/index_fr.html \
    share/html/main.css \
    share/html/rightarrow.png \
    img/dbmaster.png

# ##
# MS Windows
win32: { 
    isEmpty(PREFIX):PREFIX = install
    DEFINES += PREFIX=\\\"$${PREFIX}\\\"
    target.path = $${PREFIX}
    INSTALLS = target
    translations.path = $${PREFIX}\\share\\tr
    INSTALLS += translations
    sqlsyntax.path = $${PREFIX}\\share\\sqlsyntax
    INSTALLS += sqlsyntax
    share.path = $${PREFIX}\\share
    INSTALLS += share
}

# ##
# All unix-like
unix:!macx { 
    isEmpty( PREFIX ):PREFIX = /usr
    isEmpty( DESKTOPDIR ):DESKTOPDIR = /usr/share/applications
    DEFINES += PREFIX=\\\"$${PREFIX}\\\"
    target.path = $${PREFIX}/bin
    INSTALLS = target
    translations.path = $${PREFIX}/share/dbmaster/tr
    INSTALLS += translations
    sqlsyntax.path = $${PREFIX}/share/dbmaster/sqlsyntax
    INSTALLS += sqlsyntax
    share.path = $${PREFIX}/share/dbmaster
    INSTALLS += share
    desktop.path = $${PREFIX}/share/applications
    desktop.files = share/dbmaster.desktop
    INSTALLS += desktop
    icon.path = $${PREFIX}/share/icons
    icon.files = img/dbmaster.png
    INSTALLS += icon
}

include(../updateqm.pri)
