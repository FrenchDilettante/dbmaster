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


#include "pluginmanager.h"
#include "exportengine.h"
#include "sqlwrapper.h"

/*
 * PluginManagerPrivate
 */

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

  foreach (Plugin *p, m_plugins) {
    SqlWrapper *w = NULL;
    if (checkType(p, "WRAPPER") && ((SqlWrapper*) p)->driver() == driver) {
      return w;
    }
  }

  return NULL;
}

/**
 * Vérifie le type d'un plugin : moteur d'export, adaptateur, etc.
 */
bool PluginManagerPrivate::checkType(Plugin *p, QString type) {
  return (p && p->plid().count(".") == 2 && p->plid().split('.')[2] == type);
}

/**
 * Extrait les moteurs d'export
 */
QList<ExportEngine*> PluginManagerPrivate::exportEngines() {
  QList<ExportEngine*> engines;

  foreach (Plugin *p, m_plugins) {
    if (checkType(p, "EXPORTENGINE")) {
      engines << (ExportEngine*) p;
    }
  }

  return engines;
}

void PluginManagerPrivate::init() {

  // Grâce au filtre, on va lister les fichiers présents au démarrage
  QString filter;
#ifdef Q_OS_LINUX
  filter = "*.so";
#else
  filter = "*.dll";
#endif

  QFileInfoList pluginsInFolder;
#ifdef Q_OS_LINUX
  pluginsInFolder = QDir().entryInfoList(QStringList(filter));
#else
  pluginsInFolder = QDir("plugins").entryInfoList(QStringList(filter));
#endif

  // On trie sur le volet les plugins qui ne sont pas enregistrés
  foreach (QFileInfo f, pluginsInFolder) {
    Plugin *p = load(f);
    if (p) {
      registerPlugin(p);
    }
  }
}

void PluginManagerPrivate::registerPlugin(Plugin *plugin) {
  foreach (Plugin *p, m_plugins) {
    if (plugin->plid() == p->plid()) {
      QMessageBox::warning(NULL,
                           tr("Add a plugin"),
                           tr("This plugin is already registered"));
      return;
    }
  }

  QStandardItem *item = new QStandardItem();
  item->setText(plugin->title());
  item->setData(plugin->plid(), Qt::UserRole);

  QString type = tr("Other");

  if (checkType(plugin, "EXPORTENGINE")) {
    type = tr("Export engine");
  } else if (checkType(plugin, "WRAPPER")) {
    type = tr("SQL Wrapper");
  }

  QList<QStandardItem*> l;
  l << item;
  l << new QStandardItem(type);
  l << new QStandardItem(plugin->vendor());
  l << new QStandardItem(plugin->version());
  m_plugins << plugin;
  foreach (QStandardItem *i, l) {
    i->setEditable(false);
  }

  pluginsMap[plugin] = item;
  m_model->appendRow(l);
}

Plugin *PluginManagerPrivate::load(QFileInfo info) {
  Plugin *p = NULL;
  QPluginLoader loader(info.absoluteFilePath());
  if(loader.load())   {
#if QT_VERSION >= 0x040700
    p = dynamic_cast<Plugin*>(loader.instance());
#else
    p = (Plugin*) loader.instance();
#endif
    if(!p) {
      QMessageBox::critical(NULL,
                            tr("Incorrect plugin file"),
                            tr("This file contains no DbMaster plugin."));
    }
  } else {
    QMessageBox::critical(NULL,
                          tr("Incorrect plugin file"),
                          tr("Unable to load the plugin. Specified error : \n")
                            + loader.errorString());
  }

  return p;
}

Plugin *PluginManagerPrivate::plugin(QString plid) {
  if (plid.size() == 0) {
    return NULL;
  }
  foreach (Plugin *p, m_plugins) {
    if (p->plid() == plid) {
      return p;
    }
  }

  return NULL;
}

void PluginManagerPrivate::save() {
  QSettings s;
  s.beginGroup("plugins");
  s.beginWriteArray("list");
  for (int i=0; i<m_model->rowCount(); i++) {
    s.setArrayIndex(i);
  }
  s.endArray();
  s.endGroup();
}

SqlWrapper* PluginManagerPrivate::wrapper(QString plid) {
  Plugin *p = plugin(plid);
  if (p && checkType(p, "WRAPPER")) {
    return (SqlWrapper*) p;
  } else {
    return NULL;
  }
}

QList<SqlWrapper*> PluginManagerPrivate::wrappers() {
  QList<SqlWrapper*> wrappers;

  foreach (Plugin *p, m_plugins) {
    SqlWrapper *wp = NULL;
    if (checkType(wp, "WRAPPER")) {
      wrappers << (SqlWrapper*) wp;
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
  return instance->plugin(plid);
}

QString PluginManager::pluginDirectory() {
  return QDir::homePath();
}

void PluginManager::registerPlugin(Plugin *p) {
  instance->registerPlugin(p);
}

void PluginManager::save() {
  instance->save();
}

SqlWrapper* PluginManager::wrapper(QString plid) {
  return instance->wrapper(plid);
}

QList<SqlWrapper*> PluginManager::wrappers() {
  return instance->wrappers();
}
