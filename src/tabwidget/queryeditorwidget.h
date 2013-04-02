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
#include <QRunnable>
#include <QSqlError>
#include <QSqlResult>
#include <QSqlQuery>
#include <QSqlQueryModel>

#include "abstracttabwidget.h"

#include "ui_queryeditorwidget.h"

class QueryEditorWidget: public AbstractTabWidget, QRunnable, Ui::QueryEditorWidget {
Q_OBJECT
public:
  QueryEditorWidget(QWidget* = 0);

  Actions       availableActions();
  int           confirmCloseAll();
  QString       file();
  QIcon         icon();
  QString       id();
  bool          isSaved();
  void          print();
  QPrinter     *printer();
  void          saveAs(QString = QString::null);
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
  bool save();
  void selectAll();
  void undo();
  void upperCase();

signals:
  void fileChanged(QString);
  void queryFinished();

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
  QString               filePath;
  QSqlQueryModel       *model;
  int                   oldCount;
  int                   page;
  QToolButton*          resultButton;
  QSqlQuery             query;
  QStandardItemModel   *shortModel;
  QStatusBar           *statusBar;
  QFileSystemWatcher   *watcher;

private slots:
  void checkDbOpen();
  void onFileChanged(QString path);
  void start();
  void validateQuery();
};

#endif // QUERYEDITORWIDGET_H
