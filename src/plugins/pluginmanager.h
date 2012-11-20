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


#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include "exportengine.h"
#include "plugin.h"
#include "sqlwrapper.h"

#include <QtCore>
#include <QtGui>

class PluginManagerPrivate : QObject {
Q_OBJECT
public:
  static Plugin* toPlugin(QObject *pl);

  PluginManagerPrivate();

  void          add(QString path);
  SqlWrapper*   availableWrapper(QString driver);
  QList<ExportEngine*> exportEngines();
  QObject*      load(QFileInfo path);
  QObject*      plugin(QString plid);
  void          registerPlugin(QObject *toPlugin);
  SqlWrapper*   wrapper(QString plid);
  QList<SqlWrapper*> wrappers();

  QStandardItemModel *model()   { return m_model;   };
  QList<QObject*> plugins() { return m_plugins; };

public slots:
  void init();
  void save();

private:
  QList<QFileInfo> loadFolder(QDir dir, QStringList filter, bool recursive = false);
  QStandardItemModel             *m_model;
  QList<QObject*>                 m_plugins;
  QMap<QObject*, QStandardItem*>  pluginsMap;
};

class PluginManager {
public:
  static SqlWrapper* availableWrapper(QString driver);
  static QList<ExportEngine*> exportEngines();
  static void init();
  static QStandardItemModel *model()    { return instance->model();   };
  static Plugin *plugin(QString plid);
  static QString pluginDirectory();
  static QObject* pluginObject(QString plid);
  static void registerPlugin(QObject *p);
  static void save();
  static SqlWrapper *wrapper(QString plid);
  static QList<SqlWrapper*> wrappers();

private:
  static PluginManagerPrivate *instance;
};

#endif // PLUGINMANAGER_H
