#include "mysqlwrapper.h"

#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

MysqlWrapper::MysqlWrapper(QObject *parent)
  : QObject(parent) {
}

MysqlWrapper::MysqlWrapper(QSqlDatabase db)
  : QObject(NULL) {
  m_db = db;
}

QList<SqlColumn> MysqlWrapper::columns(QString table) {
  QList<SqlColumn> cols;

  QString sql;

  // Récupération des colonnes

  sql += "SELECT C.COLUMN_NAME, C.COLUMN_TYPE, C.IS_NULLABLE, ";
  sql += "C.COLUMN_DEFAULT, C.COLUMN_COMMENT, C.ORDINAL_POSITION ";
  sql += "FROM INFORMATION_SCHEMA.COLUMNS C ";
  sql +=   "INNER JOIN INFORMATION_SCHEMA.TABLES T ";
  sql +=   "ON C.TABLE_NAME = T.TABLE_NAME ";
  sql +=     "AND C.TABLE_SCHEMA = T.TABLE_SCHEMA ";
  sql += "WHERE C.TABLE_SCHEMA='" + m_db.databaseName() + "' ";
  sql += "AND C.TABLE_NAME='" + table + "' ";

  sql += "ORDER BY ORDINAL_POSITION";

  if (!m_db.isOpen()) {
    m_db.open();
  }

  QSqlQuery query(m_db);
  if (!query.exec(sql)) {
    qDebug() << query.lastError().text();
    m_db.close();
    return cols;
  }

  while (query.next()) {
    SqlColumn c;
    c.name = query.value(0).toString();
    c.primaryKey = false;
    SqlType ty;
    ty.name = query.value(1).toString().toUpper();
    c.type = ty;
    c.permitsNull = query.value(2).toString().toLower() == "yes";
    c.primaryKey = false;
    c.defaultValue = query.value(3);
    c.comment = query.value(4).toString();
    cols << c;
  }


  // Récupération des clés primaires

  sql = "";
  sql += "SELECT COLUMN_NAME ";
  sql += "FROM INFORMATION_SCHEMA.KEY_COLUMN_USAGE ";
  sql += "WHERE CONSTRAINT_SCHEMA='" + m_db.databaseName() + "' ";
  sql +=   "AND CONSTRAINT_NAME='PRIMARY' ";
  sql +=   "AND TABLE_NAME='" + table + "' ";

  if (!query.exec(sql)) {
    qDebug() << query.lastError().text();
    m_db.close();
    return cols;
  }

  while (query.next()) {
    QString column = query.value(0).toString();
    int position = -1;

    for (int i=0; i<cols.size(); i++) {
      if (cols[i].name == column) {
        position = i;
        cols[position].primaryKey = true;
      }
    }

    if (position == -1) {
      qDebug() << "Unable to find" << column << "in table" << table;
    }
  }

  m_db.close();
  return cols;
}

QList<SqlConstraint> MysqlWrapper::constraints(QString table) {
  QList<SqlConstraint> cons;

  QString sql;

  // Récupération des colonnes

  sql += "SELECT C.CONSTRAINT_NAME, C.CONSTRAINT_TYPE ";
  sql += "FROM INFORMATION_SCHEMA.TABLE_CONSTRAINTS C ";
  sql +=   "INNER JOIN INFORMATION_SCHEMA.TABLES T ";
  sql +=   "ON C.TABLE_NAME = T.TABLE_NAME ";
  sql +=     "AND C.TABLE_SCHEMA = T.TABLE_SCHEMA ";
  sql += "WHERE C.TABLE_SCHEMA='" + m_db.databaseName() + "' ";
  sql += "AND C.TABLE_NAME='" + table + "' ";

  sql += "ORDER BY CONSTRAINT_NAME";

  if (!m_db.isOpen()) {
    m_db.open();
  }

  QSqlQuery query(m_db);
  if (!query.exec(sql)) {
    qDebug() << query.lastError().text();
    m_db.close();
    return cons;
  }

  while (query.next()) {
    SqlConstraint c;
    c.name = query.value(0).toString();
    QString ty = query.value(1).toString();
    if (ty == "PRIMARY KEY") {
      c.type = PrimaryKey;
    } else if (ty == "FOREIGN KEY") {
      c.type = ForeignKey;
    } else {
      c.type = Unique;
    }

    cons << c;
  }

  m_db.close();
  return cons;
}

SqlWrapper::WrapperFeatures MysqlWrapper::features() {
  return BasicFeatures | ODBC;
}

SqlWrapper* MysqlWrapper::newInstance(QSqlDatabase db) {
  return new MysqlWrapper(db);
}

SqlTable MysqlWrapper::table(QString t) {
  SqlTable table;

  QString sql;

  // Récupération des colonnes

  sql += "SELECT C.TABLE_NAME, T.TABLE_TYPE, T.TABLE_COMMENT, C.COLUMN_NAME, ";
  sql += "C.COLUMN_TYPE, C.IS_NULLABLE, C.COLUMN_DEFAULT, C.COLUMN_COMMENT, ";
  sql += "C.ORDINAL_POSITION ";
  sql += "FROM INFORMATION_SCHEMA.COLUMNS C ";
  sql +=   "INNER JOIN INFORMATION_SCHEMA.TABLES T ";
  sql +=   "ON C.TABLE_NAME = T.TABLE_NAME ";
  sql +=     "AND C.TABLE_SCHEMA = T.TABLE_SCHEMA ";
  sql += "WHERE C.TABLE_SCHEMA='" + m_db.databaseName() + "' ";
  sql += "AND C.TABLE_NAME='" + t + "' ";

  sql += "ORDER BY ORDINAL_POSITION";

  if (!m_db.isOpen()) {
    m_db.open();
  }

  QSqlQuery query(m_db);
  if (!query.exec(sql)) {
    qDebug() << query.lastError().text();
    m_db.close();
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
  sql += "WHERE CONSTRAINT_SCHEMA='" + m_db.databaseName() + "' ";
  sql +=   "AND CONSTRAINT_NAME='PRIMARY' ";
  sql +=   "AND TABLE_NAME='" + t + "' ";

  if (!query.exec(sql)) {
    qDebug() << query.lastError().text();
    m_db.close();
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

  m_db.close();
  return table;
}

QList<SqlTable> MysqlWrapper::tables() {
  QList<SqlTable> tables;

  QString sql;

  sql += "SELECT TABLE_NAME, TABLE_TYPE ";
  sql += "FROM INFORMATION_SCHEMA.TABLES ";
  sql += "WHERE TABLE_SCHEMA = '";
  sql += m_db.databaseName();
  sql += "' ";

  sql += "ORDER BY TABLE_NAME";

  if (!m_db.isOpen()) {
    m_db.open();
  }

  QSqlQuery query(m_db);
  if (!query.exec(sql)) {
    qDebug() << query.lastError().text();
    m_db.close();
    return tables;
  }

  SqlTable t;
  while (query.next()) {
    t = SqlTable();
    t.name = query.value(0).toString();
    t.type = query.value(1).toString().toLower() == "base table"
        ? Table : ViewTable;

    tables << t;
  }
  m_db.close();
  return tables;
}

Q_EXPORT_PLUGIN2(dbm_mysql_wrapper, MysqlWrapper)
