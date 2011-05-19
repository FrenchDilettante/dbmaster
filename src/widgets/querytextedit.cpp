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


#include <QtSql>

#include "../dbmanager.h"
#include "querytextedit.h"

#include "../config.h"

QueryTextEdit::QueryTextEdit(QWidget *parent)
    : QTextEdit(parent)
{
  syntax = new SqlHighlighter(this);

  setupCompleter();

  QFont f = Config::editorFont;
  f.setPointSize(Config::editorFont.pointSize());
  setFont(Config::editorFont);
  setFontPointSize(Config::editorFont.pointSize());
  QFontMetrics metrics(f);
  setTabStopWidth(Config::editorTabSize * metrics.width(' '));
//  setOpenExternalLinks(true);
//  setOpenLinks(true);
//  setReadOnly(false);
}

void QueryTextEdit::cleanTables()
{

}

void QueryTextEdit::focusInEvent(QFocusEvent *e)
{
  completer->setWidget(this);
  QTextEdit::focusInEvent(e);
}

void QueryTextEdit::keyPressEvent(QKeyEvent *event)
{
  if(Config::compCharCount == -1)
    return;

  // If the completer is actually shown, it handles some keys
  if(completer->popup()->isVisible()) {
    switch(event->key()) {
    case Qt::Key_Enter:
    case Qt::Key_Return:
    case Qt::Key_Escape:
    case Qt::Key_Tab:
    case Qt::Key_Backtab:
      event->ignore();
      return;
    default:
      break;
    }
  }

  // Gestion de l'auto-indentation
  if (event->modifiers() == Qt::NoModifier &&
      (event->key() == Qt::Key_Enter
    || event->key() == Qt::Key_Return)) {

    QTextCursor cur = textCursor();
    cur.select(QTextCursor::LineUnderCursor);
    QString ligne = cur.selectedText();
    QString ind;

    for (int i=0; i<ligne.length() &&
                  (ligne[i] == ' ' || ligne[i] == '\t'); i++) {
      ind += ligne[i];
    }

    QTextEdit::keyPressEvent(event);
    cur = textCursor();
    cur.insertText(ind);
    return;
  }

  // Combinaison spéciales
  if (event->modifiers() & Qt::ControlModifier && event->key() == Qt::Key_D) {
    // Supprimer une ligne
    if (completer->popup()->isVisible()) {
      completer->popup()->hide();
    }
    QTextCursor cur = textCursor();
    cur.movePosition(QTextCursor::StartOfLine);
    cur.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
    if (!cur.movePosition(QTextCursor::NextCharacter,
                          QTextCursor::KeepAnchor)) {
      cur.movePosition(QTextCursor::StartOfLine);
      cur.movePosition(QTextCursor::PreviousCharacter);
      cur.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
      cur.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
    }

    cur.removeSelectedText();
    return;
  }

  // the shortcut to pop up the completer
  bool isShortcut = ((event->modifiers() & Qt::ControlModifier)
                     && event->key() == Qt::Key_Space);
  if(!isShortcut) {
    QTextEdit::keyPressEvent(event);
  }

  QString completionPrefix = textUnderCursor();
  if (event->key() == Qt::Key_Backspace && completer->popup()->isHidden()) {
    completer->popup()->hide();
    event->accept();
    return;
  }

  bool ctrl = event->modifiers() & Qt::ControlModifier;

  // if the user just pressed Control or Shift (nothing else)
  if (ctrl && !isShortcut)
    return;

  bool hasModifier = (event->modifiers() != Qt::NoModifier);

  // if lastChar is not a letter, the popup will not be shown
  QChar lastChar;
  if(!completionPrefix.isEmpty())
    lastChar = completionPrefix.right(1).at(0);

  bool followsTable = false;
  if(lastChar == '.')
  {
    QString table = completionPrefix.left(completionPrefix.length()-1);
    followsTable = tables.contains(table);
  }

  if(!isShortcut && ((!lastChar.isLetterOrNumber() && !followsTable) ||
                     hasModifier ||
                     event->text().isEmpty() ||
                     completionPrefix.length() < Config::compCharCount))
  {
    completer->popup()->hide();
    return;
  }

  if(completionPrefix != completer->completionPrefix())
  {
    completer->setCompletionPrefix(completionPrefix);
    completer->popup()->setCurrentIndex(
        completer->completionModel()->index(0,0));
  }

  QRect cr = cursorRect();
  cr.setWidth(completer->popup()->sizeHintForColumn(0) +
              completer->popup()->verticalScrollBar()->width());

  completer->complete(cr);
}

void QueryTextEdit::insertCompletion(QString text)
{
  if(completer->widget() != this)
    return;

  QTextCursor tc = textCursor();
  tc.select(QTextCursor::WordUnderCursor);
  tc.removeSelectedText();
  tc.insertHtml(text);
}

void QueryTextEdit::insertFromMimeData(const QMimeData *source) {
  QTextEdit::insertPlainText(source->text());
}

void QueryTextEdit::reloadCompleter()
{
  SqlHighlighter::reloadKeywords();
}

void QueryTextEdit::reloadContext(QStringList tables,
                                  QMultiMap<QString, QString> fields)
{
  if(!Config::editorSemantic)
    return;

  this->tables = tables;

  // The syntax highlighting must reload the context too
  syntax->reloadContext(tables, fields);

  // collects all items to show
  QStringList items = tables;
  items << fields.values();

  items << SqlHighlighter::sqlFunctionList();
  items << SqlHighlighter::sqlKeywordList();
  items << SqlHighlighter::sqlTypeList();

  // cleaning and sorting
  items.removeDuplicates();

  QMap<QString, QString> m;
  foreach(QString i, items)
    m.insert(i.toLower(), i);

  items = m.values();
  completerContextModel->setStringList(items);
  completer->setModel(completerContextModel);
}

/**
 * Replaces all tables by links
 */
void QueryTextEdit::scanTables()
{
  QString newstr;
  int pos = 0;
  QString text = toHtml();
  QTextCursor tc;
  foreach(QString table, tables)
  {
    pos = text.indexOf(table);
    while(pos != -1)
    {
      tc = textCursor();
      tc.setPosition(pos+1);
      tc.select(QTextCursor::WordUnderCursor);
      newstr = QString("<a href=\"table://%1\">%1</a>").arg(table);
      tc.removeSelectedText();
      tc.insertHtml(newstr);
      text = toPlainText();
      pos += newstr.size();
    }
  }
}

void QueryTextEdit::setupCompleter()
{
  completer = new QCompleter(this);
  completer->setCaseSensitivity(Qt::CaseInsensitive);
  completer->setWrapAround(false);
  completer->setWidget(this);
  completer->setCompletionMode(QCompleter::PopupCompletion);

  connect(completer, SIGNAL(activated(QString)),
          this, SLOT(insertCompletion(QString)));

  QStringList items;
  items << SqlHighlighter::sqlKeywordList()
      << SqlHighlighter::sqlFunctionList()
      << SqlHighlighter::sqlTypeList();
  completerContextModel = new QStringListModel(items, this);
  completer->setModel(completerContextModel);

  reloadContext(QStringList(), QMultiMap<QString, QString>());
}

QString QueryTextEdit::textUnderCursor() const
{
  QTextCursor tc = textCursor();
  tc.select( QTextCursor::WordUnderCursor );
  return tc.selectedText();
}
