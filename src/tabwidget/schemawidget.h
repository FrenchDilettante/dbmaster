#ifndef SCHEMAWIDGET_H
#define SCHEMAWIDGET_H

#include "abstracttabwidget.h"

#include "ui_schemawidget.h"

#include <QStandardItemModel>

class SchemaWidget : public AbstractTabWidget, private Ui::SchemaWidget {
Q_OBJECT
public:
  SchemaWidget(QString schema, int idx, QWidget *parent = 0);

  QIcon icon();
  QString id();
  QStandardItemModel* properties() { return m_properties; };

public slots:
  void reload();

private:
  void setupProperties();

  int                 dbIdx;
  QSqlDatabase        m_db;
  QStandardItem*      nameProperty;
  QStandardItemModel* m_properties;
  QString             m_schema;
  QString             prefix;

private slots:
  void on_tableTree_itemDoubleClicked(QTreeWidgetItem *item, int col);
};

#endif // SCHEMAWIDGET_H
