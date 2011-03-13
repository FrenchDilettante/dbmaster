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


#include <QtGui>

#include "logdialog.h"
#include "../dbmanager.h"
#include "../iconmanager.h"
#include "../mainwindow.h"

LogDialog *LogDialog::m_instance = NULL;

LogDialog *LogDialog::instance()
{
  if(!m_instance)
    m_instance = new LogDialog();

  return m_instance;
}

LogDialog::LogDialog(QWidget *parent)
  : QDialog(parent)
{
  setupUi( this );

  connect(lineEdit, SIGNAL(textChanged(QString)),
          this, SLOT(updateFilter(QString)));

  connect(DbManager::instance(), SIGNAL(logMessage(QString)),
          this, SLOT(append(QString)));

  clearButton->setIcon(IconManager::get("edit-clear"));
}

void LogDialog::append(QString text)
{
  append(text, QMap<QString, QVariant>());
}

void LogDialog::append(QString text, QMap<QString, QVariant> infos)
{
  QStringList l;
  l << QTime::currentTime().toString(Qt::DefaultLocaleShortDate) << text;
  QTreeWidgetItem *root = new QTreeWidgetItem(treeWidget, l);
  QList<QTreeWidgetItem*> items;
  foreach(QString key, infos.keys())
  {
    l = QStringList();
    l << key << infos[key].toString();
    items << new QTreeWidgetItem(root, l);
  }
  treeWidget->insertTopLevelItem(0, root);
  treeWidget->resizeColumnToContents(0);

  emit event(text);
}

void LogDialog::updateFilter(QString filter)
{
  QTreeWidgetItem *item;
  for(int i=0; i<treeWidget->topLevelItemCount(); i++)
  {
    item = treeWidget->topLevelItem(i);
    if(!filter.isEmpty() && item->text(1).contains(filter, Qt::CaseInsensitive))
      item->setBackgroundColor(1, Qt::yellow);
    else
      item->setBackground(1, Qt::NoBrush);
  }
}
