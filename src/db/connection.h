#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>

class Connection : public QObject {
Q_OBJECT
public:
  Connection(QSqlDatabase* db, QString alias, QObject *parent = 0);

  QString alias();
  QSqlDatabase* db();

  void setAlias(QString alias);

signals:
  void closed();
  void opened();

public slots:
  void close();
  void open(QString password=QString::null);
  void toggle();

private:
  QString m_alias;
  QSqlDatabase* m_db;

};

#endif // DATABASE_H
