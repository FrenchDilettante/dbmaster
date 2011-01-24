#include "pluginstartupdialog.h"

PluginStartupDialog::PluginStartupDialog(QWidget *parent)
  : QDialog(parent) {
  setupUi(this);
}

QList<Plugin*> PluginStartupDialog::selectedPlugins() {
  QList<Plugin*> l;

  return l;
}

void PluginStartupDialog::setPlugins(QList<Plugin *> p) {
  plugins = p;

  pluginsTree->clear();

  foreach (Plugin *p, plugins) {
    QStringList l;
    l << p->title() << p->version();
    QTreeWidgetItem *item = new QTreeWidgetItem(pluginsTree, l);
    item->setCheckState(0, Qt::Unchecked);
    pluginsTree->addTopLevelItem(item);
  }
}
