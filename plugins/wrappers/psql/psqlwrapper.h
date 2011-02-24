#ifndef PSQLWRAPPER_H
#define PSQLWRAPPER_H

#include "sqlwrapper.h"

#include <QCoreApplication>
#include <QObject>

class PsqlWrapper : public QObject, public SqlWrapper {
Q_OBJECT
Q_INTERFACES(SqlWrapper)
public:
  PsqlWrapper(QSqlDatabase *db =0);

  // Fonctions de Plugin
  QString plid() { return "DBM.PSQL.WRAPPER"; };
  QString title() { return tr("PostgreSQL wrapper"); };
  QString vendor() { return "DbMaster"; };
  QString version() { return QCoreApplication::applicationVersion(); };

  // Fonctions de SqlWrapper
  WrapperFeatures features();
  SqlWrapper* newInstance(QSqlDatabase *db);
  QList<SqlSchema> schemas();
  QString driver() { return "QPSQL"; };
  QList<SqlTable> tables();

signals:

public slots:

};

#endif // PSQLWRAPPER_H
