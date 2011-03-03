#ifndef MYSQLWRAPPER_H
#define MYSQLWRAPPER_H

#include "sqlwrapper.h"

#include <QCoreApplication>

class MysqlWrapper : public QObject, public SqlWrapper
{
Q_OBJECT
Q_INTERFACES(SqlWrapper)
public:
  MysqlWrapper(QObject *parent = 0);
  MysqlWrapper(QSqlDatabase *db);

  // Fonctions de Plugin
  QString plid() { return "DBM.MYSQL.WRAPPER"; };
  QString title() { return tr("MySQL wrapper"); };
  QString vendor() { return "DbMaster"; };
  QString version() { return QCoreApplication::applicationVersion(); };

  // Fonctions de SqlWrapper
  WrapperFeatures features();
  SqlWrapper* newInstance(QSqlDatabase *db);
  QString driver() { return "QMYSQL"; };
  SqlTable table(QString t);
  QList<SqlTable> tables();

signals:

public slots:

};

#endif // MYSQLWRAPPER_H
