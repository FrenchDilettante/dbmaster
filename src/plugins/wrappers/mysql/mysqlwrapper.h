#ifndef MYSQLWRAPPER_H
#define MYSQLWRAPPER_H

#include "../wrapper.h"

class MysqlWrapper : public QObject, Wrapper
{
Q_OBJECT
Q_INTERFACES(Wrapper)
public:
  MysqlWrapper();

  void          close();
  bool          open();
  QStringList   tables();
  QString       title();
  QString       version();

protected:
  QString driver();
};

#endif // MYSQLWRAPPER_H
