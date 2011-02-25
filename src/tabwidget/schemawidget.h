#ifndef SCHEMAWIDGET_H
#define SCHEMAWIDGET_H

#include "abstracttabwidget.h"

#include "ui_schemawidget.h"

class SchemaWidget : public AbstractTabWidget, private Ui::SchemaWidget
{
Q_OBJECT
public:
  SchemaWidget(QString schema, QSqlDatabase *db, QWidget *parent = 0);

  QIcon icon();
  QString id();

public slots:
  void reload();

private:
  QSqlDatabase *m_db;
  QString m_schema;
  QString prefix;

private slots:
  void on_tableTree_itemDoubleClicked(QTreeWidgetItem *item, int col);
};

#endif // SCHEMAWIDGET_H
