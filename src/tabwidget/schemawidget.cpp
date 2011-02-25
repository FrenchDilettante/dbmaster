#include "schemawidget.h"

#include "../iconmanager.h"
#include "../dbmanager.h"

#include <QDebug>
#include <QStandardItem>

SchemaWidget::SchemaWidget(QString schema, QSqlDatabase *db, QWidget *parent)
  : AbstractTabWidget(parent) {
  setupUi(this);

  this->m_schema = schema;
  this->m_db = db;
}

QIcon SchemaWidget::icon() {
  return IconManager::get("schema");
}

QString SchemaWidget::id() {
  return QString("s %1 on %2")
            .arg(m_schema)
            .arg(m_db->connectionName());
}

void SchemaWidget::on_tableTree_itemDoubleClicked(QTreeWidgetItem *item,
                                                  int col) {
  QString table = prefix.length() == 0 ? "" : prefix + ".";
  table += item->text(0);
  emit tableRequested(m_db, table);
}

void SchemaWidget::reload() {
  SqlSchema s = DbManager::schema(m_db, m_schema);
  schemaEdit->setText(s.name);

  prefix = s.defaultSchema ? "" : s.name;

  tableTree->clear();

  foreach (SqlTable t, s.tables) {
    QStringList ligne;
    ligne << t.name;
    ligne << QString::number(t.columns.size());

    QTreeWidgetItem *it = new QTreeWidgetItem(ligne);
    if (t.type == Table) {
      it->setIcon(0, IconManager::get("table"));
    } else {
      it->setIcon(0, IconManager::get("table_lightning"));
    }
    tableTree->addTopLevelItem(it);
  }

  tableTree->resizeColumnToContents(0);
}
