#ifndef TABLEINFODIALOG_H
#define TABLEINFODIALOG_H

#include <QtGui>
#include <QtSql>

#include "ui_tableinfodialog.h"

class TableInfoDialog : public QDialog, Ui::TableInfoDialog
{
Q_OBJECT
public:
  TableInfoDialog(int db, QString table, QWidget *parent = 0);

public slots:
  void refresh();
  void reject();
  void reload();

protected:
  QSqlDatabase *db;
  QSqlTableModel *model;
  QString table;
};

#endif // TABLEINFODIALOG_H
