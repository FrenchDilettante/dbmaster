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

#include "../query/querytoken.h"

#include "abstracttabwidget.h"

#include "ui_queryeditorwidgetclass.h"

class QueryEditorWidget: public AbstractTabWidget, Ui::QueryEditorWidgetClass
{
Q_OBJECT
public:
  QueryEditorWidget( QWidget* = 0 );
  ~QueryEditorWidget();

  Actions       availableActions();
  int           confirmCloseAll();
  QString       file();
  QIcon         icon();
  QString       id();
  bool          isSaved();
  QueryToken   *prepareToken();
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
  void run();
  bool save();
  void selectAll();
  void undo();
  void upperCase();

signals:
  void fileChanged(QString);

private:
  void closeEvent(QCloseEvent *event);
  bool confirmClose();
  void reloadFile();
  void setFilePath(QString);
  void setupConnections();
  void setupWidgets();

  Actions               baseActions;
  QString               filePath;
  QSqlQueryModel       *model;
  int                   oldCount;
//  QueryToken           *oldToken;
  QMenu                *optionsMenu;
  int                   page;
  QSqlQuery             query;
  QStandardItemModel   *shortModel;
  QStatusBar           *statusBar;
  QLabel               *statusLabel;
  QueryToken           *token;

private slots:
  void acceptToken();
  void checkDbOpen();
  void forwardChanged(bool changed);
  void rejectToken();
  void startToken();
  void validateToken(QSqlError err);
};

#endif // QUERYEDITORWIDGET_H
