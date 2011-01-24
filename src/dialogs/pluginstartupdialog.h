#ifndef PLUGINSTARTUPDIALOG_H
#define PLUGINSTARTUPDIALOG_H

#include "../plugins/plugin.h"

#include "ui_pluginstartupdialog.h"

class PluginStartupDialog : public QDialog, private Ui::PluginStartupDialog {
Q_OBJECT
public:
  PluginStartupDialog(QWidget *parent = 0);

  QList<Plugin*> selectedPlugins();
  void setPlugins(QList<Plugin*> p);

private:
  QList<Plugin*> plugins;
};

#endif // PLUGINSTARTUPDIALOG_H
