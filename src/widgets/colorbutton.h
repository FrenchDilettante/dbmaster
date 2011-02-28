#ifndef COLORBUTTON_H
#define COLORBUTTON_H

#include <QColor>
#include <QColorDialog>
#include <QToolButton>

class ColorButton : public QToolButton
{
Q_OBJECT
public:
  ColorButton(QWidget *parent = 0);

  QColor color() { return m_color; };

  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

signals:
  void colorChanged(QColor);

public slots:
  void setColor(QColor c);

private:
  QColorDialog *cDialog;
  QColor m_color;

};

#endif // COLORBUTTON_H
