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
  : AbstractTabWidget(parent)
{
  setupUi(this);

  setupWidgets();

  model = new QSqlTableModel(this);
}

TableWidget::TableWidget(QString table, QSqlDatabase *db, QWidget *parent)
  : AbstractTabWidget(parent)
{
  setupUi(this);
  setupWidgets();
  setTable(table, db);
}

QIcon TableWidget::icon()
{
  return IconManager::get("table");
}

QString TableWidget::id()
{
  return QString("t %1 on %2")
      .arg(m_table)
      .arg(m_db->databaseName());
}

void TableWidget::refresh()
{
  if(!m_db->isOpen())
    close();
}

void TableWidget::reload() {
  if(!m_db->isOpen())
    return;

  if(!m_db->tables(QSql::Tables).contains(m_table)
    && !m_db->tables(QSql::Views).contains(m_table)
    && !m_db->tables(QSql::SystemTables).contains(m_table))   {
    QMessageBox::critical(this,
                          tr("Error"),
                          tr("Unable to open the table %1. ").append(
                            tr("Check you have the necessary permissions."))
                          .arg(m_table));
    emit closeRequested();
    return;
  }

  if (!tableView->setTable(m_table, m_db)) {
    emit closeRequested();
  }
}

void TableWidget::setTable( QString table, QSqlDatabase *db )
{
  this->m_table = table;
  this->m_db = db;

//  reload();
}

void TableWidget::setupWidgets()
{
  connect(tableView, SIGNAL(reloadRequested()), this, SLOT(reload()));
}

QString TableWidget::table()
{
  return m_table;
}
