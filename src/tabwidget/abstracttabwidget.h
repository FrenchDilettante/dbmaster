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


#ifndef ABSTRACTTABWIDGET_H
#define ABSTRACTTABWIDGET_H

#include <QAction>
#include <QDir>
#include <QIcon>
#include <QMessageBox>
#include <QPrinter>
#include <QRunnable>
#include <QSqlDatabase>
#include <QString>
#include <QTextEdit>
#include <QWidget>

class AbstractTabWidget: public QWidget, protected QRunnable {

Q_OBJECT
public:
  enum Action {
    CaseLower   = 0x00001,
    CaseUpper   = 0x00002,
    Copy        = 0x00004,
    Cut         = 0x00008,
    Paste       = 0x00010,
    Redo        = 0x00020,
    Print       = 0x00040,
    Save        = 0x00080,
    SaveAs      = 0x00100,
    Search      = 0x00200,
    SelectAll   = 0x00400,
    Undo        = 0x00800
  };
  Q_DECLARE_FLAGS(Actions, Action)

  AbstractTabWidget(QWidget *parent =0);
  virtual Actions   availableActions(){ return 0x0; };
  virtual bool      confirmClose()    { return true; };
  virtual int       confirmCloseAll() { return QMessageBox::Save; };
  virtual QIcon     icon()            { return QIcon(); };
  virtual QString   id()              { return QString::null; };
  virtual bool      isSaved()         { return true; };
  static QDir       lastDir;
  virtual void      print()           {};
  virtual QPrinter *printer()         { return NULL; };
  virtual void      run()             {};
  virtual QTextEdit* textEdit()       { return NULL; };
  virtual QString   title()           { return ""; };

signals:
  void actionAvailable(AbstractTabWidget::Actions);
  void closeRequested();
  void modificationChanged(bool);
  void tableRequested(int idx, QString table);

public slots:
  virtual void copy()                 {};
  virtual void cut()                  {};
  virtual void lowerCase()            {};
  virtual void open(QString)          {};
  virtual void paste()                {};
  virtual void redo()                 {};
  virtual void refresh()              {};
  virtual void reload()               {};
  virtual bool save()                 { return true; };
  virtual void saveAs(QString =QString::null)     {};
  virtual void selectAll()            {};
  virtual void undo()                 {};
  virtual void upperCase()            {};

protected:
  QPrinter    m_printer;

protected slots:
  void        start();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(AbstractTabWidget::Actions)

#endif // ABSTRACTTABWIDGET_H
