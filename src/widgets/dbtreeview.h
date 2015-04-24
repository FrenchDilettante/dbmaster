#ifndef DBTREEVIEW_H
#define DBTREEVIEW_H

#include "db/connection.h"
#include "dialogs/dbdialog.h"
#include "wizards/newdbwizard.h"

#include <QtWidgets/QTreeView>

class DbTreeView : public QTreeView {
Q_OBJECT
public:
  DbTreeView(QWidget *parent = 0);

  Connection* currentConnection();
  QSqlDatabase *currentDb();
  bool isDbSelected();

signals:
  void itemSelected();
  void schemaSelected(QSqlDatabase*, QString);
  void tableSelected(QSqlDatabase*, QString);

public slots:
  void connectCurrent();
  void disconnectCurrent();
  void editCurrent();
  void refreshCurrent();

private:
  void contextMenuEvent(QContextMenuEvent *event);
  void mouseDoubleClickEvent(QMouseEvent *event);
  void mousePressEvent(QMouseEvent *event);
  Connection* parentConnection(QModelIndex index);
  void selectionChanged(const QItemSelection &selected,
                        const QItemSelection &deselected);
  void setupActions();

  QMenu *contextMenu;
  QAction *addDbAct;
  QAction *editDbAct;
  QAction *openTableAct;
  QAbstractItemModel *model;
  QAction *refreshAct;
  QAction *removeDbAct;
  QAction *toggleAct;

private slots:
  void addDatabase();
  void onItemExpanded(const QModelIndex &index);
  void on_model_dataChanged(const QModelIndex & topLeft,
                            const QModelIndex & bottomRight);
  void removeCurrent();
  void toggleCurrentDb();
};

#endif // DBTREEVIEW_H
