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


#include "plugindialog.h"
#include "pluginmanager.h"
#include "ui_plugindialog.h"

PluginDialog::PluginDialog(QWidget *parent)
  : QDialog(parent)
{
  setupUi(this);

  listView->setModel(PluginManager::model());

  connect(addButton, SIGNAL(clicked()), this, SLOT(add()));
}

void PluginDialog::add()
{
  QString p = QFileDialog::getOpenFileName(this, tr("Choose a plugin file"), QDir::homePath());
  PluginManager::add(p);
}
