#ifndef COLORBUTTON_H
#define COLORBUTTON_H

#include <QColor>
#include <QToolButton>

class ColorButton : public QToolButton
{
Q_OBJECT
public:
  ColorButton(QWidget *parent = 0);

  QColor color() { return m_color; };
  void setColor(QColor c) { c = m_color; };

  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

signals:
  void colorChanged(QColor);

public slots:

private:
  void render(QPainter *painter, const QPoint &targetOffset,
              const QRegion &sourceRegion, RenderFlags renderFlags);

  QColor m_color;

};

#endif // COLORBUTTON_H
