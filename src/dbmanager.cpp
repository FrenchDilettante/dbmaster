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


#include "dbmanager.h"
#include "iconmanager.h"
#include "mainwindow.h"

#include "dialogs/logdialog.h"
#include "tabwidget/abstracttabwidget.h"

DbManagerPrivate::DbManagerPrivate()
  : QThread()
{
  closingAll = false;
  nconn = 0;
  m_driverModel = new QStandardItemModel(this);
  m_model = new QStandardItemModel(this);

  setupConnections();
}

int DbManagerPrivate::addDatabase(QString driver, QString host, QString user,
                            QString pswd, QString dbnm, bool save)
{
  QSqlDatabase db = QSqlDatabase::addDatabase(driver, genConnectionName());
  db.setHostName(host);
  db.setUserName(user);
  db.setPassword(pswd);
  db.setDatabaseName(dbnm);

  foreach(QSqlDatabase *d, dbList)
  {
    if (d->hostName() == db.hostName() &&
            d->userName() == db.userName() &&
            d->password() == db.password() &&
            d->databaseName() == db.databaseName())
    {
      LogDialog::instance()->append(
              QObject::tr("Attempting to add an existing connection."));
      return indexOf(d);
    }
  }

  QSqlDatabase *newDb = new QSqlDatabase(db);
  dbList.append(newDb);
  dbMap[newDb] = new QStandardItem(dbTitle(newDb));
  dbMap[newDb]->setEditable(false);

  dbMap[newDb]->setData(DbManager::DbItem, Qt::UserRole);
  dbMap[newDb]->setIcon(IconManager::get("connect_no"));
  dbMap[newDb]->setToolTip(dbToolTip(newDb));

  if(!isRunning())
    start();

  m_model->appendRow(dbMap[newDb]);
  if(save)
  {
    saveList();
    LogDialog::instance()->append(QObject::tr("Database %1 on %2 added")
            .arg(dbnm)
            .arg(host));
  }

  return dbList.size() - 1;
}

void DbManagerPrivate::close(QSqlDatabase* db)
{
  if (!dbList.contains(db))
    return;

  qDebug() << "close" << db->hostName() << db->databaseName();
  closeStack.push(db);
}

QString DbManagerPrivate::dbTitle(QSqlDatabase *db)
{
  QString title;
  if(db->hostName().isEmpty())
    title = tr("%1 (local)").arg(db->databaseName());
  else
    title = tr("%1 on %2").arg(db->databaseName()).arg(db->hostName());

  return title;
}

/**
 * Returns a rich-texted tooltip for the database.
 */
QString DbManagerPrivate::dbToolTip(QSqlDatabase *db)
{
  QString ret;

  ret += tr("%1 on %2").prepend("<h3>").append("</h3>")
         .arg(db->databaseName())
         .arg(db->hostName());

  ret += "<table border=\"0\">";

  QString defaultLine = "<tr><td><b>%1</b></td><td>%2</td></tr>";

  // Connection's status
  QString stName;
  if(db->isOpen())
    stName = tr("Openned");
  else
    stName = tr("Closed");
  ret += defaultLine
         .arg(tr("Status"))
         .arg(stName);

  // Db driver
  ret += defaultLine
         .arg(tr("DBMS"))
         .arg(driverAlias.value(db->driverName(),
                                tr("Generic (%1)").arg(db->driverName())));

  // Host
  ret += defaultLine
         .arg(tr("Host"))
         .arg(db->hostName());

  ret += "</table>";
  return ret;
}

/**
 * Closes DbManager.
 */
void DbManagerPrivate::closeAll()
{
  if(dbMap.size() == 0)
    return;

  closingAll = true;
  closeStack << dbList.toVector();
  // waits until all connections are closed
  bool oneOpenned = true;
  while(oneOpenned)
  {
    oneOpenned = false;
    foreach(QSqlDatabase *d, dbList)
    {
      if(d->isOpen())
      {
        oneOpenned = true;
        break;
      }
    }
  }
  dbMap.clear();
}

QStandardItemModel *DbManagerPrivate::driverModel()
{
  return m_driverModel;
}

QString DbManagerPrivate::genConnectionName()
{
  nconn++;
  return QString::number(nconn);
}

QSqlDatabase* DbManagerPrivate::getDatabase(int n)
{
  if(dbMap.size() == 0)
    return NULL;

  if (n < 0)
    return dbList.first();

  if (n >= dbMap.size())
    return dbList.last();

  return dbList[n];
}

QList<QSqlDatabase*> DbManagerPrivate::getDbList()
{
  return dbList;
}

QStringList DbManagerPrivate::getDbNames(bool showHosts)
{
  QStringList ret;
  QString str;

  foreach(QSqlDatabase *db, dbList)
  {
    str = db->databaseName();
    if (str.contains("/"))
      str = str.split("/")[str.split("/").size() - 1];
    if (showHosts && !db->hostName().isEmpty())
      str.append(" on ").append(db->hostName());

    ret << str;
  }

  return ret;
}

/// DEPRECATED
int DbManagerPrivate::indexOf(QSqlDatabase *db)
{
  return dbList.indexOf(db);
}

void DbManagerPrivate::init()
{
  setupModels();
  openList();
}

QString DbManagerPrivate::lastError()
{
  return lastErr;
}

void DbManagerPrivate::open(int nb, QString pswd)
{
  open(dbList[nb], pswd);
}

void DbManagerPrivate::open(QSqlDatabase *db, QString pswd)
{
  if(!dbMap.contains(db))
    return;

  if(!pswd.isNull())
    db->setPassword(pswd);

  dbMap[db]->setIcon(IconManager::get("connect_creating"));

  openStack.push(db);
  while(closeStack.contains(db))
    closeStack.remove(closeStack.indexOf(db));
}

void DbManagerPrivate::openList()
{
  QSettings s;

  int size = s.beginReadArray("dblist");
  QString driver, host, user, pswd, name;
  for (int i = 1; i <= size; i++)
  {
    s.setArrayIndex(i);
    driver = s.value("driver").toString();
    host = s.value("hostname").toString();
    user = s.value("username").toString();
    name = s.value("database").toString();
    pswd = s.value("password", QVariant(QString::null)).toString();
    addDatabase(driver, host, user, pswd, name, false);
  }
  s.endArray();
}

void DbManagerPrivate::refreshModel()
{
  foreach(QSqlDatabase *db, dbList)
    refreshModelItem(db);
}

/**
 * @bug check indexes
 */
void DbManagerPrivate::refreshModelItem(QSqlDatabase *db)
{
  if(!dbMap.contains(db))
    return;

  QStandardItem *item = dbMap[db];
  item->setText(dbTitle(db));
  item->setToolTip(dbToolTip(db));

  QStandardItem *i;
  QModelIndex index = m_model->indexFromItem(item);
  if(db->isOpen())
  {
    item->setIcon(IconManager::get("connect_established"));
    if(m_model->rowCount(index) == 0)
    {
      QStandardItem *tablesItem =
          new QStandardItem(tr("Tables (%1)")
                            .arg(QString::number(db->tables(QSql::Tables)
                                                 .size())));
      tablesItem->setIcon(IconManager::get("folder_tables"));

      foreach(QString table, db->tables(QSql::Tables))
      {
        i = new QStandardItem(IconManager::get("table"), table);
        i->setData(DbManager::TableItem, Qt::UserRole);
        tablesItem->appendRow(i);
      }

      item->appendRow(tablesItem);

      QStandardItem *viewsItem =
          new QStandardItem(tr("Views (%1)")
                            .arg(QString::number(db->tables(QSql::Views)
                                                 .size())));

      viewsItem->setIcon(IconManager::get("folder_views"));

      foreach(QString view, db->tables(QSql::Views))
      {
        i = new QStandardItem(IconManager::get("table_lightning"), view);
        i->setData(DbManager::ViewItem, Qt::UserRole);
        viewsItem->appendRow(i);
      }

      item->appendRow(viewsItem);

      QStandardItem *sysTablesItem =
          new QStandardItem(tr("System tables (%1)")
                            .arg(QString::number(db->tables(QSql::Views)
                                                 .size())));

      sysTablesItem->setIcon(IconManager::get("folder_systemtables"));

      foreach(QString sysTable, db->tables(QSql::Views))
      {
        i = new QStandardItem(IconManager::get("folder_gear"), sysTable);
        i->setData(DbManager::SysTableItem, Qt::UserRole);
        sysTablesItem->appendRow(i);
      }

      item->appendRow(sysTablesItem);
    }
  } else {
    item->setIcon(IconManager::get("connect_no"));
    while(m_model->rowCount(index) > 0)
      m_model->removeRow(0, index);
  }
}

/**
 * Remove the database at index.
 */
void DbManagerPrivate::removeDatabase(int index)
{
  removeDatabase(dbList[index]);
}

void DbManagerPrivate::removeDatabase(QSqlDatabase *db)
{
  if(!dbMap.contains(db))
    return;

  close(db);
  m_model->removeRow(dbList.indexOf(db));
  dbMap.remove(db);
  dbList.removeAll(db);
  saveList();

  if(!isRunning())
    start();
}

/**
 * Thread de gestion des connexions
 */
void DbManagerPrivate::run()
{
  QSqlDatabase *db;

  while(dbList.size() > 0)
  {
    // traitement des connexions à ouvrir
    while(openStack.size() > 0)
    {
      foreach(QSqlDatabase *d, closeStack)
      {
        // on supprime les doublons éventuels
        while(openStack.contains(d))
          openStack.remove(openStack.indexOf(d));
      }
      db = openStack.pop();
      if(db->open()) {
        LogDialog::instance()->append(tr("Connected to %1")
                                      .arg(db->hostName()));
      } else {
        LogDialog::instance()->append(tr("Unable to connect to %1")
                                      .arg(db->hostName()));
      }

      emit statusChanged(db);
      emit statusChanged(dbMap[db]->index());
    }

    // traitement des connexions à fermer
    while(closeStack.size() > 0)
    {
      db = closeStack.pop();
      db->close();
      qDebug() << "prout" << db->databaseName();
      LogDialog::instance()->append(tr("Disconnected from %1")
                                    .arg(db->hostName()));
      if(!closingAll)
      {
        emit statusChanged(db);
        if (dbMap.contains(db)) {
          emit statusChanged(dbMap[db]->index());
        }
      }

      if(!dbList.contains(db))
        QSqlDatabase::removeDatabase(db->connectionName());
    }

    msleep(10);
  }
}

void DbManagerPrivate::saveList()
{
  QSettings s;
  s.beginWriteArray("dblist", dbMap.size());

  int i=0;
  foreach(QSqlDatabase *db, dbList)
  {
    i++;
    s.setArrayIndex(i);
    s.setValue("driver", db->driverName());
    s.setValue("hostname", db->hostName());
    s.setValue("username", db->userName());
    if (!db->password().isNull())
      s.setValue("password", db->password());
    s.setValue("database", db->databaseName());
  }
  s.endArray();
  s.sync();
}


void DbManagerPrivate::setDatabase(int nb, QSqlDatabase db)
{
  if (dbList.size() >= nb)
    return;

  QSqlDatabase *newDb = new QSqlDatabase(db);
  QSqlDatabase *oldDb = dbList[nb];

  swapDatabase(oldDb, newDb);
}

void DbManagerPrivate::setupConnections()
{
  connect(this, SIGNAL(statusChanged(QSqlDatabase*)),
          this, SLOT(refreshModelItem(QSqlDatabase*)));
}

void DbManagerPrivate::setupModels()
{
  m_driverModel->setHorizontalHeaderItem(0, new QStandardItem(tr("Driver")));

  driverAlias["QIBASE"]   = tr("Interbase/Firebird");
  driverAlias["QMYSQL"]   = tr("MySql");
  driverAlias["QMYSQL3"]  = tr("MySql 3");
  driverAlias["QODBC"]    = tr("ODBC");
  driverAlias["QODBC3"]   = tr("ODBC 3");
  driverAlias["QPSQL"]    = tr("PostGreSQL");
  driverAlias["QPSQL7"]   = tr("PostGreSQL 7");
  driverAlias["QSQLITE"]  = tr("SQLite");
  driverAlias["QSQLITE2"] = tr("SQLite 2");

  driverIcon["QIBASE"]    = QIcon(":/img/db_firebird.png");
  driverIcon["QMYSQL"]    = QIcon(":/img/db_mysql.png");
  driverIcon["QPSQL"]     = QIcon(":/img/db_postgresql.png");

  QStandardItem *item;
  foreach(QString driver, QSqlDatabase::drivers())
  {
    item = new QStandardItem();
    item->setData(driver, Qt::UserRole);
    item->setText(driverAlias.value(driver, driver));
    item->setIcon(driverIcon.value(driver, QIcon()));

    m_driverModel->appendRow(item);
  }

  m_model->setHorizontalHeaderItem(0, new QStandardItem(tr("Database")));
}

void DbManagerPrivate::swapDatabase(QSqlDatabase *oldDb, QSqlDatabase *newDb)
{
  if (!dbMap.contains(oldDb))
    return;

  QStandardItem *item = dbMap[oldDb];
  item->setText(QObject::tr("%1 on %2")
          .arg(newDb->databaseName())
          .arg(newDb->hostName()));

  oldDb->close();
  dbMap[newDb] = item;
  dbMap.remove(oldDb);

  saveList();
}

void DbManagerPrivate::terminate()
{
  closeAll();
}

void DbManagerPrivate::toggle(QSqlDatabase *db)
{
  if(db->isOpen())
    close(db);
  else
  {
    open(db);
    DbManager::lastIndex = indexOf(db);
  }
}

void DbManagerPrivate::update(QSqlDatabase *db)
{
  if(dbList.contains(db))
    refreshModelItem(db);
}

/*
 * DbManager
 */

// intialize static members
DbManagerPrivate   *DbManager::m_instance = new DbManagerPrivate();
int                 DbManager::lastIndex = 0;

int DbManager::addDatabase(QString driver, QString host, QString user,
                           QString pswd, QString dbnm)
{
  return m_instance->addDatabase(driver, host, user, pswd, dbnm);
}

int DbManager::addDatabase(QString driver, QString host, QString user,
                           QString dbnm)
{
  return addDatabase(driver, host, user, QString::null, dbnm);
}

void DbManager::close(QSqlDatabase *db)
{
  m_instance->close(db);
}

void DbManager::closeAll()
{
  m_instance->closeAll();
}

int DbManager::defaultPort(QString driver)
{
  if (driver == "QMYSQL")
    return 3306;

  return -1;
}

QStandardItemModel *DbManager::driverModel()
{
  return m_instance->driverModel();
}

QString DbManager::genConnectionName()
{
  return m_instance->genConnectionName();
}

QSqlDatabase* DbManager::getDatabase(int n)
{
  return m_instance->getDatabase(n);
}

QList<QSqlDatabase*> DbManager::getDbList()
{
  return m_instance->getDbList();
}

QStringList DbManager::getDbNames(bool showHosts)
{
  return m_instance->getDbNames(showHosts);
}

/// DEPRECATED
int DbManager::indexOf(QSqlDatabase *db)
{
  return m_instance->indexOf(db);
}

void DbManager::init()
{
  m_instance->init();
}

QString DbManager::lastError()
{
  return m_instance->lastError();
}

void DbManager::open(int nb, QString pswd)
{
  m_instance->open(nb, pswd);
}

void DbManager::open(QSqlDatabase *db, QString pswd)
{
  m_instance->open(db, pswd);
}

void DbManager::openList()
{
  m_instance->openList();
}

void DbManager::removeDatabase(int index)
{
  m_instance->removeDatabase(index);
}

void DbManager::saveList()
{
  m_instance->saveList();
}
void DbManager::setDatabase(int nb, QSqlDatabase db)
{
  m_instance->setDatabase(nb, db);
}

void DbManager::terminate()
{
  m_instance->terminate();
}

void DbManager::toggle(QSqlDatabase *db)
{
  m_instance->toggle(db);
}

void DbManager::update(QSqlDatabase *db)
{
  m_instance->update(db);
}
