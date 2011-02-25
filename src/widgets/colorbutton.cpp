#include "colorbutton.h"

#include <QPainter>

ColorButton::ColorButton(QWidget *parent)
  : QToolButton(parent) {

  cDialog = new QColorDialog(this);
  connect(this, SIGNAL(clicked()), cDialog, SLOT(exec()));
  connect(cDialog, SIGNAL(colorSelected(QColor)), this, SLOT(setColor(QColor)));
}

void ColorButton::render(QPainter *painter, const QPoint &targetOffset,
                         const QRegion &sourceRegion, RenderFlags renderFlags) {
  QToolButton::render(painter, targetOffset, sourceRegion, renderFlags);

  painter->save();
  painter->setPen(Qt::NoPen);
  painter->setBrush(Qt::red);
  painter->drawRect(QRect(0, 0, 10, 10));
  painter->restore();
}

void ColorButton::setColor(QColor c) {
  c = m_color;

  emit colorChanged(c);
}
