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


#ifndef LOGDIALOG_H
#define LOGDIALOG_H

#include <QDialog>

#include "ui_logdialogclass.h"

class LogDialog : public QDialog, Ui::LogDialogClass
{
Q_OBJECT
public:
  LogDialog(QWidget *parent=0);

  void append(QString text, QMap<QString, QVariant> infos);

  static LogDialog *instance();

signals:
  void event(QString);

public slots:
  void append(QString text);

private:
  static LogDialog* m_instance;

private slots:
  void updateFilter(QString filter);
};

#endif // LOGDIALOG_H
