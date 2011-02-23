#ifndef FAKEPLUGIN_H
#define FAKEPLUGIN_H

#include "plugin.h"

#include <QApplication>
#include <QObject>

class FakePlugin : public QObject, public Plugin {
Q_OBJECT
Q_INTERFACES(Plugin)
public:
  FakePlugin();

  QString plid() { return "DBM.FAKEPLUGIN"; };
  QString title() { return "Totally dummy plugin"; };
  QString vendor() { return "DbMaster"; };
  QString version() { return QApplication::applicationVersion(); };
};

#endif // FAKEPLUGIN_H
