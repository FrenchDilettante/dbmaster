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


#ifndef QUERYEDITORWIDGET_H
#define QUERYEDITORWIDGET_H

#include <QtGui>
#include <QtSql>

#include "abstracttabwidget.h"

#include "ui_queryeditorwidgetclass.h"

class QueryEditorWidget: public AbstractTabWidget, Ui::QueryEditorWidgetClass {
Q_OBJECT
public:
  QueryEditorWidget( QWidget* = 0 );

  Actions       availableActions();
  int           confirmCloseAll();
  QString       file();
  QIcon         icon();
  QString       id();
  bool          isSaved();
  void          print();
  QPrinter     *printer();
  void          saveAs( QString = QString::null );
  QTextEdit    *textEdit();
  QString       title();

public slots:
  void copy();
  void cut();
  void lowerCase();
  void open( QString );
  void paste();
  void redo();
  void refresh();
  void reload();
  void runQuery();
  bool save();
  void selectAll();
  void showConsole(bool show);
  void showResult(bool show);
  void undo();
  void upperCase();

signals:
  void fileChanged(QString);
  void ready();

private:
  void closeEvent(QCloseEvent *event);
  bool confirmClose();
  void keyPressEvent(QKeyEvent *event);
  void reloadFile();
  void run();
  void setFilePath(QString);
  void setupConnections();
  void setupWidgets();
  void showEvent(QShowEvent *event);

  Actions               baseActions;
  QSqlDatabase          db;
  QString               filePath;
  QSqlQueryModel       *model;
  int                   oldCount;
  QMenu                *optionsMenu;
  int                   page;
  QToolButton*          consoleButton;
  QToolButton*          resultButton;
  QSqlQuery             query;
  QStandardItemModel   *shortModel;
  QStatusBar           *statusBar;
  QFileSystemWatcher   *watcher;

private slots:
  void checkDbOpen();
  void onFileChanged(QString path);
  void validate();
};

#endif // QUERYEDITORWIDGET_H
