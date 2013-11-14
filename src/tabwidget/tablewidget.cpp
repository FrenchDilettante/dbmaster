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
      .arg(m_db->connectionName());
}

void TableWidget::refresh()
{
  if(!m_db->isOpen())
    emit closeRequested();
}

void TableWidget::refreshStructure() {
  columnsTree->clear();
  SqlTable table = DbManager::instance->table(m_db, m_table);
  foreach (SqlColumn c, table.columns) {
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

void TableWidget::reload() {
  if(!m_db->isOpen())
    return;

  /*
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
  */

  tableView->setTable(m_table, m_db);
}

void TableWidget::setTable( QString table, QSqlDatabase *db )
{
  this->m_table = table;
  this->m_db = db;

//  reload();
}

void TableWidget::setupWidgets() {
  columnsTree->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
  columnsTree->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
  columnsTree->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
  columnsTree->header()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
  columnsTree->header()->setSectionResizeMode(4, QHeaderView::Stretch);

  connect(tableView, SIGNAL(reloadRequested()), this, SLOT(reload()));
  connect(tableView, SIGNAL(structureChanged()), this, SLOT(refreshStructure()));
}

QString TableWidget::table()
{
  return m_table;
}
