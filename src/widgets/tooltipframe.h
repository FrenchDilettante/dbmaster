#ifndef TOOLTIPFRAME_H
#define TOOLTIPFRAME_H

#include <QFrame>
#include <QStringList>
#include <QTimer>
#include <QWidget>

#include "ui_tooltipframe.h"

class TooltipFrame : public QFrame, Ui::TooltipFrame {
Q_OBJECT
public:
  TooltipFrame(QWidget *parent = 0);

public slots:
  void showTooltip();

private:
  int currentIndex;
  QTimer t;
  QStringList tooltips;
};

#endif // TOOLTIPFRAME_H
