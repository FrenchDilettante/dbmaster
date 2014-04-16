#include "querydataprovider.h"

#include "tools/logger.h"

QueryDataProvider::QueryDataProvider(QObject *parent) {
  m_model = new QSqlQueryModel();

  setParent(parent);
}

QSqlError QueryDataProvider::lastError() {
  return m_model->lastError();
}

void QueryDataProvider::run() {
  m_model->setQuery(query, db);
  QString logMsg;

  if (m_model->lastError().type() == QSqlError::NoError) {
    emit success();
  } else {
    emit error();

    logMsg.append(
          QString("<span style=\"color: red\">%1</span>")
            .arg(lastError().text()));

    logMsg.append(
          QString("<br /><span style=\"color: blue\">%1</span>")
            .arg(query.replace("\n", " ")));
    Logger::instance->log(logMsg);
  }
  emit complete();
}

void QueryDataProvider::setQuery(QString query, QSqlDatabase db) {
  this->db = db;
  this->query = query;
}