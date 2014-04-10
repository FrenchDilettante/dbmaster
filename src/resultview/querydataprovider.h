#ifndef QUERYDATAPROVIDER_H
#define QUERYDATAPROVIDER_H

#include "dataprovider.h"

#include <QSqlQuery>

class QueryDataProvider : public DataProvider {
Q_OBJECT
public:
  explicit QueryDataProvider(QObject *parent = 0);

  bool acceptsFilter() { return false; };
  bool isReadOnly() { return true; };
  QSqlError lastError();
  QSqlQueryModel* model() { return m_model; };

  void setQuery(QString query, QSqlDatabase db);

signals:

public slots:

protected:
  void run();

private:
  QSqlDatabase db;
  QSqlQueryModel* m_model;
  QString query;

};

#endif // QUERYDATAPROVIDER_H
