#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include "ui_searchdialog.h"

#include <QCloseEvent>
#include <QDialog>
#include <QPushButton>
#include <QStringList>
#include <QTextDocument>
#include <QTextEdit>

class SearchDialog : public QDialog, private Ui::SearchDialog {
Q_OBJECT
public:
  SearchDialog(QWidget *parent = 0);

  QTextEdit     *editor()     { return m_textEdit;   };
  void setEditor(QTextEdit *edit) { m_textEdit = edit; };

private:
  void closeEvent(QCloseEvent *event);
  void setupWidgets();

  QTextEdit *m_textEdit;
  QPushButton *replaceButton;
  QPushButton *searchButton;

  static QStringList replaceHistory;
  static QStringList searchHistory;

private slots:
  void replace();
  QTextCursor search();
};

#endif // SEARCHDIALOG_H
