# -------------------------------------------------
# Project created by QtCreator 2009-06-01T16:44:04
# -------------------------------------------------

QT += sql webkit
TARGET = dbmaster
TEMPLATE = app
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
    dialogs/aboutdialog.cpp \
    wizards/newdbwizard.cpp \
    config.cpp \
    widgets/querytextedit.cpp \
    wizards/firstlaunchwizard.cpp \
    widgets/resultview.cpp \
    wizards/exportwizard.cpp \
    widgets/dbtreeview.cpp \
    widgets/tooltipframe.cpp \
    plugins/plugindialog.cpp \
    plugins/pluginmanager.cpp \
    iconmanager.cpp \
    dialogs/searchdialog.cpp \
    widgets/colorbutton.cpp \
    tabwidget/schemawidget.cpp \
    dialogs/blobdialog.cpp \
    widgets/resultviewtable.cpp \
    tools/logger.cpp \
    plugins/exportengines/csv/csvexportengine.cpp \
    plugins/exportengines/csv/csvwizardpage.cpp \
    plugins/exportengines/html/htmlexportengine.cpp \
    plugins/exportengines/html/htmlwizardpage.cpp \
    plugins/exportengines/plaintext/plaintextexportengine.cpp \
    plugins/exportengines/plaintext/plaintextwizardpage.cpp \
    plugins/wrappers/db2i/db2iwrapper.cpp \
    plugins/wrappers/mysql/mysqlwrapper.cpp \
    plugins/wrappers/psql/psqlconfig.cpp \
    plugins/wrappers/psql/psqlwrapper.cpp \
    plugins/wrappers/sqlite/sqlitewrapper.cpp
HEADERS += mainwindow.h \
    dbmanager.h \
    tabwidget/tablewidget.h \
    tabwidget/queryeditorwidget.h \
    dialogs/dbdialog.h \
    tabwidget/abstracttabwidget.h \
    dialogs/configdialog.h \
    widgets/dbcombobox.h \
    sqlhighlighter.h \
    dialogs/aboutdialog.h \
    wizards/newdbwizard.h \
    config.h \
    widgets/querytextedit.h \
    wizards/firstlaunchwizard.h \
    widgets/resultview.h \
    wizards/exportwizard.h \
    widgets/dbtreeview.h \
    widgets/tooltipframe.h \
    plugins/sqlwrapper.h \
    plugins/plugindialog.h \
    plugins/plugin.h \
    plugins/pluginmanager.h \
    iconmanager.h \
    dialogs/searchdialog.h \
    widgets/colorbutton.h \
    plugins/exportengine.h \
    db_enum.h \
    tabwidget/schemawidget.h \
    dialogs/blobdialog.h \
    widgets/resultviewtable.h \
    tools/logger.h \
    plugins/exportengines/csv/csvexportengine.h \
    plugins/exportengines/csv/csvwizardpage.h \
    plugins/exportengines/html/htmlexportengine.h \
    plugins/exportengines/html/htmlwizardpage.h \
    plugins/exportengines/plaintext/plaintextexportengine.h \
    plugins/exportengines/plaintext/plaintextwizardpage.h \
    plugins/wrappers/db2i/db2iwrapper.h \
    plugins/wrappers/mysql/mysqlwrapper.h \
    plugins/wrappers/psql/psqlconfig.h \
    plugins/wrappers/psql/psqlwrapper.h \
    plugins/wrappers/sqlite/sqlitewrapper.h
FORMS += mainwindow.ui \
    dialogs/dbdialog.ui \
    tabwidget/queryeditorwidget.ui \
    tabwidget/tablewidget.ui \
    dialogs/configdialog.ui \
    dialogs/aboutdialog.ui \
    wizards/newdbwizard.ui \
    wizards/ndw_firstpage.ui \
    wizards/ndw_secondpage.ui \
    wizards/firstlaunchwizard.ui \
    wizards/ew_firstpage.ui \
    wizards/ew_exportpage.ui \
    widgets/resultview.ui \
    widgets/tooltipframe.ui \
    plugins/plugindialog.ui \
    dialogs/searchdialog.ui \
    tabwidget/schemawidget.ui \
    dialogs/blobdialog.ui \
    plugins/exportengines/csv/csvwizardpage.ui \
    plugins/exportengines/html/htmlwizardpage.ui \
    plugins/exportengines/plaintext/plaintextwizardpage.ui \
    plugins/wrappers/psql/psqlconfig.ui
RESOURCES += icons.qrc \
    syntax.qrc

# ##
# Common
trs.files = ../tr/fr_FR.qm
share.files = img/dbmaster.png

# ##
# MS Windows
win32: { 
    isEmpty(PREFIX):PREFIX = install
    DEFINES += PREFIX=\\\"$${PREFIX}\\\"
    target.path = $${PREFIX}
    INSTALLS = target
    trs.path = $${PREFIX}\\share\\tr
    INSTALLS += trs
    share.path = $${PREFIX}\\share
    INSTALLS += share
}

# ##
# All unix-like
unix:!macx { 
    isEmpty( PREFIX ):PREFIX = /usr/local
    isEmpty( DESKTOPDIR ):DESKTOPDIR = $${PREFIX}/share/applications
    DEFINES += PREFIX=\\\"$${PREFIX}\\\"
    target.path = $${PREFIX}/bin
    INSTALLS = target
    trs.path = $${PREFIX}/share/dbmaster/tr
    INSTALLS += trs
    share.path = $${PREFIX}/share/dbmaster
    INSTALLS += share
    desktop.path = $${PREFIX}/share/applications
    desktop.files = share/dbmaster.desktop
    INSTALLS += desktop
    icon.path = $${PREFIX}/share/icons
    icon.files = img/dbmaster.png
    INSTALLS += icon
}

macx {
    TARGET = DbMaster
    ICON = ../dist/dbmaster.icns
}
