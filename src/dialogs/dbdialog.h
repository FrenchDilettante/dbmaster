#ifndef DBDIALOG_H
#define DBDIALOG_H

#include "ui_dbdialog.h"

#include "db/connection.h"
#include "wizards/newdbwizard.h"

#include <QDialog>
#include <QModelIndex>
#include <QSqlDatabase>
#include <QWidget>

class DbDialog: public QDialog, private Ui::DbDialog {
Q_OBJECT
public:
  DbDialog(QWidget* =0);

public slots:
  void setConnection(QModelIndex index);

signals:

private:
  void setupConnections();
  void setupWidgets();

  NewDbWizard *addWzd;
  Connection* connection;
  bool aliasOnCurrent;

private slots:
  void accept();
  void apply();
  void refresh();
  void reload();
  void removeCurrent();
  void testConnection();
  void toggleConnection();
  void updateAlias();
};

#endif // DBDIALOG_H
