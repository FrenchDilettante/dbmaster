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
#include "db_enum.h"
#include "iconmanager.h"
#include "mainwindow.h"

#include "dialogs/logdialog.h"
#include "plugins/pluginmanager.h"
#include "tabwidget/abstracttabwidget.h"

DbManagerPrivate::DbManagerPrivate()
  : QObject() {
  closingAll = false;
  nconn = 0;
  m_driverModel = new QStandardItemModel(this);
  m_model = new QStandardItemModel(this);
}

/**
 * Ajoute une base au pool de connexions.
 *
 * @param driver
 *    Voir doc Qt
 * @param host
 *    Hôte distant ou local
 * @param user
 *    Nom d'utilisateur
 * @param pswd
 *    Mot de passe
 * @param dbnm
 *    Base de données
 * @param alias
 *    Nom à afficher
 * @param usesOdbc
 *    Utilise (ou non) ODBC
 * @param save
 *    Déclenche l'enregistrement de la liste ou non
 */
int DbManagerPrivate::addDatabase(QString driver, QString host, QString user,
                                  QString pswd, QString dbnm, QString alias,
                                  bool usesOdbc, bool save) {
  // On détermine si un adaptateur est disponible...
  SqlWrapper *wrapper = PluginManager::availableWrapper(driver);
  // ... avant d'inscrire le driver en ODBC (voir assistant d'ajout)
  if (usesOdbc) {
    driver = "QODBC";
  }

  QString plid = wrapper ? wrapper->plid() : "";

  // On continue avec la procédure classique
  return addDatabase(driver, host, user, pswd, dbnm, alias, plid, save);
}

/**
 * Ajoute une base au pool de connexions.
 *
 * @param driver
 *    Voir doc Qt
 * @param host
 *    Hôte distant ou local
 * @param user
 *    Nom d'utilisateur
 * @param pswd
 *    Mot de passe
 * @param dbnm
 *    Base de données
 * @param alias
 *    Nom à afficher
 * @param wrapper
 *    Nom (PLID) de l'adaptateur SQL à utiliser
 * @param save
 *    Déclenche l'enregistrement de la liste ou non
 */
int DbManagerPrivate::addDatabase(QString driver, QString host, QString user,
                                  QString pswd, QString dbnm, QString alias,
                                  QString wrapper, bool save) {
  QSqlDatabase db = QSqlDatabase::addDatabase(driver, genConnectionName());
  db.setHostName(host);
  db.setUserName(user);
  db.setPassword(pswd);
  db.setDatabaseName(dbnm);

  for (int i=0; i<m_connections.size(); i++) {
    QSqlDatabase d = m_connections[i].db;

    // on contrôle les éventuels doublons
    if (d.hostName() == db.hostName() &&
            d.userName() == db.userName() &&
            d.password() == db.password() &&
            d.databaseName() == db.databaseName()) {
      LogDialog::instance()->append(
              QObject::tr("Attempting to add an existing connection."));
      return i;
    }
  }

  Connection co;
  co.db = db;
  co.alias = dbTitle(db);
  co.status = Closed;
  co.item = new QStandardItem(co.alias);
  co.item->setEditable(false);

  co.item->setData(DbManager::DbItem, Qt::UserRole);
  co.item->setIcon(IconManager::get("database_connect"));
  co.item->setToolTip(dbToolTip(co.db));

  QList<QStandardItem*> l;
  l << co.item;
  l << new QStandardItem(driverIcon[driver], "");
  l[1]->setSelectable(false);

  m_model->appendRow(l);

  SqlWrapper *w = NULL;
  if (wrapper.length() > 0) {
    w = PluginManager::wrapper(wrapper);
  } else if (!driver.startsWith("QODBC")) {
    w = PluginManager::availableWrapper(driver);
  }

  if (w) {
    /// @todo reprendre les Wrapper
    co.wrapper = w->newInstance(&co.db);
  }

  m_connections << co;

  if (save) {
    saveList();
    LogDialog::instance()->append(QObject::tr("Connection %1 added")
            .arg(co.alias));
  }

  return m_connections.size() - 1;
}

QString DbManagerPrivate::alias(int idx) {
  checkDbIndex(idx);

  return m_connections[idx].alias;
}

void DbManagerPrivate::checkDbIndex(int idx) {
  if (idx < 0 || idx >= m_connections.size()) {
    throw "Bad index";
  }
}

void DbManagerPrivate::close(int idx) {
  checkDbIndex(idx);

  m_connections[idx].item->setIcon(IconManager::get("database_lightning"));
  m_connections[idx].db.close();
  m_connections[idx].status = Closed;
  refreshModelItem(idx);
}

QStandardItem* DbManagerPrivate::columnsItem(QList<SqlColumn> columns) {
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

QString DbManagerPrivate::dbTitle(QSqlDatabase db) {
  QString title;
  QString simplifiedName = QFileInfo(db.databaseName()).fileName();
  if (simplifiedName.length() == 0) {
    simplifiedName = db.databaseName();
  }
  if(db.hostName().isEmpty())
    title = tr("%1 (local)").arg(simplifiedName);
  else
    title = tr("%1 on %2").arg(simplifiedName).arg(db.hostName());

  return title;
}

/**
 * Returns a rich-texted tooltip for the database.
 */
QString DbManagerPrivate::dbToolTip(QSqlDatabase db) {
  QString ret;

  ret += tr("%1 on %2").prepend("<h3>").append("</h3>")
         .arg(db.databaseName())
         .arg(db.hostName());

  ret += "<table border=\"0\">";

  QString defaultLine = "<tr><td><b>%1</b></td><td>%2</td></tr>";

  // Connection's status
  QString stName;
  if(db.isOpen())
    stName = tr("Openned");
  else
    stName = tr("Closed");
  ret += defaultLine
         .arg(tr("Status"))
         .arg(stName);

  // Db driver
  ret += defaultLine
         .arg(tr("DBMS"))
         .arg(driverAlias.value(db.driverName(),
                                tr("Generic (%1)").arg(db.driverName())));

  // Host
  ret += defaultLine
         .arg(tr("Host"))
         .arg(db.hostName());

  ret += "</table>";
  return ret;
}

/**
 * Closes DbManager.
 */
void DbManagerPrivate::closeAll() {
  foreach (Connection c, m_connections) {
    c.db.close();
  }
}

/**
 * Créé une nouvelle connexion pour un besoin temporaire.
 */
QSqlDatabase DbManagerPrivate::db(int idx) {
  checkDbIndex(idx);

  QString tempName = m_connections[idx].db.connectionName()
      .append("_")
      .append(QString::number(nconn++));

  return QSqlDatabase::cloneDatabase(m_connections[idx].db, tempName);
}

QStandardItemModel *DbManagerPrivate::driverModel()
{
  return m_driverModel;
}

QString DbManagerPrivate::genConnectionName() {
  return QString::number(m_connections.size());
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

void DbManagerPrivate::open(int idx, QString pswd) {
  checkDbIndex(idx);

  m_connections[idx].status = Connecting;
  m_connections[idx].item->setIcon(IconManager::get("database_lightning"));
  m_connections[idx].db.open();
  m_connections[idx].status = Open;

  refreshModelItem(idx);
}

void DbManagerPrivate::openList()
{
  QSettings s;

  int size = s.beginReadArray("dblist");
  QString driver, host, user, pswd, name, alias, wrapper;
  for (int i = 1; i <= size; i++)
  {
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

void DbManagerPrivate::refreshModel() {
  for (int i=0; i<m_connections.size(); i++) {
    refreshModelItem(i);
  }
}

void DbManagerPrivate::refreshModelIndex(QModelIndex index) {
  if (!index.data(Qt::UserRole).canConvert(QVariant::Int)) {
    return;
  }

  int idx = index.row();
  checkDbIndex(idx);

  QStandardItem *it = m_connections[idx].item;

  SqlWrapper *wrapper = m_connections[idx].wrapper;

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
void DbManagerPrivate::refreshModelItem(int idx) {
  checkDbIndex(idx);

  QStandardItem *item = m_connections[idx].item;
  item->setIcon(IconManager::get("database_refresh"));
  item->setToolTip(dbToolTip(m_connections[idx].db));

  QStandardItem *schemaItem = NULL;

  QSqlDatabase db = this->db(idx);
  db.open();

  QModelIndex index = m_model->indexFromItem(item);
  if (m_connections[idx].status == Open) {

    while (m_model->rowCount(index) > 0) {
      m_model->removeRow(0, index);
    }

    SqlWrapper *wrapper = m_connections[idx].wrapper;

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
      foreach (QString s, db.tables(QSql::Tables)) {
        SqlTable t;
        t.name = s;
        t.type = Table;
        tables << t;
      }

      item->appendRow(tablesItem(tables));

      QList<SqlTable> views;
      foreach (QString s, db.tables(QSql::Views)) {
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
    while(m_model->rowCount(index) > 0)
      m_model->removeRow(0, index);
  }
}

/**
 * Supprime la connexion à l'index
 */
void DbManagerPrivate::removeDatabase(int idx) {
  checkDbIndex(idx);

  close(idx);
  m_connections.removeAt(idx);
  m_model->removeRow(idx);
  saveList();
}

void DbManagerPrivate::saveList() {
  QSettings s;
  s.beginWriteArray("dblist", m_connections.size());

  for (int i=0; i<m_connections.size(); i++) {
    QSqlDatabase db = m_connections[i].db;
    i++;
    s.setArrayIndex(i);
    s.setValue("driver", db.driverName());
    s.setValue("hostname", db.hostName());
    s.setValue("username", db.userName());
    if (!db.password().isNull())
      s.setValue("password", db.password());
    s.setValue("database", db.databaseName());
    s.setValue("alias", m_connections[i].alias);
    if (m_connections[i].wrapper) {
      s.setValue("wrapper", m_connections[i].wrapper->plid());
    }
  }
  s.endArray();
  s.sync();
}

/**
 * Extrait les informations d'un schéma particulier
 */
SqlSchema DbManagerPrivate::schema(int idx, QString sch) {
  checkDbIndex(idx);
  SqlWrapper* w = m_connections[idx].wrapper;
  if (!w || !(w->features() & SqlWrapper::Schemas)) {
    return SqlSchema();
  }

  SqlSchema schema = w->schema(sch);

  return schema;
}

QStandardItem* DbManagerPrivate::schemaItem(SqlSchema schema) {
  QStandardItem *sitem = new QStandardItem(schema.name);
  sitem->setIcon(IconManager::get("schema"));
  sitem->setData(schema.name, Qt::ToolTipRole);
  sitem->setData(DbManager::SchemaItem, Qt::UserRole);

  QString prefix;
  if (!schema.defaultSchema) {
    prefix = schema.name;
  }

  sitem->appendRow(new QStandardItem(IconManager::get("view-refresh"), ""));

//  sitem->appendRow(tablesItem(schema.tables, prefix));
//  sitem->appendRow(viewsItem(schema.tables, prefix));

  return sitem;
}

void DbManagerPrivate::setAlias(int idx, QString alias) {
  checkDbIndex(idx);

  m_connections[idx].alias = alias;
  m_connections[idx].item->setText(alias);
  saveList();
}

void DbManagerPrivate::setDatabase(int idx, QSqlDatabase db) {
  checkDbIndex(idx);

  close(idx);
  m_connections[idx].db = db;
  refreshModelItem(idx);
}

void DbManagerPrivate::setupModels() {
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

void DbManagerPrivate::swapDatabase(int oldIdx, int newIdx) {
  checkDbIndex(oldIdx);
  checkDbIndex(newIdx);

  m_connections.swap(oldIdx, newIdx);
  refreshModel();
  saveList();
}

SqlTable DbManagerPrivate::table(int idx, QString tbl) {
  checkDbIndex(idx);

  SqlTable table;
  SqlWrapper* w = m_connections[idx].wrapper;

  if (w) {
    table = w->table(tbl);
  } else {
    QSqlDatabase db = this->db(idx);
    db.open();
    QSqlRecord r = db.record(tbl);
    for (int i=0; i<r.count(); i++) {
      SqlColumn c;
      c.permitsNull = false;
      c.primaryKey = false;
      c.name = r.fieldName(i);
      table.columns << c;
    }
    db.close();
  }

  return table;
}

QStandardItem *DbManagerPrivate::tablesItem(QList<SqlTable> tables,
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
      i->appendRow(new QStandardItem(IconManager::get("view-refresh"), ""));
//      if (table.columns.size() > 0) {
//        i->appendRow(columnsItem(table.columns));
//      }
      tablesItem->appendRow(i);
    }
  }

  tablesItem->setText(tr("Tables (%1)")
                      .arg(tablesItem->rowCount()));

  return tablesItem;
}

QStandardItem *DbManagerPrivate::viewsItem(QList<SqlTable> tables,
                                           QString schema) {
  QStandardItem *viewsItem = new QStandardItem();

  viewsItem->setIcon(IconManager::get("folder_views"));

  foreach(SqlTable table, tables)   {
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

  viewsItem->setText(tr("Views (%1)")
                     .arg(viewsItem->rowCount()));

  return viewsItem;
}

void DbManagerPrivate::terminate()
{
  closeAll();
}

void DbManagerPrivate::toggle(int idx) {
  if (idx < 0 || idx >= m_connections.size()) {
    throw "Bad index";
  }

  switch (m_connections[idx].status) {
  case Closed:
    // TODO et le mot de passe ?
    open(idx);
    break;

  case Connecting:
    // Ne devrait pas arriver...
    break;

  case Open:
    close(idx);
    break;
  }
}

void DbManagerPrivate::update(int idx, QString alias) {
  if (idx < 0 || idx >= m_connections.size()) {
    throw "Bad index";
  }

  m_connections[idx].alias = alias;
  m_connections[idx].item->setText(alias);
  refreshModelItem(idx);

  saveList();
}

/*
 * DbManager
 */

// intialize static members
DbManagerPrivate   *DbManager::m_instance = new DbManagerPrivate();
int                 DbManager::lastIndex  = 0;

int DbManager::addDatabase(QString driver, QString host, QString user,
                           QString pswd, QString dbnm, QString alias,
                           bool usesOdbc) {
  return
      m_instance->addDatabase(driver, host, user, pswd, dbnm, alias, usesOdbc);
}

int DbManager::addDatabase(QString driver, QString host, QString user,
                           QString dbnm, QString alias, bool usesOdbc) {
  return addDatabase(driver, host, user, QString::null, dbnm, alias, usesOdbc);
}

QString DbManager::alias(int idx) {
  return m_instance->alias(idx);
}

void DbManager::close(int idx) {
  m_instance->close(idx);
}

void DbManager::closeAll() {
  m_instance->closeAll();
}

QList<DbManagerPrivate::Connection> DbManager::connections() {
  return m_instance->connections();
}

QSqlDatabase DbManager::db(int idx) {
  return m_instance->db(idx);
}

QString DbManager::dbTitle(int idx) {
  return DbManagerPrivate::dbTitle(connections()[idx].db);
}

QStandardItemModel *DbManager::driverModel() {
  return m_instance->driverModel();
}

void DbManager::init() {
  m_instance->init();
}

bool DbManager::isClosed(int idx) {
  return connections()[idx].status == DbManagerPrivate::Closed;
}

bool DbManager::isOpen(int idx) {
  return connections()[idx].status == DbManagerPrivate::Open;
}

void DbManager::open(int nb, QString pswd) {
  m_instance->open(nb, pswd);
}

void DbManager::refreshModelIndex(QModelIndex index) {
  m_instance->refreshModelIndex(index);
}

void DbManager::refreshModelItem(int idx) {
  m_instance->refreshModelItem(idx);
}

void DbManager::removeDatabase(int idx) {
  m_instance->removeDatabase(idx);
}

SqlSchema DbManager::schema(int idx, QString schema) {
  return m_instance->schema(idx, schema);
}

void DbManager::setAlias(int idx, QString alias) {
  m_instance->setAlias(idx, alias);
}

SqlTable DbManager::table(int idx, QString tbl) {
  return m_instance->table(idx, tbl);
}

void DbManager::toggle(int idx) {
  m_instance->toggle(idx);
}

void DbManager::update(int idx, QString alias) {
  m_instance->update(idx, alias);
}
