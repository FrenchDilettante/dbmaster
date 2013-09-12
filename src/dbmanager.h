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
 * Manages the connexion pool
 *
 * @author manudwarf
 */
class DbManager : public QThread {
Q_OBJECT
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

  DbManager();

  int                     addDatabase(QString driver, QString host,
                                      QString user, QString pswd, QString dbnm,
                                      QString alias, bool usesOdbc,
                                      bool save =true);
  int                     addDatabase(QString driver, QString host,
                                      QString user, QString pswd, QString dbnm,
                                      QString alias, QString wrapper,
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
  QString                 lastError();
  QStandardItemModel*     model()     { return m_model;   };
  void                    open(int, QString=QString::null);
  void                    open(QSqlDatabase *db, QString pswd=QString::null);
  void                    openList();
  void                    removeDatabase(int);
  void                    removeDatabase(QSqlDatabase *db);
  void                    run();
  void                    saveList();
  SqlSchema               schema(QSqlDatabase *db, QString sch);
  void                    setAlias(QSqlDatabase *db, QString alias);
  void                    setDatabase(int, QSqlDatabase);
  void                    swapDatabase(QSqlDatabase *oldDb,
                                       QSqlDatabase *newDb);
  SqlTable                table(QSqlDatabase *db, QString tbl);
  void                    toggle(QSqlDatabase *db);
  void                    update(QSqlDatabase *db, QString alias);

  int lastIndex = 0;

  static QString          dbTitle(QSqlDatabase *db);
  static void             init();
  static DbManager       *instance;

public slots:
  void                    refreshModel();
  void                    refreshModelIndex(QModelIndex index);
  void                    refreshModelItem(QSqlDatabase *db);
  void                    terminate();

signals:
  void statusChanged(QModelIndex);
  void statusChanged(QSqlDatabase*);

private:
  QStandardItem*          columnsItem(QList<SqlColumn> columns);
  QString                 dbToolTip(QSqlDatabase *db);
  QSqlDatabase*           parentDb(QModelIndex index);
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

#endif // DBMANAGER_H
