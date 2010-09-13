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
#include <QSqlDatabase>
#include <QString>
#include <QTextEdit>
#include <QWidget>

class AbstractTabWidget: public QWidget
{
Q_OBJECT
public:
  enum Action {
    Copy        = 0x0001,
    Cut         = 0x0002,
    Paste       = 0x0004,
    Redo        = 0x0008,
    Print       = 0x0010,
    Save        = 0x0020,
    SaveAs      = 0x0040,
    Search      = 0x0080,
    SelectAll   = 0x0100,
    Undo        = 0x0200
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
  virtual QTextEdit* textEdit()       { return NULL; };
  virtual QString   title()           { return ""; };

signals:
  void actionAvailable(AbstractTabWidget::Actions);
  void modificationChanged(bool);
  void tableRequested(QSqlDatabase *db, QString table);

public slots:
  virtual void copy()                 {};
  virtual void cut()                  {};
  virtual void open(QString)          {};
  virtual void paste()                {};
  virtual void redo()                 {};
  virtual void refresh()              {};
  virtual void reload()               {};
  virtual bool save()                 { return true; };
  virtual void saveAs(QString =0)     {};
  virtual void selectAll()            {};
  virtual void undo()                 {};

protected:
  QPrinter    m_printer;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(AbstractTabWidget::Actions)

#endif // ABSTRACTTABWIDGET_H
