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


#include "../dbmanager.h"
#include "../iconmanager.h"
#include "tablewidget.h"

TableWidget::TableWidget(QWidget *parent)
  : AbstractTabWidget(parent) {
  setupUi(this);

  setupWidgets();
}

TableWidget::TableWidget(QString table, int idx, QWidget *parent)
  : AbstractTabWidget(parent) {
  setupUi(this);
  setupWidgets();
  db = DbManager::db(idx);
  setTable(idx, table);
}

void TableWidget::closeEvent(QCloseEvent *event) {
  db.close();
  QSqlDatabase::removeDatabase(db.connectionName());
  event->accept();
}

QIcon TableWidget::icon()
{
  return IconManager::get("table");
}

QString TableWidget::id()
{
  return QString("t %1 on %2")
      .arg(m_table)
      .arg(db.connectionName().split("_")[0]);
}

void TableWidget::refresh()
{
  if(!db.isOpen())
    emit closeRequested();
}

void TableWidget::reload() {
  if(!db.isOpen())
    db.open();

  model = new QSqlTableModel(this, db);
  model->setTable(m_table);

  start();
}

void TableWidget::run() {
  if(!db.tables(QSql::Tables).contains(m_table)
    && !db.tables(QSql::Views).contains(m_table)
    && !db.tables(QSql::SystemTables).contains(m_table))   {
    QMessageBox::critical(this,
                          tr("Error"),
                          tr("Unable to open the table %1. ").append(
                            tr("Check you have the necessary permissions."))
                          .arg(m_table));
    emit closeRequested();
    return;
  }

  if (model->select()) {
    tableInfo = DbManager::table(dbIdx, m_table);
    emit ready();
  } else {
    emit error(model->lastError());
  }
}

void TableWidget::setTable(int idx, QString table) {
  if (this->db.isOpen()) {
    this->db.close();
  }

  this->m_table = table;
  this->dbIdx = idx;
  this->db = DbManager::db(idx);
}

void TableWidget::setupWidgets() {
  columnsTree->header()->setResizeMode(0, QHeaderView::ResizeToContents);
  columnsTree->header()->setResizeMode(1, QHeaderView::ResizeToContents);
  columnsTree->header()->setResizeMode(2, QHeaderView::ResizeToContents);
  columnsTree->header()->setResizeMode(3, QHeaderView::ResizeToContents);
  columnsTree->header()->setResizeMode(4, QHeaderView::Stretch);

  connect(tableView, SIGNAL(reloadRequested()), this, SLOT(reload()));

  connect(this, SIGNAL(ready()), this, SLOT(validate()));
}

void TableWidget::showError(QSqlError err) {
  QMessageBox::critical(this,
                        tr("Error"),
                        tr("Unable to open the table. Returned error :\n%1")
                          .arg(err.text()),
                        QMessageBox::Ok);
}

QString TableWidget::table() {
  return m_table;
}

void TableWidget::validate() {
  tableView->setTable(model);

  columnsTree->clear();
  foreach (SqlColumn c, tableInfo.columns) {
    QStringList cols;
    cols << c.name
         << c.type.name
         << ( c.permitsNull ? tr("Yes") : tr("No") )
         << c.defaultValue.toString()
         << c.comment;
    QTreeWidgetItem *it = new QTreeWidgetItem(cols);
    if (c.primaryKey) {
      it->setIcon(0, IconManager::get("column_key"));
    } else {
      it->setIcon(0, IconManager::get("column"));
    }
    columnsTree->addTopLevelItem(it);
  }
}
