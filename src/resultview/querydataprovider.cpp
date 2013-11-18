#include "querydataprovider.h"

QueryDataProvider::QueryDataProvider(QObject *parent) {
  m_model = new QSqlQueryModel();

  setParent(parent);
}

void QueryDataProvider::run() {
  if (query.exec()) {
    emit complete();
  } else {
    emit error(query.lastError());
  }
}

void QueryDataProvider::setQuery(QString query, QSqlDatabase db) {
  this->query = QSqlQuery(query, db);
}
