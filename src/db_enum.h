#ifndef DB_ENUM_H
#define DB_ENUM_H

#include "plugins/sqlwrapper.h"

#include <QString>
#include <QVariant>

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

enum ConstraintType {
  ForeignKey,
  PrimaryKey,
  Unique
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
  bool primaryKey;
  SqlType type;
  bool permitsNull;
  QVariant defaultValue;
  QString comment;
};

struct SqlIndex {
  QString name;
  QList<SqlColumn> columns;
  QString owner;
  QString comment;
};

struct SqlTable {
  QString name;
  int columnCount;
  QList<SqlColumn> columns;
  QString owner;
  TableType type;
  QString comment;
};

struct SqlConstraint {
  QString name;
  SqlTable table;
  ConstraintType type;
};

struct SqlSchema {
  bool defaultSchema;
  QString name;
  QString owner;
  QList<SqlTable> tables;
};

#endif // DB_ENUM_H
