#include "abstracttabwidget.h"

QDir AbstractTabWidget::lastDir = QDir::home();

AbstractTabWidget::AbstractTabWidget(QWidget *parent)
  : QWidget(parent) {
}
