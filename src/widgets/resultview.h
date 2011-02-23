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


#ifndef RESULTVIEW_H
#define RESULTVIEW_H

#include "../query/querytoken.h"
#include "../wizards/exportwizard.h"
#include "ui_resultview.h"

#include <QAction>
#include <QContextMenuEvent>
#include <QSqlRecord>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QWidget>

/**
 * Fournit un paliatif à QTableView en ajoutant une pagination, un menu
 * contextuel pour l'export et l'ajout/suppression de lignes dans les tables.
 * Prend en charge les jetons de requête et les tables.
 */
class ResultView : public QWidget, Ui::ResultView
{
Q_OBJECT
public:
  enum Mode {
    QueryMode, TableMode
  };

  enum Action {
    Browse, Update, Insert
  };

  ResultView(QWidget *parent=0);

  void setTable(QString table, QSqlDatabase *db);
  void setToken(QueryToken *token);

  Mode mode()           { return m_mode;    };
  void setMode(Mode m);

  Q_PROPERTY(Mode mode READ mode WRITE setMode)

signals:
  void reloadRequested();

public slots:
  void resizeColumnsToContents();
  void resizeRowsToContents();
  void setAlternatingRowColors(bool enable);
  void scrollBegin();
  void scrollDown();
  void scrollEnd();
  void scrollUp();
  void sort(int col);

private:
  void contextMenuEvent(QContextMenuEvent *e);
  void setModel(QSqlQueryModel *model);
  void setupConnections();
  void setupMenus();

  QAction              *actionAlternateColor;
  QAction              *actionExport;
  QMenu                *contextMenu;
  Action                currentAction;
  QPair<int, Qt::SortOrder> currentSorting;
  ExportWizard         *exportWizard;
  QMap<int, QSqlRecord> modifiedRecords;
  int                   lastEditedRow;
  QStandardItemModel   *shortModel;
  Mode                  m_mode;
  QSqlQueryModel       *model;
  QueryToken           *m_token;

  int                   offset;

private slots:
  void on_deleteButton_clicked();
  void exportContent();
  void on_reloadButton_clicked();
  void on_insertButton_clicked();
  void updateItem(QStandardItem *item);
  void updateView();
};

#endif // RESULTVIEW_H
