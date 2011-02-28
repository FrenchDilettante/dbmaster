#include "colorbutton.h"

#include <QDebug>
#include <QPainter>

ColorButton::ColorButton(QWidget *parent)
  : QToolButton(parent) {

  cDialog = new QColorDialog(this);
  connect(this, SIGNAL(clicked()), cDialog, SLOT(exec()));
  connect(cDialog, SIGNAL(colorSelected(QColor)), this, SLOT(setColor(QColor)));
}

/**
 * Mise à jour de la couleur + affichage
 */
void ColorButton::setColor(QColor c) {
  m_color = c;

  QPixmap pixmap(10, 10);
  QPainter painter;
  painter.begin(&pixmap);
  painter.setBrush(c);
  painter.setPen(Qt::NoPen);
  painter.drawRect(QRect(QPoint(0, 0), pixmap.size()));
  painter.end();

  setIcon(QIcon(pixmap));

  emit colorChanged(c);
}
