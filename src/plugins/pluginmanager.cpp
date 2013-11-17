#include "pluginmanager.h"
#include "exportengine.h"
#include "sqlwrapper.h"

#include "exportengines/csv/csvexportengine.h"
#include "exportengines/html/htmlexportengine.h"
#include "exportengines/plaintext/plaintextexportengine.h"

#include "wrappers/db2i/db2iwrapper.h"
#include "wrappers/mysql/mysqlwrapper.h"
#include "wrappers/psql/psqlwrapper.h"
#include "wrappers/sqlite/sqlitewrapper.h"

#include <QMessageBox>

/*
 * PluginManagerPrivate
 */

Plugin* PluginManagerPrivate::toPlugin(QObject *pl) {
  Plugin *p = qobject_cast<Plugin*>(pl);
  if (!p) {
    p = qobject_cast<ExportEngine*>(pl);
  }
  if (!p) {
    p = qobject_cast<SqlWrapper*>(pl);
  }

  return p;
}

PluginManagerPrivate::PluginManagerPrivate()
  : QObject() {
  m_model = new QStandardItemModel(this);
  QStringList labels;
  labels << tr("Plugin") << tr("Type") << tr("Vendor") << tr("Version");
  m_model->setHorizontalHeaderLabels(labels);

  init();
}

/**
 * Extrait le wrapper disponible pour le driver spécifié
 */
SqlWrapper* PluginManagerPrivate::availableWrapper(QString driver) {
  if (driver == "QODBC") {
    return NULL;
  }

  foreach (QObject *p, m_plugins) {
    SqlWrapper *w = qobject_cast<SqlWrapper*>(p);
    if (w && w->driver() == driver) {
      return w;
    }
  }

  return NULL;
}

/**
 * Extrait les moteurs d'export
 */
QList<ExportEngine*> PluginManagerPrivate::exportEngines() {
  QList<ExportEngine*> engines;

  foreach (QObject *p, m_plugins) {
    ExportEngine *e = qobject_cast<ExportEngine*>(p);
    if (e) {
      engines << e;
    }
  }

  return engines;
}

void PluginManagerPrivate::init() {
  registerPlugin(new CsvExportEngine());
  registerPlugin(new HtmlExportEngine());
  // registerPlugin(new PlainTextExportEngine());

  registerPlugin(new Db2iWrapper());
  registerPlugin(new MysqlWrapper());
  registerPlugin(new PsqlWrapper());
  registerPlugin(new SqliteWrapper());
}

void PluginManagerPrivate::registerPlugin(QObject *plugin) {
  Plugin *pl = toPlugin(plugin);

  QStandardItem *item = new QStandardItem();
  item->setText(pl->title());
  item->setData(pl->plid(), Qt::UserRole);

  QString type = tr("Other");

  if (qobject_cast<ExportEngine*>(plugin)) {
    type = tr("Export engine");
  } else if (qobject_cast<SqlWrapper*>(plugin)) {
    type = tr("SQL Wrapper");
  }

  QList<QStandardItem*> l;
  l << item;
  l << new QStandardItem(type);
  l << new QStandardItem(pl->vendor());
  l << new QStandardItem(pl->version());
  m_plugins << plugin;
  foreach (QStandardItem *i, l) {
    i->setEditable(false);
  }

  pluginsMap[plugin] = item;
  m_model->appendRow(l);
}

QObject *PluginManagerPrivate::plugin(QString plid) {
  if (plid.size() == 0) {
    return NULL;
  }
  foreach (QObject *p, m_plugins) {
    if (toPlugin(p)->plid() == plid) {
      return p;
    }
  }

  return NULL;
}

SqlWrapper* PluginManagerPrivate::wrapper(QString plid) {
  QObject *p = plugin(plid);
  SqlWrapper *w = p ? qobject_cast<SqlWrapper*>(p) : NULL;

  return w;
}

QList<SqlWrapper*> PluginManagerPrivate::wrappers() {
  QList<SqlWrapper*> wrappers;

  foreach (QObject *p, m_plugins) {
    SqlWrapper *wp = qobject_cast<SqlWrapper*>(p);
    if (wp) {
      wrappers << wp;
    }
  }

  return wrappers;
}

/*
 * PluginManager
 */

PluginManagerPrivate *PluginManager::instance = NULL;

SqlWrapper* PluginManager::availableWrapper(QString driver) {
  return instance->availableWrapper(driver);
}

QList<ExportEngine*> PluginManager::exportEngines() {
  return instance->exportEngines();
}

void PluginManager::init() {
  instance = new PluginManagerPrivate();
}

Plugin* PluginManager::plugin(QString plid) {
  return PluginManagerPrivate::toPlugin(instance->plugin(plid));
}

QObject* PluginManager::pluginObject(QString plid) {
  return instance->plugin(plid);
}

SqlWrapper* PluginManager::wrapper(QString plid) {
  return instance->wrapper(plid);
}

QList<SqlWrapper*> PluginManager::wrappers() {
  return instance->wrappers();
}
