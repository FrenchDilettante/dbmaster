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


#ifndef WRAPPER_H
#define WRAPPER_H

#include "../plugin.h"

#include <QList>
#include <QSqlDatabase>
#include <QString>
#include <QStringList>

class Wrapper : public Plugin
{
public:
  enum DbType {
    FileDb,
    IndexedDb,
    LocalDb,
    RemoteDb
  };

  Wrapper();

  virtual void            close()       =0;
  virtual QStringList     tables()      =0;
  virtual bool            open()        =0;

//  static QList<Wrapper*>  availableFor(QString driver);

protected:
  virtual QString         driver()      =0;

  QSqlDatabase           *db;
  static QList<Wrapper*>  wrappers;
};

Q_DECLARE_INTERFACE(Wrapper, "dbmaster.Wrapper")

#endif // WRAPPER_H
