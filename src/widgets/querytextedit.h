#ifndef QUERYTEXTEDIT_H
#define QUERYTEXTEDIT_H

#include "../sqlhighlighter.h"

#include <QCompleter>
#include <QStringListModel>

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
  void setupCompleter();
  QString textUnderCursor() const;

  QCompleter *completer;

  QStringListModel *completerContextModel;
  QStringList tables;
  SqlHighlighter *syntax;

private slots:
  void cleanTables();
  void insertCompletion(QString text);
  void scanTables();
  void tabIndent();
  void tabUnindent();
  void updateTabSize();
};

#endif // QUERYTEXTEDIT_H
