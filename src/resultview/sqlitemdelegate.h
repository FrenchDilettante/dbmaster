#ifndef SQLITEMDELEGATE_H
#define SQLITEMDELEGATE_H

#include <QItemDelegate>

class SqlItemDelegate : public QItemDelegate {
Q_OBJECT
public:
  explicit SqlItemDelegate(QObject *parent = 0);

signals:

public slots:

protected:
  void drawDisplay(QPainter* painter,
                   const QStyleOptionViewItem& option,
                   const QRect& rect,
                   const QString& text) const;

};

#endif // SQLITEMDELEGATE_H
