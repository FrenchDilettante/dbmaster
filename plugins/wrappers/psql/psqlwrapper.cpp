#include "psqlwrapper.h"

#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

PsqlWrapper::PsqlWrapper(QSqlDatabase *db)
  : QObject(NULL) {
  m_db = db;
}

SqlWrapper::WrapperFeatures PsqlWrapper::features() {
  return Schemas;
}

SqlWrapper* PsqlWrapper::newInstance(QSqlDatabase *db) {
  return new PsqlWrapper(db);
}

/**
 * Récupération de la liste des tables
 */
QList<SqlSchema> PsqlWrapper::schemas() {
  QList<SqlSchema> schemas;

  if (!m_db) {
    return schemas;
  }

  QString sql;
  sql += "SELECT schemaname, typname, 'T', attname, attnotnull, attlen, attnum ";
  sql += "FROM pg_attribute, pg_type, pg_tables ";
  sql += "WHERE attrelid = typrelid ";
  sql +=   "AND attname NOT IN ('cmin', 'cmax', 'ctid', 'oid', ";
  sql +=      "'tableoid', 'xmin', 'xmax') ";
  sql +=   "AND typname = tablename ";
  sql +=   "AND typname not like 'pg_%' ";


  sql += "UNION ";

  sql += "SELECT schemaname, typname, 'V', attname, attnotnull, attlen, attnum ";
  sql += "FROM pg_attribute, pg_type, pg_views ";
  sql += "WHERE attrelid = typrelid ";
  sql +=   "AND attname NOT IN ('cmin', 'cmax', 'ctid', 'oid', ";
  sql +=      "'tableoid', 'xmin', 'xmax') ";
  sql +=   "AND typname = viewname ";
  sql +=   "AND typname not like 'pg_%' ";

  sql += "ORDER BY schemaname, typname, attnum ";

  QSqlQuery query(*m_db);
  if (!query.exec(sql)) {
    qDebug() << query.lastError().text();
    return schemas;
  }

  bool first = true;
  bool ruptureSchema = false;
  bool ruptureTable = false;
  SqlSchema s;
  SqlTable t;
  while (query.next()) {
    // Schémas
    ruptureSchema = s.name != query.value(0).toString();

    if (ruptureSchema && s.name != "") {
      s.tables << t;
      schemas << s;
    }

    if (ruptureSchema) {
      s = SqlSchema();
      s.name = query.value(0).toString();
      s.defaultSchema = s.name == "public";
    }

    // Tables
    ruptureTable = t.name != query.value(1).toString();

    if (first || (ruptureTable && !ruptureSchema && t.name != "")) {
      first = false;
      s.tables << t;
    }

    if (ruptureTable) {
      t = SqlTable();
      t.name = query.value(1).toString();
      if (query.value(2).toString() == "T") {
        t.type = Table;
      } else if (query.value(2).toString() == "V") {
        t.type = ViewTable;
      }
    }

    // Colonnes
    SqlColumn c;
    c.name = query.value(3).toString();
    c.permitsNull = query.value(4).toBool();
    c.primaryKey = false;
    t.columns << c;
  }

  if (s.name.length() > 0) {
    if (t.name.length() > 0) {
      s.tables << t;
    }
    schemas << s;
  }

  return schemas;
}

QList<SqlTable> PsqlWrapper::tables() {
  QList<SqlTable> tables;

  if (m_db) {
    return tables;
  }

  QString sql = "select ";

  return tables;
}

Q_EXPORT_PLUGIN2(dbm_psql_wrapper, PsqlWrapper)
