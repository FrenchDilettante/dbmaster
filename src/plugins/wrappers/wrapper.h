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

#include "../plugin.h"

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

class SqlWrapper : public Plugin, private QSqlDriver
{
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

  SqlWrapper();
};

Q_DECLARE_INTERFACE(SqlWrapper, "dbmaster.Wrapper")

#endif // WRAPPER_H
