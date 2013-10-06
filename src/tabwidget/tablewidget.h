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
