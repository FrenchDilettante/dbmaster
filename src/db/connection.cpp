#include "connection.h"

#include "tools/logger.h"

Connection::Connection(QSqlDatabase *db, QString alias, QObject *parent)
  : QObject(parent) {
  m_alias = alias;
  m_db = db;
}

void Connection::close() {
  m_db->close();
  //QSqlDatabase::removeDatabase(m_db->connectionName());

  Logger::instance->log(tr("Disconnected from %1").arg(m_alias));

  emit closed();
}

void Connection::open(QString password) {
  if (!password.isNull()) {
    m_db->setPassword(password);
  }

  if (m_db->open()) {
    Logger::instance->log(tr("Connected to %1").arg(m_alias));
  } else {
    Logger::instance->log(tr("Unable to connect to %1").arg(m_alias));
  }

  emit opened();
}

/*
 * Getters & setters
 */

QString Connection::alias() {
  return m_alias;
}

QSqlDatabase* Connection::db() {
  return m_db;
}

void Connection::setAlias(QString alias) {
  m_alias = alias;
}
