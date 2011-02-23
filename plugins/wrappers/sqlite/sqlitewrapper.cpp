#include "sqlitewrapper.h"

#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

SqliteWrapper::SqliteWrapper(QObject *parent)
  : QObject(parent) {
}

SqliteWrapper::SqliteWrapper(QSqlDatabase *db)
  : QObject(NULL) {
  m_db = db;
}

SqlWrapper::WrapperFeatures SqliteWrapper::features() {
  return BasicFeatures;
}

SqlWrapper* SqliteWrapper::newInstance(QSqlDatabase *db) {
  return new SqliteWrapper(db);
}

QList<SqlTable> SqliteWrapper::tables() {
  QList<SqlTable> tables;

  if (!m_db) {
    return tables;
  }

  QString sql;

  sql += "SELECT name FROM sqlite_master ";
  sql += "WHERE type in ('table', 'view') ";
  sql += "ORDER BY name ";

  QSqlQuery query(*m_db);
  if (!query.exec(sql)) {
    qDebug() << query.lastError().text();
    return tables;
  }

  SqlTable t;
  while (query.next()) {
    t = SqlTable();
    t.name = query.value(0).toString();
    tables << t;
  }

  return tables;
}

Q_EXPORT_PLUGIN2(dbm_sqlite_wrapper, SqliteWrapper)
