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
  return BasicFeatures | ODBC;
}

SqlWrapper* MysqlWrapper::newInstance(QSqlDatabase *db) {
  return new MysqlWrapper(db);
}

SqlTable MysqlWrapper::table(QString t) {
  SqlTable table;

  if (!m_db) {
    return table;
  }

  QString sql;

  // Récupération des colonnes

  sql += "SELECT C.TABLE_NAME, T.TABLE_TYPE, T.TABLE_COMMENT, C.COLUMN_NAME, ";
  sql += "C.COLUMN_TYPE, C.IS_NULLABLE, C.COLUMN_DEFAULT, C.COLUMN_COMMENT, ";
  sql += "C.ORDINAL_POSITION ";
  sql += "FROM INFORMATION_SCHEMA.COLUMNS C ";
  sql +=   "INNER JOIN INFORMATION_SCHEMA.TABLES T ";
  sql +=   "ON C.TABLE_NAME = T.TABLE_NAME ";
  sql +=     "AND C.TABLE_SCHEMA = T.TABLE_SCHEMA ";
  sql += "WHERE C.TABLE_SCHEMA='" + m_db->databaseName() + "' ";
  sql += "AND C.TABLE_NAME='" + t + "' ";

  sql += "ORDER BY ORDINAL_POSITION";

  QSqlQuery query(*m_db);
  if (!query.exec(sql)) {
    qDebug() << query.lastError().text();
    return table;
  }

  bool first = true;
  while (query.next()) {
    if (first) {
      table.name = query.value(0).toString();
      if (query.value(1).toString().toLower() == "base table") {
        table.type = Table;
      } else {
        table.type = ViewTable;
      }
      table.comment = query.value(2).toString();
      first = false;
    }

    SqlColumn c;
    c.name = query.value(3).toString();
    c.primaryKey = false;
    SqlType ty;
    ty.name = query.value(4).toString().toUpper();
    c.type = ty;
    c.permitsNull = query.value(5).toString().toLower() == "yes";
    c.primaryKey = false;
    c.defaultValue = query.value(6);
    c.comment = query.value(7).toString();
    table.columns << c;
  }


  // Récupération des clés primaires

  sql = "";
  sql += "SELECT COLUMN_NAME ";
  sql += "FROM INFORMATION_SCHEMA.KEY_COLUMN_USAGE ";
  sql += "WHERE CONSTRAINT_SCHEMA='" + m_db->databaseName() + "' ";
  sql +=   "AND CONSTRAINT_NAME='PRIMARY' ";
  sql +=   "AND TABLE_NAME='" + t + "' ";

  if (!query.exec(sql)) {
    qDebug() << query.lastError().text();
    return table;
  }

  while (query.next()) {
    QString column = query.value(0).toString();
    int position = -1;

    for (int i=0; i<table.columns.size(); i++) {
      if (table.columns[i].name == column) {
        position = i;
        table.columns[position].primaryKey = true;
      }
    }

    if (position == -1) {
      qDebug() << "Unable to find" << column << "in table" << t;
    }
  }

  return table;
}

QList<SqlTable> MysqlWrapper::tables() {
  QList<SqlTable> tables;

  if (!m_db) {
    return tables;
  }

  QString sql;

  sql += "SELECT C.TABLE_NAME, T.TABLE_TYPE, C.COLUMN_NAME, C.ORDINAL_POSITION ";
  sql += "FROM INFORMATION_SCHEMA.COLUMNS C ";
  sql +=   "INNER JOIN INFORMATION_SCHEMA.TABLES T ";
  sql +=   "ON C.TABLE_NAME = T.TABLE_NAME ";
  sql +=     "AND C.TABLE_SCHEMA = T.TABLE_SCHEMA ";
  sql += "WHERE C.TABLE_SCHEMA = '";
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
    c.permitsNull = false;
    c.primaryKey = false;
    c.defaultValue = QVariant();
    t.columns << c;
  }

  if (t.name.length() > 0) {
    tables << t;
  }

  if (tables.length() == 0) {
    return tables;
  }

  // Récupération des clés primaires

  sql = "";
  sql += "SELECT TABLE_NAME, COLUMN_NAME ";
  sql += "FROM INFORMATION_SCHEMA.KEY_COLUMN_USAGE ";
  sql += "WHERE CONSTRAINT_SCHEMA = '" + m_db->databaseName() + "' ";
  sql +=   "AND CONSTRAINT_NAME = 'PRIMARY' ";
  sql += "ORDER BY TABLE_NAME";

  if (!query.exec(sql)) {
    qDebug() << query.lastError().text();
    return tables;
  }

  rupture = "";
  int idx = -1;
  while (query.next()) {
    if (rupture != query.value(0).toString()) {
      rupture = query.value(0).toString();
      idx = -1;
      for (int i=0; i<tables.size(); i++) {
        if (tables[i].name == rupture) {
          idx = i;
          break;
        }
      }

      if (idx < 0) {
        qDebug() << "Unknown table" << rupture;
        continue;
      }
    }

    QString column = query.value(1).toString();
    int position = -1;
    for (int i=0; i<tables[idx].columns.size(); i++) {
      if (tables[idx].columns[i].name == column) {
        position = i;
        tables[idx].columns[position].primaryKey = true;
      }
    }

    if (position == -1) {
      qDebug() << "Unable to find" << column << "in table" << tables[idx].name;
    }
  }

  return tables;
}

Q_EXPORT_PLUGIN2(dbm_mysql_wrapper, MysqlWrapper)
