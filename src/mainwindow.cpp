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


#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "dbmanager.h"
#include "iconmanager.h"

#include "dialogs/dbdialog.h"
#include "query/queryscheduler.h"
#include "tabwidget/abstracttabwidget.h"
#include "tabwidget/queryeditorwidget.h"
#include "tabwidget/tablewidget.h"
#include "widgets/dbtreeview.h"

DbDialog     *MainWindow::dbDialog;
NewDbWizard  *MainWindow::dbWizard;
PluginDialog *MainWindow::pluginDialog;

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
{
  dbDialog      = new DbDialog(this);
  dbWizard      = new NewDbWizard(this);
  pluginDialog  = new PluginDialog(this);

  setupUi(this);

  lastPath  = QDir::homePath();

  setupWidgets();
  LogDialog::instance()->append(tr("Starting DbMaster"));
  setupConnections();
}

MainWindow::~MainWindow()
{
}

void MainWindow::addRecentFile(QString file)
{
  if(recentFiles.indexOf(file) >= 0)
    recentFiles.removeAt(recentFiles.indexOf(file));

  recentFiles.insert(0, file);
  while(recentFiles.size() > 10)
    recentFiles.removeLast();
  refreshRecent();
}

void MainWindow::checkDb(QSqlDatabase *db)
{
  if(db->lastError().type() != QSqlError::NoError)
    QMessageBox::critical(this,
                          tr("Error !"),
                          tr("Unable to connect :\n%1")
                          .arg(db->lastError().text()));
}

void MainWindow::clearRecent()
{
  recentFiles.clear();
  refreshRecent();
}

void MainWindow::closeEvent( QCloseEvent *event )
{
  // counts how many tabs are saved
  int saved = 0;

  // remembers the last not closed tab
  int n = 0;

  // note : && !closed stops immediatly the for loop if a tab wasn't closed
  for(int i = 0; i < tabWidget->count(); i++)
  {
    if(!((AbstractTabWidget*)(tabWidget->widget(i)))->isSaved())
    {
      saved++;
      n = i;
    }
  }

  // if all tabs are saved
  if(saved == 0)
    event->accept();

  // if only one is unsaved
  if(saved == 1)
  {
    if(tabWidget->widget(n)->close())
      event->accept();
    else
      event->ignore();
  } else {
    // but if more than one is unsaved
    bool yesAll = false;
    AbstractTabWidget *tab;

    // each tab will be asked
    for(int i = 0; i < tabWidget->count(); i++)
    {
      tab = (AbstractTabWidget*) tabWidget->widget(i);
      if(yesAll)
      {
        if(!tab->save())
        {
          event->ignore();
          return;
        }
      }
      else
      {
        switch(tab->confirmCloseAll())
        {
        case QMessageBox::No:
          continue;

        case QMessageBox::Cancel:
          event->ignore();
          return;

        case QMessageBox::Save:
          if(!tab->save())
          {
            event->ignore();
            return;
          }
          break;

        case QMessageBox::SaveAll:
          if(!tab->save())
          {
            event->ignore();
            return;
          }
          yesAll = true;
          break;

        case QMessageBox::NoAll:
          event->accept();
          break;
        }
      }
    }
  }

  QSettings s;
  s.beginGroup("mainwindow");
  if(windowState().testFlag(Qt::WindowMaximized))
  {
    s.setValue("maximized", true);
    s.remove("size");
    s.remove("position");
  } else {
    s.setValue("size", size());
    s.setValue("position", pos());
    s.remove("maximized");
  }
  s.setValue("tooltips", tooltipButton->isChecked());
  s.setValue("dock_area", dockWidgetArea(dockWidget));
  s.endGroup();

  s.beginWriteArray("recentfiles", recentFiles.size());
  for(int i=0; i<recentFiles.size(); i++)
  {
    s.setArrayIndex(i);
    s.setValue("entry", recentFiles[i]);
  }
  s.endArray();

  DbManager::closeAll();

  event->accept();
}

void MainWindow::closeCurrentTab()
{
  closeTab(tabWidget->currentIndex());
}

void MainWindow::closeTab(int nb)
{
  if(nb < 0 || nb >= tabWidget->count())
    return;

  // don't close the tab if the user cancelled saving
  if(!tabWidget->widget(nb)->close())
    return;

  // then remove the widget
  tabWidget->removeTab(nb);
}

void MainWindow::copy()
{
  if(currentTab() != 0)
    currentTab()->copy();
}

/**
 * Launch the new db wizard
 */
void MainWindow::createDatabase()
{
  dbWizard->exec();
}

/**
 * Returns the current tab or NULL.
 */
AbstractTabWidget* MainWindow::currentTab()
{
  if(tabWidget->count() > 0)
    return (AbstractTabWidget*) tabWidget->currentWidget();
  else
    return NULL;
}

void MainWindow::cut()
{
  if(currentTab() != 0)
    currentTab()->cut();
}

/**
 * Create a query tab and connect it.
 *
 * @return a new QueryEditorWidget
 */
QueryEditorWidget* MainWindow::newQuery()
{
  QueryEditorWidget *w = new QueryEditorWidget();
  tabWidget->addTab(w, w->icon(), w->title());
  tabWidget->setCurrentIndex(tabWidget->count() - 1);

  connect(w, SIGNAL(actionAvailable(AbstractTabWidget::Actions)),
          this, SLOT(refreshTab()));
  connect(w, SIGNAL(fileChanged(QString)), this, SLOT(addRecentFile(QString)));
  connect(w, SIGNAL(modificationChanged(bool)), this, SLOT(refreshTab()));
  connect(w, SIGNAL(tableRequested(QSqlDatabase*,QString)),
          this, SLOT(openTable(QSqlDatabase*,QString)));
  connect(w, SIGNAL(titleChanged(QString)), this, SLOT(setTabText(QString)));

  return w;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
  if(event->modifiers() && Qt::ControlModifier && event->key() == Qt::Key_W)
    closeCurrentTab();
}

void MainWindow::nextTab()
{
  int count = tabWidget->count();
  int index = tabWidget->currentIndex();

  if(count >= 2 && index < count)
    tabWidget->setCurrentIndex(index+1);
}

void MainWindow::openRecent()
{
  int index = recentActions.indexOf((QAction*)sender());
  if(index == -1)
  {
    qDebug() << "Critical error. Please report.";
    return;
  }

  QFileInfo info(recentFiles[index]);
  if(!info.exists())
  {
    recentFiles.removeAt(index);
    refreshRecent();
    return;
  }

  QueryEditorWidget *w = newQuery();
  w->open(recentFiles[index]);
  AbstractTabWidget::lastDir = info.absolutePath();
}

/**
 * Shows a browser dialog to open a query file
 */
void MainWindow::openQuery()
{
  QString f = QFileDialog::getOpenFileName( this,
                           tr( "Select a query to open" ),
                           AbstractTabWidget::lastDir.path(),
                           tr( "Query file (*.sql);;All files (*.*)" ) );

  if(f.isNull())
    return;

  QueryEditorWidget *w = newQuery();
  w->open(f);

  QFileInfo fi = QFileInfo(f);
  AbstractTabWidget::lastDir = fi.absoluteDir();
}

/**
 * Open a table browser tab
 *
 * @param db
 *          concerned database
 */
void MainWindow::openTable(QSqlDatabase *db, QString table)
{
  // generates an unique ID for the table
  QString id = QString("t %1 on %2")
               .arg(table)
               .arg(db->connectionName());

  // First of all, the table may be already openned. We're gonna check it.
  int index = -1;
  for(int i=0; i<tabWidget->count(); i++)
  {
    if(((AbstractTabWidget*)tabWidget->widget(i))->id() == id)
    {
      index = i;
      break;
    }
  }

  /* If the index is >= 0, then a table browser is already openned with the same
     ID. */
  if(index >= 0)
  {
    tabWidget->setCurrentIndex(index);
  }
  else
  {
    TableWidget *view = new TableWidget(table, db, this);
    index = tabWidget->addTab(view, view->icon(), table);
    tabWidget->setCurrentIndex(index);
  }
}

void MainWindow::paste()
{
  if(currentTab())
    currentTab()->paste();
}

void MainWindow::previousTab()
{
  int count = tabWidget->count();
  int index = tabWidget->currentIndex();

  if(count >= 2 && index > 0)
    tabWidget->setCurrentIndex(index - 1);
}

void MainWindow::print()
{
  if(currentTab()->availableActions() && AbstractTabWidget::Print)
  {
    QPrinter *printer = currentTab()->printer();
    QPrintDialog *printDialog = new QPrintDialog(printer, this);
    if(printDialog->exec() == QDialog::Accepted)
      currentTab()->print();
  }
}

void MainWindow::redo()
{
  if(currentTab() != 0)
    currentTab()->redo();
}

/**
 * Called after the tab widget's index changed.
 */
void MainWindow::refreshTab()
{
  // toolbar's actions
  AbstractTabWidget::Actions acts;
  if(currentTab())
    acts = currentTab()->availableActions();

  foreach(AbstractTabWidget::Action a, actionMap.keys())
    actionMap[a]->setEnabled(acts.testFlag(a));
}

void MainWindow::refreshRecent()
{
  for(int i=0; i<10; i++)
  {
    recentActions[i]->setVisible(i < recentFiles.size());
    if(i<recentFiles.size())
      recentActions[i]->setText(QFileInfo(recentFiles[i]).fileName());
  }

  if(recentFiles.size() == 0)
  {
    actionClearRecent->setText(tr("No recent file"));
  } else {
    actionClearRecent->setText(tr("Clear"));
  }
  actionClearRecent->setEnabled(!recentFiles.size() == 0);
}

void MainWindow::reloadDbList()
{
  for(int i = 0; i < tabWidget->count(); i++)
    ((AbstractTabWidget*)tabWidget->widget(i))->refresh();
}

void MainWindow::saveQuery()
{
  if(currentTab() != 0)
    currentTab()->save();
}

void MainWindow::saveQueryAs()
{
  if(currentTab())
    currentTab()->saveAs();
}

void MainWindow::search()
{
  if(currentTab() && currentTab()->textEdit())
  {
    searchDialog->setEditor(currentTab()->textEdit());
    searchDialog->exec();
  }
}

void MainWindow::selectAll()
{
  if(currentTab() != 0)
    currentTab()->selectAll();
}

void MainWindow::setQueryCount(int count)
{
  queriesStatusLabel->setText(tr("%1 queries pending...")
                              .arg(QString::number(count)));
  if(count == 0)
    QTimer::singleShot(3000, queriesStatusLabel, SLOT(clear()));
}

void MainWindow::setTabText(QString title)
{
  int i = tabWidget->indexOf((QWidget*)sender());
  if(!((AbstractTabWidget*)tabWidget->widget(i))->isSaved())
    title.prepend("* ");
  if(i < 0)
    return;
  tabWidget->setTabText(i, title);
}

void MainWindow::setupConnections()
{
  /*
   * Actions
   */
  connect(actionAbout,        SIGNAL(triggered()),  aboutDial,     SLOT(exec()));
  connect(actionAddDb,        SIGNAL(triggered()),  this,          SLOT(createDatabase()));
  connect(actionClearRecent,  SIGNAL(triggered()),  this,          SLOT(clearRecent()));
  connect(actionCloseTab,     SIGNAL(triggered()),  this,          SLOT(closeCurrentTab()));
  connect(actionCopy,         SIGNAL(triggered()),  this,          SLOT(copy()));
  connect(actionCut,          SIGNAL(triggered()),  this,          SLOT(cut()));
  connect(actionDbManager,    SIGNAL(triggered()),  dbDialog,      SLOT(exec()));
  connect(actionLogs,         SIGNAL(triggered()),  logDial,       SLOT(exec()));
  connect(actionNewQuery,     SIGNAL(triggered()),  this,          SLOT(newQuery()));
  connect(actionNextTab,      SIGNAL(triggered()),  this,          SLOT(nextTab()));
  connect(actionOpenQuery,    SIGNAL(triggered()),  this,          SLOT(openQuery()));
  connect(actionPaste,        SIGNAL(triggered()),  this,          SLOT(paste()));
  connect(actionPlugins,      SIGNAL(triggered()),  pluginDialog,  SLOT(exec()));
  connect(actionPreferences,  SIGNAL(triggered()),  confDial,      SLOT(exec()));
  connect(actionPreviousTab,  SIGNAL(triggered()),  this,          SLOT(previousTab()));
  connect(actionPrint,        SIGNAL(triggered()),  this,          SLOT(print()));
  connect(actionRedo,         SIGNAL(triggered()),  this,          SLOT(redo()));
  connect(actionSaveQuery,    SIGNAL(triggered()),  this,          SLOT(saveQuery()));
  connect(actionSaveQueryAs,  SIGNAL(triggered()),  this,          SLOT(saveQueryAs()));
  connect(actionSearch,       SIGNAL(triggered()),  this,          SLOT(search()));
  connect(actionSelect_all,   SIGNAL(triggered()),  this,          SLOT(selectAll()));
  connect(actionUndo,         SIGNAL(triggered()),  this,          SLOT(undo()));

  /*
   * DbTreeView
   */
  connect(dbTreeView, SIGNAL(tableSelected(QSqlDatabase*,QString)),
          this, SLOT(openTable(QSqlDatabase*,QString)));

  /*
   * DbManager
   */
  connect(DbManager::instance(), SIGNAL(statusChanged(QSqlDatabase*)),
          this, SLOT(checkDb(QSqlDatabase*)));
  connect(DbManager::model(), SIGNAL(dataChanged(QModelIndex,QModelIndex)),
          this, SLOT(reloadDbList()));

  /*
   * Dialogs
   */
  connect(dbWizard, SIGNAL(accepted()), this, SLOT(reloadDbList()));
  connect(logDial,  SIGNAL(event(QString)),
          QMainWindow::statusBar(), SLOT(showMessage(QString)));

  connect(QueryScheduler::instance(), SIGNAL(countChanged(int)),
          this, SLOT(setQueryCount(int)));

  /*
   * Tab widget
   */
  connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(refreshTab()));
  connect(tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
}

void MainWindow::setupWidgets()
{
  aboutDial     = new AboutDialog(this);
  confDial      = new ConfigDialog(this);
  logDial       = LogDialog::instance();
  searchDialog  = new SearchDialog(this);
  //printDialog = new QPrintDialog(this);

  QAction *a;
  recentActions.clear();
  for(int i=0; i<10; i++)
  {
    a = new QAction(this);
    a->setVisible(false);
    connect(a, SIGNAL(triggered()), this, SLOT(openRecent()));
    recentActions << a;
  }
  menuRecent_files->addActions(recentActions);
  menuRecent_files->addSeparator();
  menuRecent_files->addAction(actionClearRecent);

  actionPlugins->setVisible(false);

  actionMap[AbstractTabWidget::Copy]        = actionCopy;
  actionMap[AbstractTabWidget::Cut]         = actionCut;
  actionMap[AbstractTabWidget::Paste]       = actionPaste;
  actionMap[AbstractTabWidget::Print]       = actionPrint;
  actionMap[AbstractTabWidget::Redo]        = actionRedo;
  actionMap[AbstractTabWidget::Save]        = actionSaveQuery;
  actionMap[AbstractTabWidget::SaveAs]      = actionSaveQueryAs;
  actionMap[AbstractTabWidget::Search]      = actionSearch;
  actionMap[AbstractTabWidget::SelectAll]   = actionSelect_all;
  actionMap[AbstractTabWidget::Undo]        = actionUndo;

  QSettings s;
  s.beginGroup("mainwindow");
  if(s.contains("size"))
    resize(s.value("size").toSize());
  if(s.contains("position"))
    move(s.value("position").toPoint());
  if(s.value("maximized", false).toBool())
    setWindowState(Qt::WindowMaximized);

//  removeDockWidget(dockWidget);
//  addDockWidget(s.value("dock_area", Qt::LeftDockWidgetArea).toInt(),
//                dockWidget);

  queriesStatusLabel = new QLabel("", this);
  QMainWindow::statusBar()->addPermanentWidget(queriesStatusLabel);

  tooltipButton->setChecked(s.value("tooltips", true).toBool());
  tooltipFrame->setVisible(s.value("tooltips", true).toBool());
  s.endGroup();

  int count = s.beginReadArray("recentfiles");
  recentFiles.clear();
  for(int i=0; i<count && i<10; i++)
  {
    s.setArrayIndex(i);
    recentFiles << s.value("entry").toString();
  }
  s.endArray();
  refreshRecent();

#if defined(Q_WS_X11)
  QString lang = QLocale::system().name().left(2).toLower();
  QString url = QString(QString(PREFIX) + "/share/dbmaster/index_%1.html")
                  .arg(lang);

  if(!QFile::exists(url))
    url = QString(QString(PREFIX) + "/share/dbmaster/index_en.html");

  textBrowser->setSource(url);
#endif

#if defined(Q_WS_WIN)
  QString lang = QLocale::system().name().left(2).toLower();
  QString url = QString("share/index_%1.html").arg(lang);

  if(!QFile::exists(url))
    url = QString(QString(PREFIX) + "/share/index_en.html");

  textBrowser->setSource(url);
#endif

#if QT_VERSION >= 0x040500
  // ensures compatibility with Qt 4.4
  tabWidget->setMovable(true);
  tabWidget->setTabsClosable(true);
  actionCloseTab->setVisible(false);
#endif

  // loading icons from current theme
  actionAbout->setIcon(         IconManager::get("help-about"));
  actionAddDb->setIcon(         IconManager::get("db_add"));
  actionClearRecent->setIcon(   IconManager::get("edit-clear"));
  actionCloseTab->setIcon(      IconManager::get("window-close"));
  actionCopy->setIcon(          IconManager::get("edit-copy"));
  actionCut->setIcon(           IconManager::get("edit-cut"));
  actionExit->setIcon(          IconManager::get("application-exit"));
  actionNewQuery->setIcon(      IconManager::get("document-new"));
  actionOpenQuery->setIcon(     IconManager::get("document-open"));
  actionPaste->setIcon(         IconManager::get("edit-paste"));
  actionPreferences->setIcon(   IconManager::get("preferences"));
  actionPrint->setIcon(         IconManager::get("document-print"));
  actionRedo->setIcon(          IconManager::get("edit-redo"));
  actionSaveQuery->setIcon(     IconManager::get("document-save"));
  actionSaveQueryAs->setIcon(   IconManager::get("document-save-as"));
  actionSearch->setIcon(        IconManager::get("edit-find"));
  actionUndo->setIcon(          IconManager::get("edit-undo"));

  tooltipButton->setIcon(       IconManager::get("help-faq"));
  tabWidget->setTabIcon(0,      IconManager::get("go-home"));
}

void MainWindow::undo()
{
  if(currentTab())
    currentTab()->undo();
}
