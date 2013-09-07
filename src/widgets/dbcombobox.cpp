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
