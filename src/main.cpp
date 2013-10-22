#include "config.h"
#include "dbmanager.h"
#include "iconmanager.h"
#include "mainwindow.h"
#include "plugins/pluginmanager.h"
#include "sqlhighlighter.h"
#include "tabwidget/abstracttabwidget.h"
#include "tools/logger.h"
#include "widgets/querytextedit.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QSplashScreen>

int main(int argc, char *argv[]) {
  QApplication::setApplicationName("dbmaster");
  QApplication::setApplicationVersion("0.9.0");
  QApplication::setOrganizationDomain("dbmaster.org");
  QApplication a(argc, argv);
  a.setWindowIcon(QIcon(":/img/dbmaster.png"));

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

  path = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
#if defined (Q_WS_WIN)
  path = QDir::currentPath();
#endif
  
  QTranslator qtTranslator;
  qtTranslator.load("qt_" + lang, path);
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
