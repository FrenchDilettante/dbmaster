#include "sqlitemdelegate.h"

#include <QDebug>
#include <QPainter>

SqlItemDelegate::SqlItemDelegate(QObject *parent)
  : QItemDelegate(parent) {
}

void SqlItemDelegate::drawDisplay(QPainter *painter,
                                  const QStyleOptionViewItem &option,
                                  const QRect &rect, const QString &text) const {
  if (text.isNull()) {
    painter->setPen(Qt::lightGray);
    painter->drawText(rect.adjusted(4, 3, -4, -3), "null");
  } else {
    QItemDelegate::drawDisplay(painter, option, rect, text);
  }
}
