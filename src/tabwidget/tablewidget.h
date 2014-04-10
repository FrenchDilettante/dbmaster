#ifndef TABLEWIDGET_H
#define TABLEWIDGET_H

#include "abstracttabwidget.h"
#include "resultview/tabledataprovider.h"
#include "ui_tablewidget.h"

#include <QStandardItemModel>

class TableWidget: public AbstractTabWidget, Ui::TableWidget {
Q_OBJECT

public:
  TableWidget(QWidget* =0);
  TableWidget(QString table, QSqlDatabase *db, QWidget *parent =0);

  QIcon icon();
  QString id();
  void refresh();
  void reload();
  void setTable( QString, QSqlDatabase* );
  QString table();

private:
  void setupWidgets();

  QSqlDatabase *m_db;
  TableDataProvider* dataProvider;
  QString m_table;
  QSqlTableModel *model;

private slots:
  void refreshStructure();
};

#endif // TABLEWIDGET_H
