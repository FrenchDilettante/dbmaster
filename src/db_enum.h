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
  QString name;
  SqlType type;
  bool permitsNull;
};

struct SqlTable {
  QString name;
  QList<SqlColumn> columns;
  QString owner;
  TableType type;
};

struct SqlSchema {
  bool defaultSchema;
  QString name;
  QString owner;
  QList<SqlTable> tables;
};

#endif // DB_ENUM_H
