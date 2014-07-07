#include "dbtreeview.h"

#include "../dbmanager.h"
#include "../mainwindow.h"

DbTreeView::DbTreeView(QWidget *parent)
  : QTreeView(parent)
{
  model = DbManager::instance->model();
  connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
          this, SLOT(on_model_dataChanged(QModelIndex,QModelIndex)));
  setModel(model);

  header()->setSectionResizeMode(0, QHeaderView::Stretch);

  connect(this, SIGNAL(expanded(QModelIndex)),
          this, SLOT(onItemExpanded(QModelIndex)));

  setupActions();
}

void DbTreeView::addDatabase()
{
  MainWindow::dbWizard->exec();
}

void DbTreeView::connectCurrent() {
  QSqlDatabase *db = currentDb();
  if (db && !db->isOpen()) {
    DbManager::instance->open(db);
  }
}

void DbTreeView::disconnectCurrent() {
  QSqlDatabase *db = currentDb();
  if (db && db->isOpen()) {
    DbManager::instance->close(db);
  }
}

void DbTreeView::contextMenuEvent(QContextMenuEvent *event)
{
  addDbAct->setVisible(false);
  editDbAct->setVisible(false);
  removeDbAct->setVisible(false);
  toggleAct->setVisible(false);
  refreshAct->setEnabled(false);

  if(selectedIndexes().size() == 1) {
    refreshAct->setEnabled(true);
    QModelIndex index = selectedIndexes()[0];
    if (index.data(Qt::UserRole).canConvert(QVariant::Int)) {
      switch(index.data(Qt::UserRole).toInt()) {
      case DbManager::DbItem:
        editDbAct->setVisible(true);
        removeDbAct->setVisible(true);
        toggleAct->setVisible(true);
        if (DbManager::instance->getDatabase(index.row())->isOpen()) {
          toggleAct->setText(tr("Disconnect"));
        } else {
          toggleAct->setText(tr("Connect"));
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

QSqlDatabase* DbTreeView::currentDb() {
  if (selectedIndexes().size() != 1) {
    return NULL;
  }

  return parentConnection(selectedIndexes()[0])->db();
}

void DbTreeView::editCurrent() {
  if(selectedIndexes().size() == 1) {
    QModelIndex index = selectedIndexes()[0];
    while (index.parent() != QModelIndex()) {
      index = index.parent();
    }
    MainWindow::dbDialog->setConnection(index);
    MainWindow::dbDialog->exec();
  }
}

bool DbTreeView::isDbSelected() {
  if (selectedIndexes().size() == 1) {
    return parentConnection(selectedIndexes()[0]);
  } else {
    return false;
  }
}

/**
 * Gestion du double-clic
 */
void DbTreeView::mouseDoubleClickEvent(QMouseEvent *event) {
  if (selectedIndexes().size() != 0) {
    QModelIndex index = selectedIndexes()[0];
    if (!visualRect(index).contains(event->pos())) {
      event->accept();
      return;
    }

    if (index.data(Qt::UserRole).canConvert(QVariant::Int)) {
      QString table;
      switch(index.data(Qt::UserRole).toInt()) {
      case DbManager::DbItem:
        toggleCurrentDb();
        break;

      case DbManager::FieldItem:
        break;

      case DbManager::SchemaItem:
        emit schemaSelected(parentConnection(index)->db(),
                            index.data(Qt::ToolTipRole).toString());
        break;

      case DbManager::SysTableItem:
      case DbManager::TableItem:
      case DbManager::ViewItem:
        table = index.data(Qt::ToolTipRole).toString();
        if (table.startsWith("public.")) {
          table = table.mid(7);
        }
        emit tableSelected(parentConnection(index)->db(), table);
        break;

      case DbManager::DisplayItem:
      default:
        break;
      }
    }
  }
}

void DbTreeView::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::RightButton) {
    QModelIndex item = indexAt(event->pos());
    selectionModel()->select(item, QItemSelectionModel::ClearAndSelect);
    QTreeView::mousePressEvent(event);
  } else {
    QTreeView::mousePressEvent(event);
  }
}

void DbTreeView::onItemExpanded(const QModelIndex &index) {
  if (index.data(Qt::UserRole).canConvert(QVariant::Int)) {
    DbManager::instance->refreshModelIndex(index);
  }
}

/**
 * Prise en charge de la modification du modèle
 */
void DbTreeView::on_model_dataChanged(const QModelIndex &topLeft,
                                      const QModelIndex &bottomRight) {
  Connection* connection = parentConnection(topLeft);
  if (connection && connection->db()->isOpen()) {
    expand(topLeft);
  }
}

/**
 * Suppression de la connexion sélectionnée. Ne fait rien si elle est ouverte.
 */
void DbTreeView::removeCurrent() {
  if(selectedIndexes().size() != 0) {
    QModelIndex index = selectedIndexes()[0];
    while (index.parent() != QModelIndex()) {
      index = index.parent();
    }
    if (DbManager::instance->getDatabase(index.row())->isOpen()) {
      QMessageBox::warning(this,
                           tr("Cannot remove"),
                           tr("You must close the database before remove it."));
      return;
    }

    DbManager::instance->removeDatabase(index.row());
  }
}

Connection* DbTreeView::parentConnection(QModelIndex index) {
  while (index != QModelIndex()) {
    if (index.data(Qt::UserRole) == DbManager::DbItem) {
      return DbManager::instance->connections()[index.row()];
    }
    index = index.parent();
  }

  return NULL;
}

void DbTreeView::refreshCurrent() {
  if (selectedIndexes().size() == 0) {
    return;
  }

  DbManager::instance->refreshModelItem(parentConnection(selectedIndexes()[0]));
}

void DbTreeView::selectionChanged(const QItemSelection &selected,
                                  const QItemSelection &deselected) {
  emit itemSelected();

  QTreeView::selectionChanged(selected, deselected);
}

void DbTreeView::setupActions()
{
  addDbAct = new QAction(this);
  addDbAct->setText(tr("New connection"));
  connect(addDbAct, SIGNAL(triggered()), MainWindow::dbWizard, SLOT(exec()));

  editDbAct = new QAction(this);
  editDbAct->setText(tr("Edit"));
  connect(editDbAct, SIGNAL(triggered()), this, SLOT(editCurrent()));

  refreshAct = new QAction(this);
  refreshAct->setText(tr("Refresh"));
  refreshAct->setShortcut(QKeySequence(Qt::ShiftModifier + Qt::Key_F5));
  connect(refreshAct, SIGNAL(triggered()), this, SLOT(refreshCurrent()));

  removeDbAct = new QAction(this);
  removeDbAct->setText(tr("Remove"));
  connect(removeDbAct, SIGNAL(triggered()), this, SLOT(removeCurrent()));

  toggleAct = new QAction(this);
  toggleAct->setText(tr("Connect"));
  connect(toggleAct, SIGNAL(triggered()), this, SLOT(toggleCurrentDb()));

  contextMenu = new QMenu(this);
  contextMenu->addAction(toggleAct);
  contextMenu->addAction(refreshAct);
  contextMenu->addAction(addDbAct);
  contextMenu->addAction(editDbAct);
  contextMenu->addAction(removeDbAct);
}

void DbTreeView::toggleCurrentDb() {
  if (selectedIndexes().size() == 1) {
    QModelIndex index = selectedIndexes()[0];
    DbManager::instance->toggle(DbManager::instance->getDatabase(index.row()));
  }
}
