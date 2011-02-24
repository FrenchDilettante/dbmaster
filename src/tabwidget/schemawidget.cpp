#include "schemawidget.h"

#include "../iconmanager.h"

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

void SchemaWidget::reload() {
  schemaEdit->setText(m_schema);
}
