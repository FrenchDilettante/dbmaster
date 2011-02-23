#include "mysqlwrapper.h"

#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

MysqlWrapper::MysqlWrapper(QObject *parent)
  : QObject(parent) {
}

MysqlWrapper::MysqlWrapper(QSqlDatabase *db)
  : QObject(NULL) {
  m_db = db;
}

SqlWrapper::WrapperFeatures MysqlWrapper::features() {
  return BasicFeatures;
}

SqlWrapper* MysqlWrapper::newInstance(QSqlDatabase *db) {
  return new MysqlWrapper(db);
}

QList<SqlTable> MysqlWrapper::tables() {
  QList<SqlTable> tables;

  if (!m_db) {
    return tables;
  }

  QString sql;

  sql += "SELECT C.TABLE_NAME, T.TABLE_TYPE, C.COLUMN_NAME, C.DATA_TYPE, ";
  sql += "C.IS_NULLABLE, 0 AS 'PK', C.ORDINAL_POSITION ";
  sql += "FROM INFORMATION_SCHEMA.COLUMNS C ";
  sql +=   "INNER JOIN INFORMATION_SCHEMA.TABLES T ";
  sql +=   "ON C.TABLE_NAME = T.TABLE_NAME ";
  sql += "WHERE C.TABLE_SCHEMA='";
  sql += m_db->databaseName();
  sql += "' ";

  sql += "ORDER BY TABLE_NAME, ORDINAL_POSITION";

  QSqlQuery query(*m_db);
  if (!query.exec(sql)) {
    qDebug() << query.lastError().text();
    return tables;
  }

  QString rupture;
  SqlTable t;
  while (query.next()) {
    if (rupture != query.value(0).toString()) {
      if (rupture != "") {
        tables << t;
        t = SqlTable();
      }
      t.name = query.value(0).toString();
      if (query.value(1).toString().toLower() == "base table") {
        t.type = Table;
      } else {
        t.type = ViewTable;
      }
      rupture = t.name;
    }

    SqlColumn c;
    c.name = query.value(2).toString();
    c.primaryKey = false;
    SqlType ty;
    ty.name = query.value(3).toString();
    c.type = ty;
    c.permitsNull = query.value(4).toString().toLower() == "yes";
    c.primaryKey = query.value(5).toInt();
    t.columns << c;
  }

  if (t.name.length() > 0) {
    tables << t;
  }

  return tables;
}

Q_EXPORT_PLUGIN2(dbm_mysql_wrapper, MysqlWrapper)
