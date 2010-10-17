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


#include "dbtreeview.h"

#include "../dbmanager.h"
#include "../iconmanager.h"
#include "../mainwindow.h"

DbTreeView::DbTreeView(QWidget *parent)
  : QTreeView(parent)
{
  model = DbManager::model();
  connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
          this, SLOT(on_model_dataChanged(QModelIndex,QModelIndex)));
  setModel(model);

  setupActions();
}

void DbTreeView::addDatabase()
{
  MainWindow::dbWizard->exec();
}

void DbTreeView::contextMenuEvent(QContextMenuEvent *event)
{
  addDbAct->setVisible(false);
  editDbAct->setVisible(false);
  removeDbAct->setVisible(false);
  toggleAct->setVisible(false);

  if(selectedIndexes().size() == 1)
  {
    QModelIndex index = selectedIndexes()[0];
    if(index.data(Qt::UserRole).canConvert(QVariant::Int))
    {
      switch(index.data(Qt::UserRole).toInt())
      {
      case DbManager::DbItem:
        editDbAct->setVisible(true);
        removeDbAct->setVisible(true);
        toggleAct->setVisible(true);
        if(DbManager::getDatabase(index.row())->isOpen())
        {
          toggleAct->setText(tr("Disconnect"));
          toggleAct->setIcon(QIcon(":/img/connect_no.png"));
        } else {
          toggleAct->setText(tr("Connect"));
          toggleAct->setIcon(QIcon(":/img/connect_creating.png"));
        }
        break;

      case DbManager::FieldItem:
        break;

      case DbManager::SysTableItem:
      case DbManager::TableItem:
      case DbManager::ViewItem:
        /// @todo table action
        break;

      case DbManager::DisplayItem:
      default:
        break;
      }
    }
  } else {
    addDbAct->setVisible(true);
  }

  bool allHidden = false;
  foreach(QAction *a, contextMenu->actions())
    allHidden |= a->isVisible();

  if(allHidden)
  {
    contextMenu->move(event->globalPos());
    contextMenu->show();
  }

  event->accept();
}

void DbTreeView::editCurrent()
{
  if(selectedIndexes().size() == 1)
  {
    QModelIndex index = selectedIndexes()[0];
    MainWindow::dbDialog->setDatabase(index);
    MainWindow::dbDialog->exec();
  }
}

void DbTreeView::mouseDoubleClickEvent(QMouseEvent *event)
{
  if(selectedIndexes().size() == 1)
  {
    QModelIndex index = selectedIndexes()[0];
    if(!visualRect(index).contains(event->pos()))
    {
      event->accept();
      return;
    }

    if(index.data(Qt::UserRole).canConvert(QVariant::Int))
    {
      QString _db, _tb;
      switch(index.data(Qt::UserRole).toInt())
      {
      case DbManager::DbItem:
        toggleCurrentDb();
        // DbManager::toggle(DbManager::getDatabase(index.row()));
        break;

      case DbManager::FieldItem:
        break;

      case DbManager::SysTableItem:
      case DbManager::TableItem:
      case DbManager::ViewItem:
        _db = parentDb(index)->connectionName();
        _tb = index.data(Qt::DisplayRole).toString();
        emit tableSelected(parentDb(index),
                           index.data(Qt::DisplayRole).toString());
        break;

      case DbManager::DisplayItem:
      default:
        break;
      }
    }
  }
}

/**
 * Prise en charge de la modification du modèle
 */
void DbTreeView::on_model_dataChanged(const QModelIndex &topLeft,
                                      const QModelIndex &bottomRight) {
  QSqlDatabase *_db = parentDb(topLeft);
  if (_db && _db->isOpen()) {
    expand(topLeft);
  }
}

/**
 * Suppression de la connexion sélectionnée. Ne fait rien si elle est ouverte.
 */
void DbTreeView::on_removeDbAct_triggered() {
  if(selectedIndexes().size() == 1) {
    QModelIndex index = selectedIndexes()[0];
    if (DbManager::getDatabase(index.row())->isOpen()) {
      QMessageBox::warning(this,
                           tr("Cannot remove"),
                           tr("You must close the database before remove it."));
      return;
    }

    DbManager::removeDatabase(index.row());
  }
}

QSqlDatabase *DbTreeView::parentDb(QModelIndex index)
{
  while(index != QModelIndex())
  {
    if(index.data(Qt::UserRole) == DbManager::DbItem)
      return DbManager::getDatabase(index.row());
    index = index.parent();
  }

  return NULL;
}

void DbTreeView::setupActions()
{
  addDbAct = new QAction(this);
  addDbAct->setText(tr("New connection"));
  addDbAct->setIcon(IconManager::get("list-add"));
  connect(addDbAct, SIGNAL(triggered()), MainWindow::dbWizard, SLOT(exec()));

  editDbAct = new QAction(this);
  editDbAct->setText(tr("Edit"));
  connect(editDbAct, SIGNAL(triggered()), this, SLOT(editCurrent()));

  removeDbAct = new QAction(this);
  removeDbAct->setText(tr("Remove"));
  removeDbAct->setIcon(IconManager::get("list-remove"));
  connect(removeDbAct, SIGNAL(triggered()),
          this, SLOT(on_removeDbAct_triggered()));

  toggleAct = new QAction(this);
  toggleAct->setText(tr("Connect"));
  toggleAct->setIcon(QIcon(":/img/connect_creating.png"));
  connect(toggleAct, SIGNAL(triggered()), this, SLOT(toggleCurrentDb()));

  contextMenu = new QMenu(this);
  contextMenu->addAction(toggleAct);
  contextMenu->addAction(addDbAct);
  contextMenu->addAction(editDbAct);
  contextMenu->addAction(removeDbAct);
}

void DbTreeView::toggleCurrentDb()
{
  if(selectedIndexes().size() == 1)
  {
    QModelIndex index = selectedIndexes()[0];
    DbManager::toggle(DbManager::getDatabase(index.row()));
  }
}
