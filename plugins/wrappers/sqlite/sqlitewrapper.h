#ifndef SQLITEWRAPPER_H
#define SQLITEWRAPPER_H

#include "sqlwrapper.h"

#include <QCoreApplication>
#include <QObject>

class SqliteWrapper : public QObject, public SqlWrapper {
Q_OBJECT
Q_INTERFACES(SqlWrapper)
public:
  SqliteWrapper(QObject *parent = 0);
  SqliteWrapper(QSqlDatabase *db);

  // Fonctions de Plugin
  QString plid() { return "DBM.SQLITE.WRAPPER"; };
  QString title() { return tr("SQLite wrapper"); };
  QString vendor() { return "DbMaster"; };
  QString version() { return QCoreApplication::applicationVersion(); };

  // Fonctions de SqlWrapper
  WrapperFeatures features();
  SqlWrapper *newInstance(QSqlDatabase *db);
  QString driver() { return "QSQLITE"; };
  QList<SqlTable> tables();

signals:

public slots:

};

#endif // SQLITEWRAPPER_H
