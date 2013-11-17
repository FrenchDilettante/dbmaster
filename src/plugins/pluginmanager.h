#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include "exportengine.h"
#include "plugin.h"
#include "sqlwrapper.h"

#include <QtCore>
#include <QtGui>

class PluginManagerPrivate : QObject {
Q_OBJECT
public:
  static Plugin* toPlugin(QObject *pl);

  PluginManagerPrivate();

  void          add(QString path);
  SqlWrapper*   availableWrapper(QString driver);
  QList<ExportEngine*> exportEngines();
  QObject*      plugin(QString plid);
  void          registerPlugin(QObject *toPlugin);
  SqlWrapper*   wrapper(QString plid);
  QList<SqlWrapper*> wrappers();

  QStandardItemModel *model()   { return m_model;   };
  QList<QObject*> plugins() { return m_plugins; };

public slots:
  void init();

private:
  QStandardItemModel             *m_model;
  QList<QObject*>                 m_plugins;
  QMap<QObject*, QStandardItem*>  pluginsMap;
};

class PluginManager {
public:
  static SqlWrapper* availableWrapper(QString driver);
  static QList<ExportEngine*> exportEngines();
  static void init();
  static QStandardItemModel *model()    { return instance->model();   };
  static Plugin *plugin(QString plid);
  static QObject* pluginObject(QString plid);
  static SqlWrapper *wrapper(QString plid);
  static QList<SqlWrapper*> wrappers();

private:
  static PluginManagerPrivate *instance;
};

#endif // PLUGINMANAGER_H
