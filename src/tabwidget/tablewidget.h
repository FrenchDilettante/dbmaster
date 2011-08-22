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


#ifndef TABLEWIDGET_H
#define TABLEWIDGET_H

#include "../plugins/sqlwrapper.h"

#include <QSqlDatabase>
#include <QSqlError>

#include "abstracttabwidget.h"
#include "ui_tablewidgetclass.h"

class TableWidget: public AbstractTabWidget, Ui::TableWidgetClass
{
Q_OBJECT

public:
  TableWidget(QWidget* =0);
  TableWidget(QString table, int idx, QWidget *parent =0);

  QIcon     icon();
  QString   id();
  void      refresh();
  void      reload();
  void      setTable(int idx, QString table);
  QString   table();

signals:
  void error(QSqlError);
  void ready();

protected:
  void      closeEvent(QCloseEvent *event);
  void      run();
  void      setupWidgets();

  int       dbIdx;
  QSqlDatabase db;
  QString   m_table;
  SqlTable  tableInfo;
  QSqlTableModel *model;

private slots:
  void showError(QSqlError err);
  void validate();
};

#endif // TABLEWIDGET_H
