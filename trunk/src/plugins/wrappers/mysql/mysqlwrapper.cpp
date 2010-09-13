#include "mysqlwrapper.h"

MysqlWrapper::MysqlWrapper()
  : Wrapper()
{
  db = NULL;

  wrappers << this;
}

void MysqlWrapper::close()
{
  if(db)
    db->close();
}

QString MysqlWrapper::driver()
{
  return "QMYSQL";
}

bool MysqlWrapper::open()
{
  if(db)
    return db->open();
  else
    return false;
}

QStringList MysqlWrapper::tables()
{
  return QStringList();
}

QString MysqlWrapper::title()
{
  return tr("MySQL Wrapper");
}

QString MysqlWrapper::version()
{
  return "0.7";
}

Q_EXPORT_PLUGIN2(mysql_wrapper, MysqlWrapper)
