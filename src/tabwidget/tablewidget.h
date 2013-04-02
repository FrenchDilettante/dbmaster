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

#include <QtGui>
#include <QtSql>

#include "abstracttabwidget.h"
#include "ui_tablewidget.h"

class TableWidget: public AbstractTabWidget, Ui::TableWidget {
Q_OBJECT

public:
  TableWidget(QWidget* =0);
  TableWidget(QString table, QSqlDatabase *db, QWidget *parent =0);

  QIcon     icon();
  QString   id();
  void      refresh();
  void      reload();
  void      setTable( QString, QSqlDatabase* );
  QString   table();

protected:
  void      setupWidgets();

  QSqlDatabase *m_db;
  QString m_table;
  QSqlTableModel *model;
};

#endif // TABLEWIDGET_H
