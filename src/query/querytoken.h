/*
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 */


#ifndef QUERYTOKEN_H
#define QUERYTOKEN_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QString>

/**
 * Jeton de requête : contient toutes les informations nécessaires pour son
 * exécution.
 */
class QueryToken : public QObject
{
Q_OBJECT
public:
  QueryToken(QString query, QSqlDatabase *db, bool enqueue =true,
             QObject *parent = 0);


  QSqlDatabase       *db()          { return m_db;        };
  unsigned int        duration()    { return m_duration;  };
  QString             query()       { return m_query;     };
  bool                enqueue()     { return m_enqueue;   };
  QSqlError           error()       { return m_error;     };
  QSqlQueryModel     *model()       { return m_model;     };
  void setDb(QSqlDatabase *db)      { m_db = db;          };
  void setDuration(unsigned int d)  { m_duration = d;     };
  void setQuery(QString q)          { m_query = q;        };

  Q_PROPERTY(QSqlDatabase*    db      READ db         WRITE setDb)
  Q_PROPERTY(QSqlError        error   READ error)
  Q_PROPERTY(QSqlQueryModel*  model   READ model)
  Q_PROPERTY(QString          query   READ query      WRITE setQuery)

signals:
  void accepted();
  void finished(QSqlError);
  void rejected();
  void started();

public slots:
  void accept();
  void reject();
  void validate(QSqlError err, unsigned int duration);

private:
  QSqlDatabase     *m_db;
  unsigned int      m_duration;
  bool              m_enqueue;
  QSqlError         m_error;
  QSqlQueryModel   *m_model;
  QString           m_query;
};


#endif // QUERYTOKEN_H
