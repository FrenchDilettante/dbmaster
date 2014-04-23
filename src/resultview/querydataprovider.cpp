#include "querydataprovider.h"

#include "tools/logger.h"

#include <QDebug>

QueryDataProvider::QueryDataProvider(QObject *parent) {
  m_model = new QSqlQueryModel();

  setParent(parent);
}

QSqlError QueryDataProvider::lastError() {
  return m_model->lastError();
}

void QueryDataProvider::run() {
  m_model->setQuery(query, db);
  qDebug() << query;

  if (m_model->lastError().type() == QSqlError::NoError) {
    emit success();
  } else {
    Logger::instance->logError(lastError().text());
    emit error();
  }
  emit complete();
}

void QueryDataProvider::setQuery(QString query, QSqlDatabase db) {
  this->db = db;
  this->query = query;
}
