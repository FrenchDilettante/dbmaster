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
#include "plugins/pluginmanager.h"
#include "tabwidget/abstracttabwidget.h"
#include "tabwidget/queryeditorwidget.h"
#include "tabwidget/schemawidget.h"
#include "tabwidget/tablewidget.h"
#include "widgets/dbtreeview.h"

#include <QDesktopServices>

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

/**
 * Ajoute le nom de fichier passé en paramètre à la liste des fichiers récents.
 */
void MainWindow::addRecentFile(QString file)
{
  if(recentFiles.indexOf(file) >= 0)
    recentFiles.removeAt(recentFiles.indexOf(file));

  recentFiles.insert(0, file);
  while(recentFiles.size() > 10)
    recentFiles.removeLast();
  refreshRecent();
}

/**
 * Lorsqu'une connexion change de statut, on contrôle s'il y a des erreurs.
 */
void MainWindow::checkDb(QSqlDatabase *db)
{
  if(db->isOpenError() && db->lastError().type() != QSqlError::NoError)
    QMessageBox::critical(this,
                          tr("Error"),
                          tr("Unable to connect :\n%1")
                          .arg(db->lastError().text()));
}

/**
 * Efface l'historique des fichiers récents
 */
void MainWindow::clearRecent()
{
  recentFiles.clear();
  refreshRecent();
}

/**
 * Intercepte l'événement de fermeture pour contrôler que tous les onglets ont
 * été enregistrés. Montre une fenêtre de dialogue le cas échéant.
 */
void MainWindow::closeEvent(QCloseEvent *event) {
  /// nombre d'onglets à fermer
  int saved = 0;

  /// pour des raisons pratiques, stocke le dernier onglet non enregistré
  int n = 0;

  // vérifie s'il y a 0, 1 ou plus onglets non enregistrés
  for(int i = 0; i < tabWidget->count() && saved < 2; i++)
  {
    if(!((AbstractTabWidget*)(tabWidget->widget(i)))->isSaved())
    {
      n = i;
      saved++;
    }
  }

  if(saved == 0) {

    // tous les onglets sont prêts à être fermés
    event->accept();

  } else if(saved == 1) {

    // un seul onglet n'est pas enregistré
    if(((AbstractTabWidget*) tabWidget->widget(n))->confirmClose()) {
      event->accept();
    } else {
      event->ignore();
      return;
    }

  } else {

    // plus d'un onglet n'est pas enregistré

    // demande de confirmation de confirmation
    int ret = QMessageBox::warning(this,
             tr("Unsaved files"),
             tr("Some files are not saved. Are you sure to want to quit ?"),
             QMessageBox::SaveAll | QMessageBox::Discard | QMessageBox::Cancel,
             QMessageBox::SaveAll );

    switch(ret)
    {
    case QMessageBox::SaveAll:
      // on sauvegarde tout
      for(int i=0; i<tabWidget->count(); i++) {
        ((AbstractTabWidget*) tabWidget->widget(i))->save();
      }
      break;

    case QMessageBox::Discard:
      // on ignore : rien à faire, les onglets seront détruits à la fermeture
      break;

    default:
      // on annule
      event->ignore();
      return;
    }
  }

  // sauvegarde des préférences de la fenêtre
  QSettings s;
  s.beginGroup("mainwindow");
  s.setValue("position", pos());
  if(windowState().testFlag(Qt::WindowMaximized))
  {
    // fenêtre maximisée
    s.setValue("maximized", true);
    s.remove("size");
  } else {
    // fenêtre non maximisée : on stocke position + taille
    s.setValue("size", size());
    s.remove("maximized");
  }

  // tooltips activés ou non
  s.setValue("tooltips", tooltipButton->isChecked());

  // où se situe le dock de connexion
  s.setValue("maindock_visible", dockWidget->isVisible());
  s.setValue("maindock_area", dockWidgetArea(dockWidget));
  s.setValue("maindock_floating", dockWidget->isFloating());
  s.setValue("maindock_position", dockWidget->pos());
  s.setValue("maindock_size", dockWidget->size());

  // Positionnement de la toolbar principale
  s.setValue("maintoolbar_area", toolBarArea(mainToolBar));
  s.setValue("dbtoolbar_area", toolBarArea(dbToolBar));

  s.endGroup();

  // fichiers récents
  s.beginWriteArray("recentfiles", recentFiles.size());
  for(int i=0; i<recentFiles.size(); i++) {
    s.setArrayIndex(i);
    s.setValue("entry", recentFiles[i]);
  }
  s.endArray();

  // on prend le soin de fermer toutes les connexions
  DbManager::closeAll();

  // et d'enregistrer les plugins
  PluginManager::save();

  event->accept();
}

void MainWindow::closeCurrentTab() {
  closeTab(tabWidget->currentIndex());
}

void MainWindow::closeSender() {
  QWidget *w = dynamic_cast<QWidget*>(sender());
  if (w && tabWidget->indexOf(w) > -1) {
    tabWidget->removeTab(tabWidget->indexOf(w));
  }
}

void MainWindow::closeTab(int nb) {
  if(nb < 0 || nb >= tabWidget->count())
    return;

  // don't close the tab if the user cancelled saving
  if(!tabWidget->widget(nb)->close())
    return;

  // then remove the widget
  tabWidget->removeTab(nb);

  if (tabWidget->count() == 0) {
    foreach (QAction *a, actionMap.values()) {
      a->setEnabled(false);
    }
  }
}

void MainWindow::copy()
{
  if(currentTab() != 0)
    currentTab()->copy();
}

/**
 * Affiche l'assistant de nouvelle connexion
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

void MainWindow::lowerCase() {
  if (currentTab() != 0) {
    currentTab()->lowerCase();
  }
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

  return w;
}

void MainWindow::nextTab()
{
  int count = tabWidget->count();
  int index = tabWidget->currentIndex();

  if(count >= 2 && index < count)
    tabWidget->setCurrentIndex(index+1);
}

void MainWindow::on_actionUserManual_triggered() {
  QDesktopServices::openUrl(
        QUrl("http://dbmaster.sourceforge.net/userdoc/0.7/01-a-propos.html"));
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

  openQuery(f);
}

void MainWindow::openQuery(QString file) {
  QueryEditorWidget *w = newQuery();
  w->open(file);

  QFileInfo fi = QFileInfo(file);
  AbstractTabWidget::lastDir = fi.absoluteDir();
}

/**
 * Ouvre un schéma dans l'onglet associé
 */
void MainWindow::openSchema(QSqlDatabase *db, QString schema) {
  // ID unique pour retrouver facilement le schéma ouvert
  QString id = QString("s %1 on %2")
                .arg(schema)
                .arg(db->connectionName());

  // On vérifie si le schéma demandé n'est pas déjà ouvert
  int index = -1;
  for (int i=0; i<tabWidget->count(); i++) {
    if (((AbstractTabWidget*) tabWidget->widget(i))->id() == id) {
      index = i;
      break;
    }
  }

  if (index >= 0) {
    tabWidget->setCurrentIndex(index);
  } else {
    SchemaWidget *tab = new SchemaWidget(schema, db, this);
    index = tabWidget->addTab(tab, tab->icon(), schema);
    tabWidget->setCurrentIndex(index);
    connect(tab, SIGNAL(closeRequested()), this, SLOT(closeSender()));
    connect(tab, SIGNAL(tableRequested(QSqlDatabase*,QString)),
            this, SLOT(openTable(QSqlDatabase*,QString)));
    tab->reload();
  }
}

/**
 * Open a table browser tab
 *
 * @param db
 *          concerned database
 */
void MainWindow::openTable(QSqlDatabase *db, QString table) {
  // generates an unique ID for the table
  QString id = QString("t %1 on %2")
               .arg(table)
               .arg(db->connectionName());

  // First of all, the table may be already openned. We're gonna check it.
  int index = -1;
  for (int i=0; i<tabWidget->count(); i++) {
    if (((AbstractTabWidget*) tabWidget->widget(i))->id() == id) {
      index = i;
      break;
    }
  }

  /* If the index is >= 0, then a table browser is already openned with the same
     ID. */
  if (index >= 0) {
    tabWidget->setCurrentIndex(index);
  } else {
    TableWidget *view = new TableWidget(table, db, this);
    index = tabWidget->addTab(view, view->icon(), table);
    tabWidget->setCurrentIndex(index);
    connect(view, SIGNAL(closeRequested()), this, SLOT(closeSender()));
    view->reload();
  }
}

void MainWindow::paste() {
  if(currentTab())
    currentTab()->paste();
}

void MainWindow::previousTab() {
  int count = tabWidget->count();
  int index = tabWidget->currentIndex();

  if(count >= 2 && index > 0)
    tabWidget->setCurrentIndex(index - 1);
}

void MainWindow::print()
{
  if (currentTab() && currentTab()->availableActions() && AbstractTabWidget::Print)
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
  AbstractTabWidget *tab = dynamic_cast<AbstractTabWidget*>(sender());

  if(tab)
  {
    QString text = tab->title();
    if(!tab->isSaved())
      text.prepend("* ");
    tabWidget->setTabText(tabWidget->indexOf(tab), text);
  }

  // toolbar's actions
  if(currentTab())
  {
    AbstractTabWidget::Actions acts;
    acts = currentTab()->availableActions();
    foreach(AbstractTabWidget::Action a, actionMap.keys())
      actionMap[a]->setEnabled(acts.testFlag(a));
  }
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

void MainWindow::reloadDbList() {
  for(int i = 0; i < tabWidget->count(); i++)
    ((AbstractTabWidget*)tabWidget->widget(i))->refresh();

  updateDbActions();
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
    searchDialog->show();
  }
}

void MainWindow::selectAll()
{
  if(currentTab() != 0)
    currentTab()->selectAll();
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
  connect(actionConnect,      SIGNAL(triggered()),  dbTreeView,    SLOT(connectCurrent()));
  connect(actionCut,          SIGNAL(triggered()),  this,          SLOT(cut()));
  connect(actionDbManager,    SIGNAL(triggered()),  dbDialog,      SLOT(exec()));
  connect(actionDisconnect,   SIGNAL(triggered()),  dbTreeView,    SLOT(disconnectCurrent()));
  connect(actionEditConnection,SIGNAL(triggered()), dbTreeView,    SLOT(editCurrent()));
  connect(actionLeftPanel,    SIGNAL(triggered()),  this,          SLOT(toggleLeftPanel()));
  connect(actionLowerCase,    SIGNAL(triggered()),  this,          SLOT(lowerCase()));
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
  connect(actionRefreshConnection, SIGNAL(triggered()),dbTreeView, SLOT(refreshCurrent()));
  connect(actionRemoveConnection, SIGNAL(triggered()), dbTreeView, SLOT(removeCurrent()));
  connect(actionSaveQuery,    SIGNAL(triggered()),  this,          SLOT(saveQuery()));
  connect(actionSaveQueryAs,  SIGNAL(triggered()),  this,          SLOT(saveQueryAs()));
  connect(actionSearch,       SIGNAL(triggered()),  this,          SLOT(search()));
  connect(actionSelect_all,   SIGNAL(triggered()),  this,          SLOT(selectAll()));
  connect(actionUndo,         SIGNAL(triggered()),  this,          SLOT(undo()));
  connect(actionUpperCase,    SIGNAL(triggered()),  this,          SLOT(upperCase()));

  /*
   * DbTreeView
   */
  connect(dbTreeView, SIGNAL(schemaSelected(QSqlDatabase*,QString)),
          this, SLOT(openSchema(QSqlDatabase*,QString)));
  connect(dbTreeView, SIGNAL(itemSelected()), this, SLOT(updateDbActions()));
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

  actionMap[AbstractTabWidget::CaseLower]   = actionLowerCase;
  actionMap[AbstractTabWidget::CaseUpper]   = actionUpperCase;
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

  addDockWidget((Qt::DockWidgetArea) s.value("maindock_area", 1).toInt(),
                dockWidget);
  dockWidget->setFloating(s.value("maindock_floating", false).toBool());
  dockWidget->move(s.value("maindock_position").toPoint());

  if (s.contains("maindock_size")) {
    dockWidget->resize(s.value("maindock_size").toSize());
  }

  dockWidget->setVisible(s.value("maindock_visible", true).toBool());

  addToolBar((Qt::ToolBarArea) s.value("dbtoolbar_area", 4).toInt(), dbToolBar);
  addToolBar((Qt::ToolBarArea) s.value("maintoolbar_area", 4).toInt(),
             mainToolBar);

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
  QDir d = QDir("../../dbmaster/src/share/html");
  QString folder = d.exists() ? d.absolutePath() + "/" : QString(PREFIX) + "/share/dbmaster/";
  QString url = folder + QString("index_%1.html").arg(lang);

  if (!QFile::exists(url)) {
    url = folder + QString("index_en.html");
  }

  textBrowser->setSource(url);
#endif

#if defined(Q_WS_WIN)
  QString lang = QLocale::system().name().left(2).toLower();
  QString url = QString("share\\index_%1.html").arg(lang);

  if(!QFile::exists(url))
    url = QString("share\\index_en.html");

  textBrowser->setSource(url);
#endif

  // loading icons from current theme
  actionAbout->setIcon(         IconManager::get("help-about"));
  actionAddDb->setIcon(         IconManager::get("database_add"));
  actionClearRecent->setIcon(   IconManager::get("edit-clear"));
  actionCloseTab->setIcon(      IconManager::get("window-close"));
  actionConnect->setIcon(       IconManager::get("database_go"));
  actionCopy->setIcon(          IconManager::get("edit-copy"));
  actionCut->setIcon(           IconManager::get("edit-cut"));
  actionDisconnect->setIcon(    IconManager::get("database_connect"));
  actionEditConnection->setIcon(IconManager::get("database_edit"));
  actionExit->setIcon(          IconManager::get("application-exit"));
  actionNewQuery->setIcon(      IconManager::get("document-new"));
  actionOpenQuery->setIcon(     IconManager::get("document-open"));
  actionPaste->setIcon(         IconManager::get("edit-paste"));
  actionPlugins->setIcon(       IconManager::get("plugins"));
  actionPreferences->setIcon(   IconManager::get("preferences"));
  actionPrint->setIcon(         IconManager::get("document-print"));
  actionRedo->setIcon(          IconManager::get("edit-redo"));
  actionRefreshConnection->setIcon(IconManager::get("database_refresh"));
  actionRemoveConnection->setIcon(IconManager::get("database_delete"));
  actionSaveQuery->setIcon(     IconManager::get("document-save"));
  actionSearch->setIcon(        IconManager::get("edit-find"));
  actionUndo->setIcon(          IconManager::get("edit-undo"));

  tooltipButton->setIcon(       IconManager::get("help-faq"));
  tabWidget->setTabIcon(0,      IconManager::get("go-home"));
}

void MainWindow::toggleLeftPanel() {
  dockWidget->setVisible(!dockWidget->isVisible());
}

void MainWindow::undo() {
  if(currentTab())
    currentTab()->undo();
}

void MainWindow::updateDbActions() {
  bool select = dbTreeView->isDbSelected();
  QSqlDatabase *currentDb = dbTreeView->currentDb();
  bool dbOpen = currentDb && currentDb->isOpen();

  actionEditConnection->setEnabled(select);
  actionRefreshConnection->setEnabled(select);
  actionRemoveConnection->setEnabled(select && dbOpen);
  actionConnect->setEnabled(select && dbOpen);
  actionDisconnect->setEnabled(select && dbOpen);
}

void MainWindow::upperCase() {
  if (currentTab() != 0) {
    currentTab()->upperCase();
  }
}

/**
 * Ouvre la doc. utilisateur
 */
void MainWindow::userManual() {
  QString url = "";
#if defined(Q_WS_X11)
  QString lang = QLocale::system().name().left(2).toLower();
  url = QString("share/doc/%1/index.html").arg(lang);

  if(!QFile::exists(url))
    url = QString(QString(PREFIX) + "/share/en/index.html");
#endif
  QDesktopServices::openUrl(url);
}
