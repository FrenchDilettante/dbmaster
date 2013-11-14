#ifndef DATAPROVIDER_H
#define DATAPROVIDER_H

#include <QSqlError>
#include <QSqlQueryModel>
#include <QThread>

class DataProvider : public QThread {
  Q_OBJECT
public:
  virtual QSqlQueryModel* model() =0;

  virtual bool isReadOnly() =0;

signals:
  void complete();
  void error(QSqlError);

public slots:

protected:
  virtual void run() =0;
};

#endif // DATAPROVIDER_H
