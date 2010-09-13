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


#include "../config.h"
#include "../dbmanager.h"
#include "dbcombobox.h"

#include <QtSql>

DbComboBox::DbComboBox(QWidget *parent)
  : QComboBox(parent)
{
  setModel(DbManager::driverModel());
  setCurrentDriver(Config::defaultDriver);
}

QString DbComboBox::currentDriverName()
{
  return itemData(currentIndex(), Qt::UserRole).toString();
}

void DbComboBox::setCurrentDriver(QString driver)
{
  for(int i=0; i<count(); i++)
  {
    if(itemData(i, Qt::UserRole).toString() == driver)
    {
      setCurrentIndex(i);
      return;
    }
  }
}
