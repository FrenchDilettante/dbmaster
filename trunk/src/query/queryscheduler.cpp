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


#include "queryscheduler.h"

QuerySchedulerPrivate::QuerySchedulerPrivate()
  : QObject(0)
{
  queryCount = 0;
}

void QuerySchedulerPrivate::forwardExec(bool exec)
{
  queryCount--;
  emit countChanged(queryCount);
}

void QuerySchedulerPrivate::push(QueryToken *token)
{
  if(!token->db()->isOpen())
    token->reject();

  int index = -1;
  for(int i=0; index<0 && i<threads.size(); i++)
  {
    if(threads[i]->isRunning() && threads[i]->db() == token->db())
      index = i;

    if(!threads[i]->isRunning())
    {
      threads[i]->setDb(token->db());
      index = i;
    }
  }

  QueryThread *th;
  if(index < 0)
  {
    th = new QueryThread(token->db(), this);
    connect(th, SIGNAL(queryExecuted(bool)), this, SLOT(forwardExec(bool)));
    threads << th;
  } else {
    th = threads[index];
  }
  th->push(token);
  queryCount++;
  token->accept();
  emit countChanged(queryCount);
  if(!th->isRunning())
    th->start();
}


/*
 * QueryScheduler
 */

QuerySchedulerPrivate *QueryScheduler::d = new QuerySchedulerPrivate();

void QueryScheduler::push(QueryToken *token)
{
  d->push(token);
}
