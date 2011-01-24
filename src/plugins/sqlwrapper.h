/*
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 */


#ifndef WRAPPER_H
#define WRAPPER_H

#include "plugin.h"

#include <QList>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QString>
#include <QStringList>

struct SqlType {
  QString name;
  bool hasSize;
  int size;
};

struct SqlColumn {
  SqlType type;
  bool permitsNull;
};

struct SqlTable {
  QString name;
  QList<SqlColumn> columns;
};

struct SqlSchema {
  QString name;
  QList<SqlTable> tables;
};

class SqlWrapper : public Plugin {
public:
  enum ColumnFamily {
    Blob,
    Char,
    Date,
    Float,
    Integer,
    Time,
    Timestamp,
    Varchar
  };

  enum DbType {
    FileDb,
    IndexedDb,
    LocalDb,
    RemoteDb
  };

  enum WrapperFeature {
    CustomTypes,
    Dumps,
    ForeignKeys,
    Indexes,
    Schemas,
    StoredProcedures,
    Triggers,
    Views
  };

  /**
   * Retourne la liste des schémas disponibles sur cette connexion.
   *
   * @param fillTables
   *    Cette option permet à l'adaptateur de remplir la liste des tables pour
   *    chaque schéma. Voir la variable tables du type SqlSchema.
   *    Noter que cette option est par défaut à vrai.
   *
   * @return une liste vide si la fonctionnalité n'est pas supportée.
   */
  virtual QList<SqlSchema> schemas(bool fillTables =true) =0;
};

Q_DECLARE_INTERFACE(SqlWrapper, "dbmaster.Wrapper")

#endif // WRAPPER_H
