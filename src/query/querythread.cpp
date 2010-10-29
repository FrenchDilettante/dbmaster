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


#include "querythread.h"

#include <time.h>

QueryThread::QueryThread(QSqlDatabase *_db, QObject *parent)
  : QThread(parent)
{
  this->m_db = _db;
}

void QueryThread::push(QueryToken *token)
{
  tokenStack.push(token);
}

void QueryThread::run()
{
  bool            exec;
  QSqlQuery       query;
  time_t          startTime;
  QueryToken     *token;

  while(!tokenStack.isEmpty())
  {
    token = tokenStack.pop();
    if(token->db() != m_db)
      continue;

//    query = QSqlQuery(token->query(), *m_db);
    query = QSqlQuery(*m_db);
    startTime = time(NULL);
    exec = query.exec(token->query());
    token->validate(query.lastError(), time(NULL) - startTime);

    if(exec)
    {
      token->model()->setQuery(query);
    }

    emit queryExecuted(exec);
  }
}
