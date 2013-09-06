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


#ifndef DBDIALOG_H
#define DBDIALOG_H

#include "ui_dbdialog.h"

#include "../wizards/newdbwizard.h"

#include <QDialog>
#include <QModelIndex>
#include <QSqlDatabase>
#include <QWidget>

class DbDialog: public QDialog, private Ui::DbDialog {
Q_OBJECT
public:
  DbDialog(QWidget* =0);

public slots:
  void setDatabase(QModelIndex index);

signals:

private:
  void setupConnections();
  void setupWidgets();

  NewDbWizard *addWzd;
  QSqlDatabase *db;
  bool aliasOnCurrent;

private slots:
  void accept();
  void apply();
  void refresh(QModelIndex index);
  void reload();
  void removeCurrent();
  void testConnection();
  void toggleConnection();
  void updateAlias();
};

#endif // DBDIALOG_H
