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


#ifndef DBTREEVIEW_H
#define DBTREEVIEW_H

#include "../dialogs/dbdialog.h"
#include "../wizards/newdbwizard.h"

#include <QtGui>
#include <QtSql>

class DbTreeView : public QTreeView
{
Q_OBJECT
public:
  DbTreeView(QWidget *parent = 0);

signals:
  void tableSelected(QSqlDatabase*, QString);

public slots:

private:
  void contextMenuEvent(QContextMenuEvent *event);
  void mouseDoubleClickEvent(QMouseEvent *event);
  void mousePressEvent(QMouseEvent *event);
  QSqlDatabase *parentDb(QModelIndex index);
  void setupActions();

  QMenu *contextMenu;
  QAction *addDbAct;
  QAction *editDbAct;
  QAction *openTableAct;
  QAbstractItemModel *model;
  QAction *removeDbAct;
  QAction *toggleAct;

private slots:
  void addDatabase();
  void on_model_dataChanged(const QModelIndex & topLeft,
                            const QModelIndex & bottomRight);
  void editCurrent();
  void on_removeDbAct_triggered();
  void toggleCurrentDb();
};

#endif // DBTREEVIEW_H
