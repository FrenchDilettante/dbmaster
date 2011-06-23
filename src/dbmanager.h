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
class DbManagerPrivate : public QObject {
Q_OBJECT
public:
  enum ConnectionStatus {
    Closed,
    Connecting,
    Open
  };

  /**
   * Structure pour regrouper plusieurs infos.
   */
  struct Connection {
    QString           alias;
    QSqlDatabase      db;
    QStandardItem*    item;
    ConnectionStatus  status;
    SqlWrapper*       wrapper;
  };

  DbManagerPrivate();

  int                     addDatabase(QString driver, QString host,
                                      QString user, QString pswd, QString dbnm,
                                      QString alias, bool usesOdbc,
                                      bool save =true);
  int                     addDatabase(QString driver, QString host,
                                      QString user, QString pswd, QString dbnm,
                                      QString alias, QString wrapper,
                                      bool save =true);
  QString                 alias(int idx);
  void                    close(int idx);
  void                    closeAll();
  QList<Connection>       connections() { return m_connections; };
  QSqlDatabase            db(int idx);
  QStandardItemModel     *driverModel();
  QString                 genConnectionName();
  void                    init();
  QString                 lastError();
  QStandardItemModel*     model()     { return m_model;   };
  void                    open(int, QString=QString::null);
  void                    openList();
  void                    removeDatabase(int idx);
  void                    saveList();
  SqlSchema               schema(int idx, QString sch);
  void                    setAlias(int idx, QString alias);
  void                    setDatabase(int idx, QSqlDatabase db);
  void                    swapDatabase(int oldIdx, int newIdx);
  SqlTable                table(int idx, QString tbl);
  void                    toggle(int idx);
  void                    update(int idx, QString alias);

  static QString          dbTitle(QSqlDatabase db);

public slots:
  void                    refreshModel();
  void                    refreshModelIndex(QModelIndex index);
  void                    refreshModelItem(int idx);
  void                    terminate();

signals:
  void logMessage(QString);
  void statusChanged(QModelIndex);
  void statusChanged(int);

private:
  void                    checkDbIndex(int idx);
  QStandardItem*          columnsItem(QList<SqlColumn> columns);
  QString                 dbToolTip(QSqlDatabase db);
  QSqlDatabase*           parentDb(QModelIndex index);
  void                    setupModels();
  QStandardItem*          schemaItem(SqlSchema schema);
  QStandardItem*          tablesItem(QList<SqlTable> tables,
                                     QString schema ="");
  QStandardItem*          viewsItem(QList<SqlTable> tables,
                                    QString schema ="");

  QMap<QString, QString>  driverAlias;
  QMap<QString, QIcon>    driverIcon;

  bool                    closingAll;
  QList<Connection>       m_connections;
  QStandardItemModel     *m_driverModel;

  QStandardItemModel     *m_model;
  /// @deprecated
  unsigned long           nconn;
  QString                 lastErr;

  QStack<QSqlDatabase*>   closeStack;
  QStack<QSqlDatabase*>   openStack;
};

/**
 * Classe statique pour l'accès aux connexions.
 */
class DbManager {
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
                                    QString pswd, QString dbnm, QString alias,
                                    bool usesOdbc =false);
  static int            addDatabase(QString driver, QString host, QString user,
                                    QString dbnm, QString alias,
                                    bool usesOdbc =false);
  static QString        alias(int idx);
  static void           close(int idx);
  static void           closeAll();
  static QList<DbManagerPrivate::Connection> connections();
  static QSqlDatabase   db(int idx);
  static QString        dbTitle(int idx);
  static QStandardItemModel*
                        driverModel();
  static void           init();
  static DbManagerPrivate *instance()   { return m_instance;    };
  static bool           isClosed(int idx);
  static bool           isOpen(int idx);
  static int            lastIndex;
  static QStandardItemModel*
                        model()     { return m_instance->model();   };
  static void           open(int, QString=QString::null);
  static void           refreshModelIndex(QModelIndex index);
  static void           refreshModelItem(int idx);
  static void           removeDatabase(int idx);
  static SqlSchema      schema(int idx, QString schema);
  static void           setAlias(int idx, QString alias);
  static SqlTable       table(int idx, QString tbl);
  static void           toggle(int idx);
  static void           update(int idx, QString alias);

private:
  static DbManagerPrivate *m_instance;
};

#endif // DBMANAGER_H
