#ifndef PLUGINDIALOG_H
#define PLUGINDIALOG_H

#include "ui_plugindialog.h"

#include <QDialog>
#include <QItemDelegate>

class PluginDialog : public QDialog, Ui::PluginDialog {
Q_OBJECT
public:
  PluginDialog(QWidget *parent = 0);

private slots:
  void on_configButton_clicked();
};

#endif // PLUGINDIALOG_H
