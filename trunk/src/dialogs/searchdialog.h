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


#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include "ui_searchdialog.h"

#include <QDialog>
#include <QPushButton>
#include <QTextDocument>
#include <QTextEdit>

class SearchDialog : public QDialog, private Ui::SearchDialog
{
Q_OBJECT
public:
  SearchDialog(QWidget *parent = 0);

  QTextEdit     *editor()     { return m_textEdit;   };
  void setEditor(QTextEdit *edit) { m_textEdit = edit; };

private:
  void setupWidgets();

  QTextEdit *m_textEdit;
  QPushButton *replaceButton;
  QPushButton *searchButton;

private slots:
  void search();
};

#endif // SEARCHDIALOG_H
