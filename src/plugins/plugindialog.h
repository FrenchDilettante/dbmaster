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


#ifndef PLUGINDIALOG_H
#define PLUGINDIALOG_H

#include "ui_plugindialog.h"

#include <QDialog>
#include <QItemDelegate>

class PluginDialog : public QDialog, Ui::PluginDialog
{
Q_OBJECT
public:
  PluginDialog(QWidget *parent = 0);

private slots:
  void on_configButton_clicked();
};

#endif // PLUGINDIALOG_H
