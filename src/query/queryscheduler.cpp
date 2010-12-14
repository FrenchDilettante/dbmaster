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

/**
 * Place le jeton en attente
 */
void QuerySchedulerPrivate::enqueue(QueryToken *token) {

  if(!token->db()->isOpen())
    token->reject();

  // à quel thread sera affecté le jeton (-1 = nouveau thread)
  int index = -1;

  // si le jeton doit être placé en file (comportement par défaut)
  if (token->enqueue()) {
    // on cherche le thread associé à sa connexion
    for(int i=0; index<0 && i<threads.size(); i++) {
      if(threads[i]->isRunning() && threads[i]->db() == token->db())
        index = i;

      if(!threads[i]->isRunning()) {
        threads[i]->setDb(token->db());
        index = i;
      }
    }
  }

  QueryThread *th;
  if(index < 0) {
    th = new QueryThread(token->db(), this);
    connect(th, SIGNAL(queryExecuted(bool)), this, SLOT(forwardExec(bool)));
    threads << th;
  } else {
    th = threads[index];
  }

  run(token, th);
}

void QuerySchedulerPrivate::forwardExec(bool exec)
{
  queryCount--;
  emit countChanged(queryCount);
}

/**
 * Place la requête dans la pile du thread.
 *
 * @param token Jeton à exécuter
 * @param th Thread sur lequel le jeton sera placé
 */
void QuerySchedulerPrivate::run(QueryToken *token, QueryThread *th) {
  th->enqueue(token);
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

void QueryScheduler::enqueue(QueryToken *token) {
  d->enqueue(token);
}
