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

#include <QtCore>
#include <QtGui>

class PluginManagerPrivate : QObject
{
Q_OBJECT
public:
  PluginManagerPrivate();

  void add(QString path);
  QList<ExportEngine*> exportEngines();
  void registerPlugin(Plugin *plugin);

  QStandardItemModel *model()   { return m_model;   };
  QList<Plugin*> plugins() { return m_plugins; };

public slots:
  void init();
  void save();

private:
  QStandardItemModel             *m_model;
  QList<Plugin*>                  m_plugins;
  QMap<Plugin*, QStandardItem*>   pluginsMap;
};

class PluginManager
{
public:
  static void add(QString path);
  static QList<ExportEngine*> exportEngines();
  static void init();
  static QStandardItemModel *model()    { return instance->model();   };
  static QString pluginDirectory();
  static void registerPlugin(Plugin *p);
  static void save();

private:
  static PluginManagerPrivate *instance;
};

#endif // PLUGINMANAGER_H
