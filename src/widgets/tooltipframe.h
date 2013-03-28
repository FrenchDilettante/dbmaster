/*
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 */


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
