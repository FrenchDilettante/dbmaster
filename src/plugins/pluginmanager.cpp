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

void PluginManagerPrivate::add(QString path)
{
  Plugin *p = load(path);
  if (p) {
    registerPlugin(p);
  }
}

/**
 * Extrait les moteurs d'export
 */
QList<ExportEngine*> PluginManagerPrivate::exportEngines() {
  QList<ExportEngine*> engines;

  foreach (Plugin *p, m_plugins) {
    ExportEngine *en = dynamic_cast<ExportEngine*>(p);
    if (en) {
      engines << en;
    }
  }

  return engines;
}

void PluginManagerPrivate::init() {

  // Grâce au filtre, on va lister les fichiers présents au démarrage
  QString filter;
#ifdef Q_OS_LINUX
  filter = "*.so";
#endif

  QStringList pluginsInFolder = QDir().entryList(QStringList(filter));
  QStringList registeredPlugins;
  QStringList unavailablePlugins;
  QList<Plugin*> unregisteredPlugins;

  // Liste des plugins enregistrés
  QSettings s;
  s.beginGroup("plugins");
  int size = s.beginReadArray("list");
  for(int i=0; i<size; i++) {
    s.setArrayIndex(i);

    registeredPlugins << s.value("path").toString();
  }
  s.endArray();
  s.endGroup();

  // On trie sur le volet les plugins qui ne sont pas enregistrés
  foreach (QString f, pluginsInFolder) {
    if (!registeredPlugins.contains(f)) {
      Plugin *p = load(f);
      if (p) {
        unregisteredPlugins << p;
      }
    }
  }

  // On fait le tri dans ces déjà enregistrés
  foreach (QString f, registeredPlugins) {
    if (QFile::exists(f)) {
      // Ceux qui ne sont pas valides
      Plugin *p = load(f);
      if (p) {
        // Bon lui par exemple il est bon.
        registerPlugin(p);
      } else {
        unavailablePlugins << f;
      }
    } else {
      // On met de côté ceux qui n'existent plus
      unavailablePlugins << f;
    }
  }

  if (unregisteredPlugins.size() > 0) {
    foreach (Plugin *p, unregisteredPlugins) {
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

  QString type = tr("Other");

  if (dynamic_cast<ExportEngine*>(plugin)) {
    type = tr("Export engine");
  } else if (dynamic_cast<SqlWrapper*>(plugin)) {
    type = tr("Adaptateur SQL");
  }

  QList<QStandardItem*> l;
  l << item;
  l << new QStandardItem(type);
  l << new QStandardItem(plugin->vendor());
  l << new QStandardItem(plugin->version());
  m_plugins << plugin;
  pluginsMap[plugin] = item;
  m_model->appendRow(l);
}

Plugin *PluginManagerPrivate::load(QString path) {
  Plugin *p = NULL;
  QPluginLoader loader(path);
  if(loader.load())   {
    p = dynamic_cast<Plugin*>(loader.instance());
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

QList<SqlWrapper*> PluginManagerPrivate::wrappers() {
  QList<SqlWrapper*> wrappers;

  foreach (Plugin *p, m_plugins) {
    SqlWrapper *wp = dynamic_cast<SqlWrapper*>(p);
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

void PluginManager::add(QString path) {
  instance->add(path);
}

QList<ExportEngine*> PluginManager::exportEngines() {
  return instance->exportEngines();
}

void PluginManager::init() {
  instance = new PluginManagerPrivate();
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

QList<SqlWrapper*> PluginManager::wrappers() {
  return instance->wrappers();
}
