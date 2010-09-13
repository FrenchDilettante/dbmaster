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


#ifndef QUERYTHREAD_H
#define QUERYTHREAD_H

#include "querytoken.h"

#include <QtCore>
#include <QtSql>

class QueryThread : public QThread
{
Q_OBJECT
public:
  QueryThread(QSqlDatabase *_db, QObject *parent = 0);

  void push(QueryToken *token);

  QSqlDatabase *db()            { return m_db;    };
  void setDb(QSqlDatabase *db)  { m_db = db;      };

  Q_PROPERTY(QSqlDatabase*    db    READ db     WRITE setDb)

signals:
  void queryExecuted(bool);

public slots:

private:
  void run();

  QSqlDatabase       *m_db;
  QStack<QueryToken*> tokenStack;
};

#endif // QUERYTHREAD_H
