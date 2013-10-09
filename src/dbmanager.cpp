#include "dbmanager.h"
#include "db_enum.h"
#include "iconmanager.h"
#include "tools/logger.h"
#include "mainwindow.h"

#include "plugins/pluginmanager.h"
#include "tabwidget/abstracttabwidget.h"

DbManager* DbManager::instance;

DbManager::DbManager()
  : QThread() {
  closingAll = false;
  nconn = 0;
  m_driverModel = new QStandardItemModel(this);
  m_model = new QStandardItemModel(this);

  setupConnections();
  setupModels();
  openList();
}

int DbManager::addDatabase(QString driver, QString host, QString user,
                                  QString pswd, QString dbnm, QString alias,
                                  bool usesOdbc, bool save) {
  SqlWrapper *wrapper = PluginManager::availableWrapper(driver);
  if (usesOdbc) {
    driver = "QODBC";
  }

  // need to store what plugin what used
  QString plid = wrapper ? wrapper->plid() : "";
  return addDatabase(driver, host, user, pswd, dbnm, alias, plid, save);
}

/**
 * @brief DbManager::addDatabase
 * @param driver
 * @param host
 * @param user
 * @param pswd
 * @param dbnm
 * @param alias
 * @param wrapperName
 * @param save
 * @return the index of the added db OR if the db already exists, its index
 */
int DbManager::addDatabase(QString driver, QString host, QString user,
                                  QString pswd, QString dbnm, QString alias,
                                  QString wrapperName, bool save) {
  QSqlDatabase db = QSqlDatabase::addDatabase(driver, genConnectionName());
  db.setHostName(host);
  db.setUserName(user);
  db.setPassword(pswd);
  db.setDatabaseName(dbnm);

  foreach (QSqlDatabase *d, dbList) {
    // checking for doubles
    if (d->hostName() == db.hostName() &&
            d->userName() == db.userName() &&
            d->password() == db.password() &&
            d->databaseName() == db.databaseName()) {
      return indexOf(d);
    }
  }

  QSqlDatabase *newDb = new QSqlDatabase(db);
  dbList.append(newDb);

  QString title;
  if (alias.isEmpty()) {
    title = dbTitle(newDb);
  } else {
    title = alias;
  }

  dbMap[newDb] = new QStandardItem(title);
  dbMap[newDb]->setEditable(false);

  dbMap[newDb]->setData(DbManager::DbItem, Qt::UserRole);
  dbMap[newDb]->setIcon(IconManager::get("database_connect"));
  dbMap[newDb]->setToolTip(dbToolTip(newDb));

  QList<QStandardItem*> uiItemList;
  // name
  uiItemList << dbMap[newDb];
  // icon (= DBMS icon)
  uiItemList << new QStandardItem(driverIcon[driver], "");
  uiItemList[1]->setSelectable(false);

  m_model->appendRow(uiItemList);

  SqlWrapper *wrapper = NULL;
  if (wrapperName.length() > 0) {
    wrapper = PluginManager::wrapper(wrapperName);
  } else if (!driver.startsWith("QODBC")) {
    wrapper = PluginManager::availableWrapper(driver);
  }

  if (wrapper) {
    dbWrappers[newDb] = wrapper->newInstance(newDb);
  }

  if (save) {
    saveList();
  }

  return dbList.size() - 1;
}

QString DbManager::alias(QSqlDatabase *db) {
  if (dbMap.contains(db)) {
    return dbMap[db]->text();
  } else {
    return "";
  }
}

void DbManager::close(QSqlDatabase* db) {
  if (!dbList.contains(db))
    return;

  closeStack.push(db);

  if (!isRunning())
    start();
}

QStandardItem* DbManager::columnsItem(QList<SqlColumn> columns) {
  QStandardItem *cItem =
      new QStandardItem(IconManager::get("folder_columns"),
                        tr("Columns (%1)")
                        .arg(columns.size()));

  foreach (SqlColumn c, columns) {
    QStandardItem *i = new QStandardItem();
    i->setText(c.name);
    if (c.primaryKey) {
      i->setIcon(IconManager::get("column_key"));
    } else {
      i->setIcon(IconManager::get("column"));
    }
    cItem->appendRow(i);
  }

  return cItem;
}

QString DbManager::dbTitle(QSqlDatabase *db) {
  QString title;
  QString simplifiedName = QFileInfo(db->databaseName()).fileName();
  if (simplifiedName.length() == 0) {
    simplifiedName = db->databaseName();
  }
  if(db->hostName().isEmpty())
    title = tr("%1 (local)").arg(simplifiedName);
  else
    title = tr("%1 on %2").arg(simplifiedName).arg(db->hostName());

  return title;
}

/**
 * Returns a rich-texted tooltip for the database.
 */
QString DbManager::dbToolTip(QSqlDatabase *db) {
  QString ret;

  ret += tr("%1 on %2").prepend("<h3>").append("</h3>")
         .arg(db->databaseName())
         .arg(db->hostName());

  ret += "<table border=\"0\">";

  QString defaultLine = "<tr><td><b>%1</b></td><td>%2</td></tr>";

  // Connection's status
  QString stName;
  if (db->isOpen()) {
    stName = tr("Openned");
  } else {
    stName = tr("Closed");
  }
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
void DbManager::closeAll()
{
  if (dbMap.size() == 0) {
    return;
  }

  closingAll = true;
  closeStack << dbList.toVector();
  dbMap.clear();
}

QStandardItemModel *DbManager::driverModel() {
  return m_driverModel;
}

QString DbManager::genConnectionName() {
  nconn++;
  return QString::number(nconn);
}

QSqlDatabase* DbManager::getDatabase(int n) {
  if (dbMap.size() == 0) {
    return NULL;
  }

  if (n < 0) {
    return dbList.first();
  }

  if (n >= dbMap.size()) {
    return dbList.last();
  }

  return dbList[n];
}

QList<QSqlDatabase*> DbManager::getDbList() {
  return dbList;
}

QStringList DbManager::getDbNames(bool showHosts) {
  QStringList ret;
  QString str;

  foreach(QSqlDatabase *db, dbList)   {
    str = db->databaseName();
    if (str.contains("/")) {
      str = str.split("/")[str.split("/").size() - 1];
    }
    if (showHosts && !db->hostName().isEmpty()) {
      str.append(" on ").append(db->hostName());
    }

    ret << str;
  }

  return ret;
}

/// DEPRECATED
int DbManager::indexOf(QSqlDatabase *db) {
  return dbList.indexOf(db);
}

void DbManager::init() {
  instance = new DbManager();
}

QString DbManager::lastError() {
  return lastErr;
}

void DbManager::open(int nb, QString pswd) {
  open(dbList[nb], pswd);
}

void DbManager::open(QSqlDatabase *db, QString pswd) {
  if (!dbMap.contains(db)) {
    return;
  }

  if (!pswd.isNull()) {
    db->setPassword(pswd);
  }

  dbMap[db]->setIcon(IconManager::get("database_lightning"));

  openStack.push(db);
  while (closeStack.contains(db)) {
    closeStack.remove(closeStack.indexOf(db));
  }

  if (!isRunning()) {
    start();
  }
}

void DbManager::openList() {
  QSettings s;

  int size = s.beginReadArray("dblist");
  QString driver, host, user, pswd, name, alias, wrapper;
  for (int i = 1; i <= size; i++) {
    s.setArrayIndex(i);
    driver = s.value("driver").toString();
    host = s.value("hostname").toString();
    user = s.value("username").toString();
    name = s.value("database").toString();
    pswd = s.value("password", QVariant(QString::null)).toString();
    alias = s.value("alias", "").toString();
    wrapper = s.value("wrapper", "").toString();
    addDatabase(driver, host, user, pswd, name, alias, wrapper, false);
  }
  s.endArray();
}

QSqlDatabase *DbManager::parentDb(QModelIndex index) {
  while (index != QModelIndex()) {
    if (index.data(Qt::UserRole) == DbManager::DbItem) {
      return DbManager::getDatabase(index.row());
    }
    index = index.parent();
  }

  return NULL;
}

void DbManager::refreshModel() {
  foreach (QSqlDatabase *db, dbList) {
    refreshModelItem(db);
  }
}

void DbManager::refreshModelIndex(QModelIndex index) {
  if (!index.data(Qt::UserRole).canConvert(QVariant::Int)) {
    return;
  }

  QStandardItem *it = m_model->itemFromIndex(index);
  if (!it) {
    return;
  }

  SqlWrapper *wrapper = dbWrappers.value(parentDb(index), NULL);
  if (!wrapper) {
    return;
  }

  QList<QStandardItem*> toAppend;
  switch (index.data(Qt::UserRole).toInt()) {
  case DbManager::SchemaItem:
    toAppend << tablesItem(wrapper->tables(index.data().toString()),
                           index.data().toString());
    break;

  case DbManager::TableItem:
    toAppend << columnsItem(wrapper->columns(index.data().toString()));
    break;

  default:
    break;
  }

  while (it->rowCount() > 0) {
    it->removeRow(0);
  }

  it->appendRows(toAppend);
}

/**
 * @bug check indexes
 */
void DbManager::refreshModelItem(QSqlDatabase *db) {
  if (!dbMap.contains(db)) {
    return;
  }

  QStandardItem *item = dbMap[db];
  item->setIcon(IconManager::get("database_refresh"));
  item->setToolTip(dbToolTip(db));

  QStandardItem *schemaItem = NULL;

  QModelIndex index = m_model->indexFromItem(item);
  if (db->isOpen()) {
    while (m_model->rowCount(index) > 0) {
      m_model->removeRow(0, index);
    }

    SqlWrapper *wrapper = dbWrappers.value(db, NULL);

    if (wrapper) {
      if (wrapper->features().testFlag(SqlWrapper::Schemas)) {
        QList<SqlSchema> schemas = wrapper->schemas();

        schemaItem = new QStandardItem(tr("Schemas (%1)")
                                       .arg(schemas.size()));
        schemaItem->setIcon(IconManager::get("folder_schemas"));

        foreach (SqlSchema s, schemas) {
          schemaItem->appendRow(this->schemaItem(s));
        }

        item->appendRow(schemaItem);
      } else {
        QList<SqlTable> tables = wrapper->tables();

        item->appendRow(tablesItem(tables));
        item->appendRow(viewsItem(tables));
      }
    } else {
      QList<SqlTable> tables;
      foreach (QString s, db->tables(QSql::Tables)) {
        SqlTable t;
        t.name = s;
        t.type = Table;
        tables << t;
      }

      item->appendRow(tablesItem(tables));

      QList<SqlTable> views;
      foreach (QString s, db->tables(QSql::Views)) {
        SqlTable t;
        t.name = s;
        t.type = ViewTable;
        views << t;
      }

      item->appendRow(viewsItem(views));
    }

    item->setIcon(IconManager::get("database"));

  } else {
    item->setIcon(IconManager::get("database_connect"));
    while (m_model->rowCount(index) > 0) {
      m_model->removeRow(0, index);
    }
  }
}

/**
 * Remove the database at index.
 */
void DbManager::removeDatabase(int index) {
  removeDatabase(dbList[index]);
}

void DbManager::removeDatabase(QSqlDatabase *db) {
  if (!dbMap.contains(db)) {
    return;
  }

  close(db);
  m_model->removeRow(dbList.indexOf(db));
  dbMap.remove(db);
  dbList.removeAll(db);
  saveList();

  if (!isRunning()) {
    start();
  }
}

/**
 * Connexion pool thread.
 * @todo find a better way to proceed...
 */
void DbManager::run() {
  QSqlDatabase *db;

  while (dbList.size() > 0) {
    // connections to open stack
    while (openStack.size() > 0) {
      // check for doubles
      foreach (QSqlDatabase *d, closeStack) {
        while (openStack.contains(d)) {
          openStack.remove(openStack.indexOf(d));
        }
      }

      db = openStack.pop();
      if (db->open()) {
        Logger::instance->log(tr("Connected to %1").arg(alias(db)));
      } else {
        Logger::instance->log(tr("Unable to connect to %1").arg(alias(db)));
      }

      emit statusChanged(db);
      emit statusChanged(dbMap[db]->index());
    }

    // connections to close stack
    while (closeStack.size() > 0) {
      db = closeStack.pop();
      db->close();

      Logger::instance->log(tr("Disconnected from %1").arg(alias(db)));

      // don't emit signal when closing the application
      if (!closingAll) {
        emit statusChanged(db);
        if (dbMap.contains(db)) {
          emit statusChanged(dbMap[db]->index());
        }
      }

      if (!dbList.contains(db)) {
        QSqlDatabase::removeDatabase(db->connectionName());
      }
    }

    msleep(10);
  }
}

void DbManager::saveList() {
  QSettings s;
  s.beginWriteArray("dblist", dbMap.size());

  int i=0;
  foreach (QSqlDatabase *db, dbList) {
    i++;
    s.setArrayIndex(i);
    s.setValue("driver", db->driverName());
    s.setValue("hostname", db->hostName());
    s.setValue("username", db->userName());
    if (!db->password().isNull()) {
      s.setValue("password", db->password());
    }
    s.setValue("database", db->databaseName());
    s.setValue("alias", dbMap[db]->text());
    if (dbWrappers.contains(db) && dbWrappers[db]) {
      s.setValue("wrapper", dbWrappers[db]->plid());
    }
  }
  s.endArray();
  s.sync();
}

SqlSchema DbManager::schema(QSqlDatabase *db, QString schemaName) {
  if (!dbWrappers[db] || !(dbWrappers[db]->features() & SqlWrapper::Schemas)) {
    return SqlSchema();
  }

  SqlSchema schema = dbWrappers[db]->schema(schemaName);

  return schema;
}

QStandardItem* DbManager::schemaItem(SqlSchema schema) {
  QStandardItem *sitem = new QStandardItem(schema.name);
  sitem->setIcon(IconManager::get("schema"));
  sitem->setData(schema.name, Qt::ToolTipRole);
  sitem->setData(DbManager::SchemaItem, Qt::UserRole);

  QString prefix;
  if (!schema.defaultSchema) {
    prefix = schema.name;
  }

  sitem->appendRow(new QStandardItem(IconManager::get("view-refresh"), ""));

  return sitem;
}

void DbManager::setAlias(QSqlDatabase *db, QString alias) {
  if (!dbList.contains(db)) {
    return;
  }

  dbMap[db]->setText(alias);
  saveList();
}

void DbManager::setDatabase(int nb, QSqlDatabase db) {
  if (dbList.size() >= nb) {
    return;
  }

  QSqlDatabase *newDb = new QSqlDatabase(db);
  QSqlDatabase *oldDb = dbList[nb];

  swapDatabase(oldDb, newDb);
}

void DbManager::setupConnections() {
  connect(this, SIGNAL(statusChanged(QSqlDatabase*)),
          this, SLOT(refreshModelItem(QSqlDatabase*)));
}

void DbManager::setupModels() {
  m_driverModel->setHorizontalHeaderItem(0, new QStandardItem(tr("Driver")));

  driverAlias["QDB2"]     = tr("DB2");
  driverAlias["QIBASE"]   = tr("Interbase/Firebird");
  driverAlias["QMYSQL"]   = tr("MySql");
  driverAlias["QMYSQL3"]  = tr("MySql 3");
  driverAlias["QOCI"]     = tr("Oracle");
  driverAlias["QOCI8"]    = tr("Oracle 8");
  driverAlias["QODBC"]    = tr("Generic (ODBC)");
  driverAlias["QODBC3"]   = tr("Generic (ODBC 3)");
  driverAlias["QPSQL"]    = tr("PostGreSQL");
  driverAlias["QPSQL7"]   = tr("PostGreSQL 7");
  driverAlias["QSQLITE"]  = tr("SQLite");
  driverAlias["QSQLITE2"] = tr("SQLite 2");

  driverIcon["QDB2"]      = QIcon(":/img/db_db2.png");
  driverIcon["QIBASE"]    = QIcon(":/img/db_firebird.png");
  driverIcon["QMYSQL"]    = QIcon(":/img/db_mysql.png");
  driverIcon["QPSQL"]     = QIcon(":/img/db_postgresql.png");
  driverIcon["QSQLITE"]   = QIcon(":/img/db_sqlite.png");

  QStandardItem *item;
  foreach (QString driver, QSqlDatabase::drivers()) {
    item = new QStandardItem();
    item->setData(driver, Qt::UserRole);
    item->setText(driverAlias.value(driver, driver));
    item->setIcon(driverIcon.value(driver, QIcon()));

    m_driverModel->appendRow(item);
  }

  foreach (SqlWrapper *w, PluginManager::wrappers()) {
    if (w->driver() == "QODBC") {
      item = new QStandardItem();
      item->setData("QODBC", Qt::UserRole);
      item->setData(w->plid(), Qt::UserRole + 1);
      item->setText(w->driverName());
      item->setIcon(IconManager::get(w->driverIconCode()));

      m_driverModel->appendRow(item);
    }
  }

  m_driverModel->sort(0);

  m_model->setHorizontalHeaderItem(0, new QStandardItem(tr("Database")));
}

void DbManager::swapDatabase(QSqlDatabase *oldDb, QSqlDatabase *newDb) {
  if (!dbMap.contains(oldDb)) {
    return;
  }

  QStandardItem *item = dbMap[oldDb];
  item->setText(QObject::tr("%1 on %2")
          .arg(newDb->databaseName())
          .arg(newDb->hostName()));

  oldDb->close();
  dbMap[newDb] = item;
  dbMap.remove(oldDb);

  saveList();
}

SqlTable DbManager::table(QSqlDatabase *db, QString tbl) {
  SqlTable table;

  if (dbWrappers[db]) {
    table = dbWrappers[db]->table(tbl);
  } else {
    QSqlRecord r = db->record(tbl);
    for (int i=0; i<r.count(); i++) {
      SqlColumn c;
      c.permitsNull = false;
      c.primaryKey = false;
      c.name = r.fieldName(i);
      table.columns << c;
    }
  }

  return table;
}

QStandardItem *DbManager::tablesItem(QList<SqlTable> tables,
                                            QString schema) {
  QStandardItem *tablesItem = new QStandardItem();
  tablesItem->setIcon(IconManager::get("folder_tables"));

  foreach (SqlTable table, tables) {
    if (table.type == Table) {
      QStandardItem *i = new QStandardItem(IconManager::get("table"),
                                           table.name);
      i->setData(DbManager::TableItem, Qt::UserRole);
      if (schema.length() == 0) {
        i->setData(table.name, Qt::ToolTipRole);
      } else {
        i->setData(QString(schema + "." + table.name), Qt::ToolTipRole);
      }
//      i->appendRow(new QStandardItem(IconManager::get("view-refresh"), ""));
      if (table.columns.size() > 0) {
        i->appendRow(columnsItem(table.columns));
      }
      tablesItem->appendRow(i);
    }
  }

  tablesItem->setText(tr("Tables (%1)")
                      .arg(tablesItem->rowCount()));

  return tablesItem;
}

QStandardItem *DbManager::viewsItem(QList<SqlTable> tables,
                                           QString schema) {
  QStandardItem *viewsItem = new QStandardItem();

  viewsItem->setIcon(IconManager::get("folder_views"));

  foreach (SqlTable table, tables) {
    if (table.type == ViewTable) {
      QStandardItem *i = new QStandardItem(IconManager::get("table_lightning"),
                                           table.name);
      i->setData(DbManager::ViewItem, Qt::UserRole);
      if (schema.length() == 0) {
        i->setData(table.name, Qt::ToolTipRole);
      } else {
        i->setData(QString(schema + "." + table.name), Qt::ToolTipRole);
      }
      viewsItem->appendRow(i);
    }
  }

  viewsItem->setText(tr("Views (%1)").arg(viewsItem->rowCount()));

  return viewsItem;
}

void DbManager::terminate() {
  closeAll();
}

void DbManager::toggle(QSqlDatabase *db) {
  if(db->isOpen()) {
    close(db);
  } else {
    open(db);
    DbManager::lastUsedDbIndex = indexOf(db);
  }
}

void DbManager::update(QSqlDatabase *db, QString alias) {
  if (dbList.contains(db)) {
    dbMap[db]->setText(alias);
    refreshModelItem(db);

    saveList();
  }
}
