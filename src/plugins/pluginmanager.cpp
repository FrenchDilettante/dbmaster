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
  : QObject() {
  m_model = new QStandardItemModel(this);

  init();
}

void PluginManagerPrivate::add(QString path)
{
  QPluginLoader loader(path);
  if(loader.load())
  {
    Plugin *p = dynamic_cast<Plugin*>(loader.instance());
    if(p) {
      QStandardItem *item = new QStandardItem();
      item->setText(QString("<b>%1</b> <i>%2</i>")
                    .arg(p->title())
                    .arg(p->version()));
      item->setData(path, Qt::UserRole);
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

void PluginManagerPrivate::init() {
  QSettings s;
  s.beginGroup("plugins");
  int size = s.beginReadArray("list");
  for(int i=0; i<size; i++) {
    s.setArrayIndex(i);
    add(s.value("path").toString());
  }
  s.endArray();
  s.endGroup();
}

void PluginManagerPrivate::save() {
  QSettings s;
  s.beginGroup("plugins");
  s.beginWriteArray("list");
  for (int i=0; i<m_model->rowCount(); i++) {
    s.setArrayIndex(i);
    s.setValue("path", m_model->item(i, 0)->data(Qt::UserRole));
  }
  s.endArray();
  s.endGroup();
}

/*
 * PluginManager
 */

PluginManagerPrivate *PluginManager::instance = NULL;

void PluginManager::add(QString path) {
  instance->add(path);
}

void PluginManager::init() {
  instance = new PluginManagerPrivate();
}

QString PluginManager::pluginDirectory() {
  return QDir::homePath();
}

void PluginManager::registerPlugin(Plugin *p) {

}

void PluginManager::save() {
  instance->save();
}
