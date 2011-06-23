#include "schemawidget.h"

#include "../iconmanager.h"
#include "../dbmanager.h"

#include <QDebug>
#include <QStandardItem>

SchemaWidget::SchemaWidget(QString schema, int idx, QWidget *parent)
  : AbstractTabWidget(parent) {
  setupUi(this);

  this->m_schema = schema;
  this->dbIdx = idx;
  this->m_db = DbManager::db(idx);
}

QIcon SchemaWidget::icon() {
  return IconManager::get("schema");
}

QString SchemaWidget::id() {
  return QString("s %1 on %2")
            .arg(m_schema)
            .arg(dbIdx);
}

void SchemaWidget::on_tableTree_itemDoubleClicked(QTreeWidgetItem *item,
                                                  int col) {
  QString table = prefix.length() == 0 ? "" : prefix + ".";
  table += item->text(0);
  emit tableRequested(dbIdx, table);
}

void SchemaWidget::reload() {
  SqlSchema s = DbManager::schema(dbIdx, m_schema);
  schemaEdit->setText(s.name);

  prefix = s.defaultSchema ? "" : s.name;

  tableTree->clear();

  foreach (SqlTable t, s.tables) {
    QStringList ligne;
    ligne << t.name;
    ligne << QString::number(t.columns.size());
    ligne << t.comment;

    QTreeWidgetItem *it = new QTreeWidgetItem(ligne);
    if (t.type == Table) {
      it->setIcon(0, IconManager::get("table"));
      tableTree->addTopLevelItem(it);
    } else {
      it->setIcon(0, IconManager::get("table_lightning"));
      viewTree->addTopLevelItem(it);
    }
  }
}
