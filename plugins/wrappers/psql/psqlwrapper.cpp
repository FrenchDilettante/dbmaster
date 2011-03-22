#include "psqlwrapper.h"

#include <QDebug>
#include <QSettings>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

bool PsqlWrapper::informationSchemaHidden = true;
bool PsqlWrapper::pgCatalogHidden = true;

PsqlWrapper::PsqlWrapper(QSqlDatabase *db)
  : QObject(NULL) {
  m_db = db;

  QSettings s;
  s.beginGroup(plid());
  informationSchemaHidden = s.value("informationSchemaHidden", true).toBool();
  pgCatalogHidden = s.value("pgCatalogHidden", true).toBool();
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
  sql += "c.is_nullable, c.data_type, c.column_default ";
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
  sql += "c.is_nullable, c.data_type, c.column_default ";
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

  // compte le nombre total de tables
  // si = 0 alors inutile de chercher les clés primaires.
  int tableCount = 0;

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

      tableCount += s.tables.size();
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

    tableCount += s.tables.size();
  }

  if (tableCount == 0) {
    return schemas;
  }



  // Récupération des clés primaires

  sql = "";
  sql += "SELECT U.TABLE_SCHEMA, U.TABLE_NAME, COLUMN_NAME ";
  sql += "FROM INFORMATION_SCHEMA.KEY_COLUMN_USAGE U ";
  sql += "INNER JOIN INFORMATION_SCHEMA.TABLE_CONSTRAINTS T ";
  sql += "USING (TABLE_CATALOG, TABLE_SCHEMA, TABLE_NAME, CONSTRAINT_NAME) ";
  sql += "WHERE T.CONSTRAINT_CATALOG = '" + m_db->databaseName() + "' ";
  sql +=   "AND CONSTRAINT_TYPE = 'PRIMARY KEY' ";
  sql += "ORDER BY TABLE_SCHEMA, TABLE_NAME";

  if (!query.exec(sql)) {
    qDebug() << query.lastError().text();
    return schemas;
  }

  QString ruptureS = "";
  QString ruptureT = "";
  s = schemas[0];
  int t_idx = -1;
  int s_idx = 0;
  while (query.next()) {

    ruptureS = query.value(0).toString();
    if (schemas[s_idx].name != ruptureS) {
      s_idx = -1;
      for (int i=0; i<schemas.size(); i++) {
        if (schemas[i].name == ruptureS) {
          s_idx = i;
          s = schemas[i];
          break;
        }
      }

      if (s_idx == -1) {
        s_idx = 0;
        qDebug() << "Unknown schema" << ruptureS;
        continue;
      }
    }

    if (ruptureT != query.value(1).toString()) {
      ruptureT = query.value(1).toString();
      t_idx = -1;
      for (int i=0; i<s.tables.size(); i++) {
        if (s.tables[i].name == ruptureT) {
          t_idx = i;
          break;
        }
      }

      if (t_idx < 0) {
        qDebug() << "Unknown table" << ruptureT;
        continue;
      }
    }

    QString column = query.value(2).toString();
    int position = -1;
    for (int i=0; i<schemas[s_idx].tables[t_idx].columns.size(); i++) {
      if (schemas[s_idx].tables[t_idx].columns[i].name == column) {
        position = i;
        schemas[s_idx].tables[t_idx].columns[position].primaryKey = true;
      }
    }

    if (position == -1) {
      qDebug() << "Unable to find" << column
               << "in table" << schemas[s_idx].tables[t_idx].name;
    }
  }

  return schemas;
}

SqlTable PsqlWrapper::table(QString t) {
  SqlTable table;

  if (!m_db) {
    return table;
  }

  QString sch = "public";
  if (t.contains(".")) {
    sch = t.left(t.indexOf("."));
    t = t.right(t.indexOf(".") + 2);
  }

  QString sql;
  sql += "SELECT t.table_schema, t.table_name, t.table_type, c.column_name, ";
  sql += "c.is_nullable, c.data_type, c.column_default ";
  sql += "FROM INFORMATION_SCHEMA.COLUMNS C ";
  sql += "INNER JOIN INFORMATION_SCHEMA.TABLES T ";
  sql +=   "ON C.TABLE_NAME = T.TABLE_NAME ";
  sql +=      "AND C.TABLE_SCHEMA = T.TABLE_SCHEMA ";
  sql +=   "WHERE t.table_catalog='" + m_db->databaseName() + "' ";
  if (sch.length() > 0) {
    sql +=   "AND t.table_schema = '" + sch + "'";
  }
  sql +=     "AND t.table_name = '" + t + "' ";
  sql +=   "ORDER BY ordinal_position ";

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
    c.type.name = query.value(5).toString();
    c.defaultValue = query.value(6);
    table.columns << c;
  }

  if (table.columns.size() == 0) {
    return table;
  }

  // Récupération des clés primaires

  sql = "";

  sql += "SELECT COLUMN_NAME ";
  sql += "FROM INFORMATION_SCHEMA.KEY_COLUMN_USAGE U ";
  sql += "INNER JOIN INFORMATION_SCHEMA.TABLE_CONSTRAINTS T ";
  sql += "USING (TABLE_CATALOG, TABLE_SCHEMA, TABLE_NAME, CONSTRAINT_NAME) ";
  sql += "WHERE T.CONSTRAINT_CATALOG = '" + m_db->databaseName() + "' ";
  sql +=   "AND U.TABLE_SCHEMA = '" + sch + "' ";
  sql +=   "AND U.TABLE_NAME = '" + t + "' ";
  sql +=   "AND CONSTRAINT_TYPE = 'PRIMARY KEY' ";

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
      qDebug() << "Unable to find" << column << "in table" << table.name;
    }
  }

  return table;
}

Q_EXPORT_PLUGIN2(dbm_psql_wrapper, PsqlWrapper)
