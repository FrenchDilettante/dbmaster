#ifndef TABLEDATAPROVIDER_H
#define TABLEDATAPROVIDER_H

#include "dataprovider.h"

#include <QSqlTableModel>

class TableDataProvider : public DataProvider {
Q_OBJECT
public:
  explicit TableDataProvider(QString table, QSqlDatabase *db, QObject *parent = 0);

  bool isReadOnly() { return true; };
  QSqlQueryModel* model() { return m_model; };

signals:

public slots:

protected:
  void run();

private:
  QSqlDatabase* db;
  QSqlTableModel* m_model;
  QString table;

};

#endif // TABLEDATAPROVIDER_H
