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

/*
 * PluginManagerPrivate
 */

PluginManagerPrivate::PluginManagerPrivate()
  : QObject()
{
  m_model = new QStandardItemModel(this);

  check();
}

void PluginManagerPrivate::add(QString path)
{
  QPluginLoader loader(path);
  if(loader.load())
  {
    Plugin *p = dynamic_cast<Plugin*>(loader.instance());
    if(p) {
      QStandardItem *item = new QStandardItem(p->title());
      m_plugins[p] = item;
      m_model->appendRow(item);
    } else {
      qDebug() << "dynamic_cast<>()";
    }
  } else {
    QMessageBox::critical(NULL,
                          tr("Incorrect plugin file"),
                          tr("Unable to load the plugin. Specified error : \n")
                            + loader.errorString());
  }
}

void PluginManagerPrivate::check()
{
  QSettings s;
  s.beginGroup("plugins");
  int size = s.beginReadArray("list");
  for(int i=0; i<size; i++)
  {
    add(s.value("path").toString());
  }
}

/*
 * PluginManager
 */

PluginManagerPrivate *PluginManager::instance = new PluginManagerPrivate();

void PluginManager::add(QString path)
{
  instance->add(path);
}

void PluginManager::init()
{
  instance->check();
}

void PluginManager::registerPlugin(Plugin *p) {

}
