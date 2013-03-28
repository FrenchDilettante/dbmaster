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


#ifndef QUERYTEXTEDIT_H
#define QUERYTEXTEDIT_H

#include <QtGui>

#include "../sqlhighlighter.h"

class QueryTextEdit : public QTextEdit {
Q_OBJECT
public:
  QueryTextEdit(QWidget *parent=0);

  void reloadContext(QStringList tables, QMultiMap<QString, QString> fields);
  static void reloadCompleter();

protected:
  void insertFromMimeData(const QMimeData *source);

private:
  void focusInEvent(QFocusEvent *e);
  void keyPressEvent(QKeyEvent *e);
  /**
   * Defines the completion
   */
  void setupCompleter();

  /**
   * @returns the word under the cursor
   */
  QString textUnderCursor() const;

  QCompleter *completer;

  QStringListModel *completerContextModel;
  QStringList tables;
  SqlHighlighter *syntax;

private slots:
  void cleanTables();
  void insertCompletion(QString text);
  void scanTables();
};

#endif // QUERYTEXTEDIT_H
