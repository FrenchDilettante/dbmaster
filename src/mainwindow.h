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


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "dialogs/aboutdialog.h"
#include "dialogs/configdialog.h"
#include "dialogs/dbdialog.h"
#include "dialogs/logdialog.h"
#include "dialogs/searchdialog.h"
#include "ui_mainwindow.h"
#include "plugins/plugindialog.h"
#include "tabwidget/queryeditorwidget.h"
#include "wizards/newdbwizard.h"

#include <QSqlDatabase>
#include <QMainWindow>
#include <QWidget>

class MainWindow: public QMainWindow, public Ui::MainWindowClass
{
Q_OBJECT

public:
  MainWindow(QWidget *parent =0);
  ~MainWindow();

  static DbDialog      *dbDialog;
  static NewDbWizard   *dbWizard;
  static PluginDialog  *pluginDialog;

public slots:
  void addRecentFile(QString file);
  void createDatabase();
  QueryEditorWidget*  newQuery();
  void openQuery();
  void openTable(QSqlDatabase *db, QString table);
  void refreshTab();
  void refreshRecent();
  void reloadDbList();
  void saveQuery();
  void saveQueryAs();
  void toggleLeftPanel();

private:
  void                closeEvent(QCloseEvent*);
  AbstractTabWidget*  currentTab();
  void                setupConnections();
  void                setupWidgets();

  AboutDialog        *aboutDial;
  QMap<AbstractTabWidget::Action, QAction*> actionMap;
  ConfigDialog       *confDial;
  LogDialog          *logDial;
  QString             lastPath;
  SearchDialog       *searchDialog;
  QLabel             *queriesStatusLabel;
  //QPrintDialog       *printDialog;
  QList<QAction*>     recentActions;
  QStringList         recentFiles;

private slots:
  void checkDb(QSqlDatabase *db);
  void clearRecent();
  void closeCurrentTab();
  void closeTab(int nb);
  void copy();
  void cut();
  void nextTab();
  void openRecent();
  void paste();
  void previousTab();
  void print();
  void redo();
  void search();
  void selectAll();
  void setQueryCount(int count);
  void undo();
  void userManual();
};

#endif // MAINWINDOW_H
