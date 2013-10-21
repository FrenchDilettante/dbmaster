#include "abstracttabwidget.h"

QDir AbstractTabWidget::lastDir = QDir::home();

AbstractTabWidget::AbstractTabWidget(QWidget *parent)
  : QWidget(parent) {
}

QString AbstractTabWidget::displayTitle() {
  QString display = title();
  if (!isSaved()) {
    display.prepend("* ");
  }
  return display;
}
