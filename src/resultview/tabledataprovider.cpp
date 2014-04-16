#include "tabledataprovider.h"

#include "tools/logger.h"

TableDataProvider::TableDataProvider(QString table, QSqlDatabase *db, QObject *parent) {
  this->table = table;
  this->db = db;

  m_model = new QSqlTableModel(this, *db);
  m_model->setEditStrategy(QSqlTableModel::OnManualSubmit);

  setParent(parent);
}

QSqlError TableDataProvider::lastError() {
  return m_model->lastError();
}

void TableDataProvider::run() {
  m_model->setTable(table);
  m_model->setFilter(filter);
  if (m_model->select()) {
    emit complete();
  } else {
    Logger::instance->logError(lastError().text());
    emit error();
  }
}

void TableDataProvider::setFilter(QString filter) {
  this->filter = filter;
  start();
}
