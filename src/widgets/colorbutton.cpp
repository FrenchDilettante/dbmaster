#include "colorbutton.h"

#include <QPainter>

ColorButton::ColorButton(QWidget *parent) :
    QToolButton(parent) {
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
