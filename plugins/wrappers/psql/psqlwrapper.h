#ifndef PSQLWRAPPER_H
#define PSQLWRAPPER_H

#include "sqlwrapper.h"
#include "psqlconfig.h"

#include <QCoreApplication>
#include <QObject>

class PsqlWrapper : public QObject, public SqlWrapper {
Q_OBJECT
Q_INTERFACES(SqlWrapper)
public:
  PsqlWrapper(QSqlDatabase *db =0);

  // Fonctions de Plugin
  QString plid() { return "DBM.PSQL.WRAPPER"; };
  void    save();
  QString title() { return tr("PostgreSQL wrapper"); };
  QString vendor() { return "DbMaster"; };
  QString version() { return QCoreApplication::applicationVersion(); };

  // Fonctions de SqlWrapper
  QList<SqlColumn> columns(QString table);
  QDialog*        configDialog() { return m_configDialog; };
  WrapperFeatures features();
  SqlWrapper*     newInstance(QSqlDatabase *db);
  SqlSchema       schema(QString s);
  QList<SqlSchema> schemas();
  QString         driver() { return "QPSQL"; };
  SqlTable        table(QString t);
  QList<SqlTable> tables(QString schema);

  // Fonctions propres
  static bool pgCatalogHidden;
  static bool informationSchemaHidden;

signals:

public slots:

private:
  PsqlConfig* m_configDialog;

};

#endif // PSQLWRAPPER_H
