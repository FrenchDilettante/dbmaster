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


#ifndef DBMANAGER_H
#define DBMANAGER_H

#include "plugins/sqlwrapper.h"

#include <QList>
#include <QSqlDatabase>
#include <QSqlError>
#include <QStack>
#include <QStandardItemModel>
#include <QString>
#include <QThread>

/**
 * Classe interne, singleton de DbManager.
 * ! À ne pas appeler en direct !
 */
class DbManagerPrivate : public QThread
{
Q_OBJECT
public:
  DbManagerPrivate();

  int                     addDatabase(QString driver, QString host,
                                      QString user, QString pswd, QString dbnm,
                                      QString alias, QString wrapper ="",
                                      bool save =true);
  QString                 alias(QSqlDatabase *db);
  void                    close(QSqlDatabase *db);
  void                    closeAll();
  QStandardItemModel     *driverModel();
  QString                 genConnectionName();
  QSqlDatabase*           getDatabase(int n);
  QList<QSqlDatabase*>    getDbList();
  QStringList             getDbNames(bool);
  int                     indexOf(QSqlDatabase *db);
  void                    init();
  QString                 lastError();
  QStandardItemModel*     model()     { return m_model;   };
  void                    open(int, QString=QString::null);
  void                    open(QSqlDatabase *db, QString pswd=QString::null);
  void                    openList();
  void                    removeDatabase(int);
  void                    removeDatabase(QSqlDatabase *db);
  void                    run();
  void                    saveList();
  SqlSchema               schema(QSqlDatabase *db, QString schema);
  void                    setDatabase(int, QSqlDatabase);
  void                    swapDatabase(QSqlDatabase *oldDb,
                                       QSqlDatabase *newDb);
  void                    toggle(QSqlDatabase *db);
  void                    update(QSqlDatabase *db, QString alias);

  static QString          dbTitle(QSqlDatabase *db);

public slots:
  void                    refreshModel();
  void                    refreshModelItem(QSqlDatabase *db);
  void                    terminate();

signals:
  void statusChanged(QModelIndex index);
  void statusChanged(QSqlDatabase*);

private:
  QStandardItem*          columnsItem(QList<SqlColumn> columns);
  QString                 dbToolTip(QSqlDatabase *db);
  void                    setupConnections();
  void                    setupModels();
  QStandardItem*          schemaItem(SqlSchema schema);
  QStandardItem*          tablesItem(QList<SqlTable> tables,
                                     QString schema ="");
  QStandardItem*          viewsItem(QList<SqlTable> tables,
                                    QString schema ="");

  QMap<QString, QString>  driverAlias;
  QMap<QString, QIcon>    driverIcon;

  bool                    closingAll;
  QStandardItemModel     *m_driverModel;
  QList<QSqlDatabase*>    dbList;
  QMap<QSqlDatabase*, QStandardItem*> dbMap;
  QMap<QSqlDatabase*, SqlWrapper*> dbWrappers;
  QStandardItemModel     *m_model;
  int                     nconn;
  QString                 lastErr;

  QStack<QSqlDatabase*>   closeStack;
  QStack<QSqlDatabase*>   openStack;

};

/**
 * Classe statique pour l'accès aux connexions.
 */
class DbManager
{
public:
  enum ItemTypes {
    DbItem,
    DisplayItem,
    FieldItem,
    SchemaItem,
    SysTableItem,
    TableItem,
    ViewItem
  };

  static int            addDatabase(QString driver, QString host, QString user,
                                    QString pswd, QString dbnm, QString alias);
  static int            addDatabase(QString driver, QString host, QString user,
                                    QString dbnm, QString alias);
  static QString        alias(QSqlDatabase *db);
  static void           close(QSqlDatabase *db);
  static void           closeAll();
  static QString        dbTitle(QSqlDatabase *db);
  static QStandardItemModel*
                        driverModel();
  static QString        genConnectionName();
  static QSqlDatabase*
                        getDatabase(int n);
  static QList<QSqlDatabase*>
                        getDbList();
  static QStringList    getDbNames(bool);
  /// @deprecated
  static int            indexOf(QSqlDatabase *db);
  static void           init();
  static DbManagerPrivate *instance()   { return m_instance;    };
  static int            lastIndex;
  /// @deprecated
  static QString        lastError();
  static QStandardItemModel*
                        model()     { return m_instance->model();   };
  static void           open(int, QString=QString::null);
  static void           open(QSqlDatabase *db, QString pswd=QString::null);
  static void           openList();
  static void           refreshModelItem(QSqlDatabase *db);
  static void           removeDatabase(int);
  static void           saveList();
  static SqlSchema      schema(QSqlDatabase *db, QString schema);
  static void           setDatabase(int, QSqlDatabase);
  static void           terminate();
  static void           toggle(QSqlDatabase *db);
  static void           update(QSqlDatabase *db, QString alias);

private:
  static DbManagerPrivate *m_instance;
};

#endif // DBMANAGER_H
