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


#include "querytoken.h"

QueryToken::QueryToken(QString query, QSqlDatabase *db, QObject *parent)
  : QObject(parent)
{
  qRegisterMetaType<QSqlError>("QSqlError");
  m_db = db;
  m_duration = 0;
  m_model = new QSqlQueryModel(this);
  m_query = query;
}

void QueryToken::accept()
{
  emit accepted();
}

void QueryToken::reject()
{
  emit rejected();
}

void QueryToken::validate(QSqlError err, unsigned int duration)
{
  m_duration = duration;
  m_error = err;

  emit finished(m_error);
}
