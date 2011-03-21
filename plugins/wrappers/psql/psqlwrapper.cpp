#include "psqlwrapper.h"

#include <QDebug>
#include <QSettings>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

PsqlWrapper::PsqlWrapper(QSqlDatabase *db)
  : QObject(NULL) {
  m_db = db;

  QSettings s;
  s.beginGroup(plid());
  informationSchemaHidden = s.value("informationSchemaHidden", false).toBool();
  pgCatalogHidden = s.value("pgCatalogHidden", false).toBool();
  s.endGroup();

  m_configDialog = new PsqlConfig(this);
}

SqlWrapper::WrapperFeatures PsqlWrapper::features() {
  return ODBC | Schemas;
}

SqlWrapper* PsqlWrapper::newInstance(QSqlDatabase *db) {
  return new PsqlWrapper(db);
}

void PsqlWrapper::save() {
  QSettings s;
  s.beginGroup(plid());
  s.setValue("informationSchemaHidden", informationSchemaHidden);
  s.setValue("pgCatalogHidden", pgCatalogHidden);
  s.endGroup();
}

/**
 * Récupération d'un schéma
 */
SqlSchema PsqlWrapper::schema(QString sch) {
  SqlSchema schema;

  if (!m_db) {
    return schema;
  }

  QString sql;
  sql += "SELECT '" + sch + "', t.table_name, t.table_type, c.column_name, ";
  sql += "c.is_nullable, c.data_type ";
  sql += "FROM INFORMATION_SCHEMA.COLUMNS C ";
  sql += "INNER JOIN INFORMATION_SCHEMA.TABLES T ";
  sql +=   "ON C.TABLE_NAME = T.TABLE_NAME ";
  sql +=      "AND C.TABLE_SCHEMA = T.TABLE_SCHEMA ";
  sql += "WHERE t.table_catalog='" + m_db->databaseName() + "' ";
  sql +=   "AND t.table_schema='" + sch + "' ";
  sql += "ORDER BY table_name, ordinal_position ";

  QSqlQuery query(*m_db);
  if (!query.exec(sql)) {
    qDebug() << query.lastError().text();
    return schema;
  }

  bool ruptureTable = false;
  SqlTable t;
  while (query.next()) {
    schema.name = query.value(0).toString();
    schema.defaultSchema = schema.name == "public";

    // Tables
    ruptureTable = t.name != query.value(1).toString();

    if (ruptureTable) {
      if (t.name.length() > 0) {
        schema.tables << t;
      }

      t = SqlTable();
      t.name = query.value(1).toString();
      if (query.value(2).toString() == "BASE TABLE") {
        t.type = Table;
      } else if (query.value(2).toString() == "VIEW") {
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

  if (t.name.length() > 0) {
    schema.tables << t;
  }

  return schema;
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
  sql += "SELECT t.table_schema, t.table_name, t.table_type, c.column_name, ";
  sql += "c.is_nullable, c.data_type ";
  sql += "FROM INFORMATION_SCHEMA.COLUMNS C ";
  sql += "INNER JOIN INFORMATION_SCHEMA.TABLES T ";
  sql +=   "ON C.TABLE_NAME = T.TABLE_NAME ";
  sql +=      "AND C.TABLE_SCHEMA = T.TABLE_SCHEMA ";
  sql +=   "WHERE t.table_catalog='" + m_db->databaseName() + "' ";
  if (informationSchemaHidden) {
    sql +=   "AND t.table_schema <> 'information_schema' ";
  }
  if (pgCatalogHidden) {
    sql +=   "AND t.table_schema <> 'pg_catalog' ";
  }
  sql +=   "ORDER BY table_schema, table_name, ordinal_position ";

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
      if (query.value(2).toString() == "BASE TABLE") {
        t.type = Table;
      } else if (query.value(2).toString() == "VIEW") {
        t.type = ViewTable;
      }
    }

    // Colonnes
    SqlColumn c;
    c.name = query.value(3).toString();
    c.permitsNull = query.value(4).toBool();
    c.primaryKey = false;
    c.defaultValue = query.value(6);
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

SqlTable PsqlWrapper::table(QString t) {
  SqlTable table;

  if (!m_db) {
    return table;
  }

  QString sql;
  sql += "SELECT schemaname, typname, 'T', attname, attnotnull, attlen, attnum ";
  sql += "FROM pg_attribute, pg_type, pg_tables ";
  sql += "WHERE attrelid = typrelid ";
  sql +=   "AND attname NOT IN ('cmin', 'cmax', 'ctid', 'oid', ";
  sql +=      "'tableoid', 'xmin', 'xmax') ";
  sql +=   "AND typname = tablename ";
  sql +=   "AND typname = '" + t + "' ";


  sql += "UNION ";

  sql += "SELECT schemaname, typname, 'V', attname, attnotnull, attlen, attnum ";
  sql += "FROM pg_attribute, pg_type, pg_views ";
  sql += "WHERE attrelid = typrelid ";
  sql +=   "AND attname NOT IN ('cmin', 'cmax', 'ctid', 'oid', ";
  sql +=      "'tableoid', 'xmin', 'xmax') ";
  sql +=   "AND typname = viewname ";
  sql +=   "AND typname = '" + t + "' ";

  sql += "ORDER BY schemaname, typname, attnum ";

  QSqlQuery query(*m_db);
  if (!query.exec(sql)) {
    qDebug() << query.lastError().text();
    return table;
  }

  bool first = true;
  while (query.next()) {
    if (first) {
      table.name = query.value(1).toString();
      if (query.value(2).toString() == "T") {
        table.type = Table;
      } else if (query.value(2).toString() == "V") {
        table.type = ViewTable;
      }
      first = false;
    }

    // Colonnes
    SqlColumn c;
    c.name = query.value(3).toString();
    c.permitsNull = query.value(4).toBool();
    c.primaryKey = false;
    table.columns << c;
  }

  return table;
}

Q_EXPORT_PLUGIN2(dbm_psql_wrapper, PsqlWrapper)
