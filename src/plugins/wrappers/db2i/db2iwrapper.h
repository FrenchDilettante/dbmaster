#ifndef DB2IWRAPPER_H
#define DB2IWRAPPER_H

#include "../../sqlwrapper.h"

#include <QCoreApplication>

class Db2iWrapper : public QObject, public SqlWrapper {
Q_OBJECT
Q_INTERFACES(SqlWrapper)
public:
  Db2iWrapper(QObject *parent = 0);
  Db2iWrapper(QSqlDatabase *db);

  // Fonctions de Plugin
  QString plid() { return "DBM.DB2I.WRAPPER"; };
  QString title() { return tr("DB2 for System i wrapper"); };
  QString vendor() { return "WMG Informatique"; };
  QString version() { return QCoreApplication::applicationVersion(); };

  // Fonctions de SqlWrapper
  WrapperFeatures features();
  SqlWrapper* newInstance(QSqlDatabase *db);
  QString driver() { return "QODBC"; };
  QString driverName() { return tr("DB2 for i"); };
  QString driverIconCode() { return "db_db2"; };
  SqlTable table(QString t);
  QList<SqlTable> tables();

signals:

public slots:

};

#endif // DB2IWRAPPER_H
