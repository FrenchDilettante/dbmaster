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


#ifndef QUERYSCHEDULER_H
#define QUERYSCHEDULER_H

#include "querythread.h"
#include "querytoken.h"

#include <QtCore>
#include <QtSql>

class QuerySchedulerPrivate : public QObject
{
Q_OBJECT
public:
  QuerySchedulerPrivate();

  void enqueue(QueryToken *token);

signals:
  void countChanged(int);

private:
  void run(QueryToken *token, QueryThread *th);
  int                 queryCount;
  QList<QueryThread*> threads;

private slots:
  void forwardExec(bool exec);
};

class QueryScheduler
{
public:
  static void enqueue(QueryToken *token);


  static QuerySchedulerPrivate *instance()  { return d;   };

private:
  static QuerySchedulerPrivate *d;
};

#endif // QUERYSCHEDULER_H
