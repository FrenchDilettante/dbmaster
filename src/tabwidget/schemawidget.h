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
};

#endif // SCHEMAWIDGET_H
