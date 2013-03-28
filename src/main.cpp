/*
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 */


#include <QtGui/QApplication>
#include "config.h"
#include "dbmanager.h"
#include "iconmanager.h"
#include "mainwindow.h"
#include "plugins/pluginmanager.h"
#include "sqlhighlighter.h"
#include "tabwidget/abstracttabwidget.h"
#include "tools/logger.h"
#include "widgets/querytextedit.h"

int main(int argc, char *argv[]) {
  QApplication::setApplicationName("dbmaster");
  QApplication::setApplicationVersion("0.8.2");
  QApplication::setOrganizationDomain("dbmaster.org");
  QApplication a(argc, argv);

  QSplashScreen splash(QPixmap(":/img/splash.png"));
  splash.show();

  Logger *logger = new Logger;

  // Loading translations
  splash.showMessage(QObject::tr("Loading translations..."), Qt::AlignBottom);

  QTranslator translator;
  // getting the current locale
  QString lang = QLocale::system().name();
  QString transdir;
#if defined(Q_WS_X11)
  // for *nix
  transdir = QString(PREFIX).append("/share/dbmaster/tr");
#endif

#if defined(Q_WS_WIN)
  transdir = "share/tr";
#endif

  QString path;
  /* on teste d'abord si le .qm est dans le dossier courant -> alors on est en
   * dev, on le charge, sinon il est dans le répertoire d'installation. */
  path = QString("../../dbmaster/tr/%1.qm").arg(lang);
  if (!QFile::exists(path))
    path = transdir.append("/%1.qm").arg(lang);
  translator.load(path);
  a.installTranslator(&translator);
  
  QTranslator qtTranslator;
  qtTranslator.load("qt_" + lang,
#if defined (Q_WS_X11)
                    QLibraryInfo::location(QLibraryInfo::TranslationsPath));
#endif
#if defined (Q_WS_MAC)
                    QLibraryInfo::location(QLibraryInfo::TranslationsPath));
#endif
#if defined (Q_WS_WIN)
                    QDir::currentPath());
#endif
  a.installTranslator(&qtTranslator);

  // Ajout des plugins
  splash.showMessage(QObject::tr("Loading plugins..."), Qt::AlignBottom);
  PluginManager::init();

  splash.showMessage(QObject::tr("Initialization..."), Qt::AlignBottom);

  IconManager::init();
  DbManager::init();
  Config::init();
  QueryTextEdit::reloadCompleter();

  MainWindow *w = new MainWindow();
  w->show();
  splash.finish(w);

  if (a.arguments().size() >= 2) {
    for (int i=1; i<a.arguments().size(); i++) {
      w->openQuery(a.arguments()[i]);
    }
  }

  return a.exec();
}
