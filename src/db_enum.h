#ifndef DB_ENUM_H
#define DB_ENUM_H

#include "plugins/sqlwrapper.h"

#include <QString>

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

enum TableType {
  Table,
  ViewTable,
  SysTable
};

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
  TableType type;
};

struct SqlSchema {
  QString name;
  QList<SqlTable> tables;
};

#endif // DB_ENUM_H
