#ifndef SCHEMAWIDGET_H
#define SCHEMAWIDGET_H

#include "abstracttabwidget.h"

#include "ui_schemawidget.h"

class SchemaWidget : public AbstractTabWidget, private Ui::SchemaWidget
{
Q_OBJECT
public:
  SchemaWidget(QWidget *parent = 0);
};

#endif // SCHEMAWIDGET_H
